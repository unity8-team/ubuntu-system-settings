/*
 * Copyright (C) 2016 Canonical, Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3, as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranties of MERCHANTABILITY,
 * SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "qquickscreenwindow.h"

// mirserver
#include "screen.h"
#include "screenscontroller.h"

// Qt
#include <QGuiApplication>
#include <QScreen>
#include <qpa/qplatformnativeinterface.h>
#include <QDebug>

using namespace qtmir;

/*
 * QQuickScreenWindow - wrapper of QQuickWindow to enable QML to specify destination screen
 * and read Mir-specific properties of that screen like scale & form factor
 **/

/*
 * Small implementation detail: during construction, the backing qtmir::ScreenWindow/QPlatformScreen
 * has not been created yet, so handle() returns nullptr there.
 *
 * The "scale" & "form factor" properties are obtainable only via the QPlatformNativeInterface,
 * which refers to windows by their backing handles. So need to postpone querying these values
 * until after QQuickScreenWindow construction. Handiest approach is to only fetch the value
 * on first read of the property.
 *
 * There's no problem connecting to QPlatformNativeInterface::windowPropertyChanged in the
 * constructor however.
 */
QQuickScreenWindow::QQuickScreenWindow(QQuickWindow *parent)
    : QQuickWindow(parent)
    , m_scale(-1.0) // start with invalid initial state, fetch correct value on first invokation
    , m_formFactor(Screens::FormFactorUnknown)
{
    if (qGuiApp->platformName() == QLatin1String("mirserver")) {
        connect(qGuiApp->platformNativeInterface(), &QPlatformNativeInterface::windowPropertyChanged,
                this, &QQuickScreenWindow::nativePropertyChanged);
        //m_scale = getScaleNativeProperty(); DO NOT CALL HERE - see note above
        //m_formFactor = getFormFactorNativeProperty(); ditto
    } else {
        qCritical("Not using 'mirserver' QPA plugin, the Unity.Screens plugin will be useless!");
    }
}

QScreen *QQuickScreenWindow::screen() const
{
    return QQuickWindow::screen();
}

void QQuickScreenWindow::setScreen(QScreen *screen)
{
    QQuickWindow::setScreen(screen);

    float scale = getScaleNativeProperty();
    if (!qFuzzyCompare(m_scale, scale)) {
        m_scale = scale;
        Q_EMIT scaleChanged(m_scale);
    }

    auto formFactor = getFormFactorNativeProperty();
    if (formFactor != m_formFactor) {
        m_formFactor = formFactor;
        Q_EMIT formFactorChanged(m_formFactor);
    }
}

qreal QQuickScreenWindow::scale()
{
    if (m_scale < 0) {
        m_scale = getScaleNativeProperty();
    }
     // am keeping local copy, to avoid emitting changed signal if screen changes but scale doesn't.
    return m_scale;
}

bool QQuickScreenWindow::setScaleAndFormFactor(const float scale, const Screens::FormFactor formFactor)
{
    if (qFuzzyCompare(scale, m_scale) && formFactor == m_formFactor) {
        return true;
    }

    // Operates through the mirserver ScreensController API
    auto controller = static_cast<ScreensController*>(qGuiApp->platformNativeInterface()
                                                      ->nativeResourceForIntegration("ScreensController"));
    if (!controller) {
        return false;
    }

    auto screenHandle = static_cast<Screen *>(screen()->handle());
    if (!screenHandle) {
        return false;
    }

    auto id = screenHandle->outputId();

    auto configs = controller->configuration();

    auto config = configs.begin();
    while (config != configs.end()) {
        if (config->id == id) {
            config->scale = scale;
            config->formFactor = static_cast<MirFormFactor>(formFactor);
        }
        config++;
    }

    return controller->setConfiguration(configs);
}

Screens::FormFactor QQuickScreenWindow::formFactor()
{
    if (m_formFactor == Screens::FormFactorUnknown) {
        m_formFactor = getFormFactorNativeProperty();
    }
    return m_formFactor;
}

void QQuickScreenWindow::nativePropertyChanged(QPlatformWindow *window, const QString &propertyName)
{
    if (window != handle()) {
        return;
    }

    if (propertyName == QStringLiteral("scale")) {
        float scale = getScaleNativeProperty();

        if (qFuzzyCompare(m_scale, scale)) {
            return;
        }
        m_scale = scale;
        Q_EMIT scaleChanged(m_scale);
    } else if (propertyName == QStringLiteral("formFactor")) {
        auto formFactor = getFormFactorNativeProperty();

        if (formFactor == m_formFactor) {
            return;
        }
        m_formFactor = formFactor;
        Q_EMIT formFactorChanged(m_formFactor);
    }
}

float QQuickScreenWindow::getScaleNativeProperty() const
{
    QVariant scaleVal = qGuiApp->platformNativeInterface()
                            ->windowProperty(handle(), QStringLiteral("scale"));
    if (!scaleVal.isValid()) {
        return m_scale;
    }
    bool ok;
    float scale = scaleVal.toFloat(&ok);
    if (!ok || scale <= 0) {
        return m_scale;
    }
    return scale;
}

Screens::FormFactor QQuickScreenWindow::getFormFactorNativeProperty() const
{
    QVariant formFactorVal = qGuiApp->platformNativeInterface()
                                ->windowProperty(handle(), QStringLiteral("formFactor"));
    if (!formFactorVal.isValid()) {
        return m_formFactor;
    }

    return static_cast<Screens::FormFactor>(formFactorVal.toInt());
}
