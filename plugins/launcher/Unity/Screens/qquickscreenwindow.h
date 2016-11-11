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

#ifndef QQUICKSCREENWINDOW_H
#define QQUICKSCREENWINDOW_H

#include <QQuickWindow>
#include "screens.h"

namespace qtmir {

class QQuickScreenWindow : public QQuickWindow
{
    Q_OBJECT

    Q_PROPERTY(QScreen *screen READ screen WRITE setScreen NOTIFY screenChanged)
    Q_PROPERTY(float scale READ scale NOTIFY scaleChanged)
    Q_PROPERTY(Screens::FormFactor formFactor READ formFactor NOTIFY formFactorChanged)

public:
    explicit QQuickScreenWindow(QQuickWindow *parent = 0);

    QScreen *screen() const;
    void setScreen(QScreen *screen);

    qreal scale();
    Screens::FormFactor formFactor();
    Q_INVOKABLE bool setScaleAndFormFactor(const float scale, const Screens::FormFactor formFactor);

Q_SIGNALS:
    void screenChanged(QScreen *screen);
    void scaleChanged(qreal scale);
    void formFactorChanged(Screens::FormFactor arg);

private Q_SLOTS:
    void nativePropertyChanged(QPlatformWindow *window, const QString &propertyName);

private:
    float getScaleNativeProperty() const;
    float m_scale;
    Screens::FormFactor getFormFactorNativeProperty() const;
    Screens::FormFactor m_formFactor;
};

} //namespace qtmir

#endif // QQUICKSCREENWINDOW_H
