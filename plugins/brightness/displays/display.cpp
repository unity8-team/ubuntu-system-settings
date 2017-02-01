/*
 * This file is part of system-settings
 *
 * Copyright (C) 2017 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "display.h"
#include "helpers.h"

#include <QDebug>

namespace DisplayPlugin
{
Display::Display(QSharedPointer<Output> output, QObject *parent)
    : QObject(parent)
    , m_output(output)
{
    QObject::connect(this, SIGNAL(enabledChanged()),
                     this, SLOT(changedSlot()));
    QObject::connect(this, SIGNAL(modeChanged()),
                     this, SLOT(changedSlot()));
    QObject::connect(this, SIGNAL(orientationChanged()),
                     this, SLOT(changedSlot()));
    QObject::connect(this, SIGNAL(scaleChanged()),
                     this, SLOT(changedSlot()));
    QObject::connect(this, SIGNAL(uncommittedChangesChanged()),
                     this, SLOT(changedSlot()));

    storeConfiguration();
    changedSlot();
}

void Display::storeConfiguration()
{
    m_storedConfig["enabled"] = enabled();
    m_storedConfig["mode"] = QVariant::fromValue(mode());
    m_storedConfig["orientation"] = QVariant::fromValue(orientation());
    m_storedConfig["scale"] = scale();
}

bool Display::hasChanged() const
{
    bool considerModes = availableModes().size() > 0;
    return (
        m_storedConfig["enabled"].toBool() != enabled()
        || (considerModes && (
            m_storedConfig["mode"].value<QSharedPointer<OutputMode>>()->toString() != mode()->toString()))
        || m_storedConfig["orientation"].value<Enums::Orientation>() != orientation()
        || m_storedConfig["scale"].toFloat() != scale()
    );
}

int Display::id() const
{
    return m_output->getId();
}

QString Display::name() const
{
    return m_name;
}

QString Display::type() const
{
    return Helpers::typeToString(m_output->getType());
}

bool Display::connected() const
{
    return m_output->getConnectionState() == Enums::ConnectionState::Connected;
}

bool Display::enabled() const
{
    return m_output->isEnabled();
}

QSharedPointer<OutputMode> Display::mode() const
{
    return m_output->getCurrentMode();
}

QList<QSharedPointer<OutputMode>> Display::availableModes() const
{
    return m_output->getAvailableModes();
}

Enums::Orientation Display::orientation() const
{
    return m_output->getOrientation();
}

float Display::scale() const
{
    return m_output->getScaleFactor();
}

bool Display::uncommittedChanges() const
{
    return m_uncommittedChanges;
}

uint Display::physicalWidthMm() const
{
    return (uint) m_output->getPhysicalWidthMm();
}

uint Display::physicalHeightMm() const
{
    return (uint) m_output->getPhysicalHeightMm();
}

Enums::PowerMode Display::powerMode() const
{
    return m_output->getPowerMode();
}

void Display::setEnabled(const bool &enabled)
{
    bool wasEnabled = m_output->isEnabled();
    m_output->setEnabled(enabled);

    if (wasEnabled != enabled) {
        Q_EMIT enabledChanged();
    }
}

void Display::setMode(const QSharedPointer<OutputMode> &mode)
{
    if (mode != this->mode()) {
        m_output->setCurrentMode(mode);
        Q_EMIT modeChanged();
    }
}

void Display::setOrientation(const Enums::Orientation &orientation)
{
    if (orientation != this->orientation()) {
        m_output->setOrientation(orientation);
        Q_EMIT orientationChanged();
    }
}

void Display::setScale(const float &scale)
{
    if (scale != this->scale()) {
        m_output->setScaleFactor(scale);
        Q_EMIT scaleChanged();
    }
}

void Display::setUncommitedChanges(const bool uncommittedChanges)
{
    if (m_uncommittedChanges != uncommittedChanges) {
        m_uncommittedChanges = uncommittedChanges;
        Q_EMIT uncommittedChangesChanged();
    }
}

void Display::changedSlot()
{
    setUncommitedChanges(hasChanged());
    Q_EMIT displayChanged(this);
}
} // DisplayPlugin
