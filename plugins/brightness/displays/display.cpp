#include "display.h"

Display::Display(QObject *parent)
    : QObject(parent)
{
    QObject::connect(this, SIGNAL(nameChanged()),
                     this, SLOT(changedSlot()));
    QObject::connect(this, SIGNAL(typeChanged()),
                     this, SLOT(changedSlot()));
    QObject::connect(this, SIGNAL(mirroredChanged()),
                     this, SLOT(changedSlot()));
    QObject::connect(this, SIGNAL(connectedChanged()),
                     this, SLOT(changedSlot()));
    QObject::connect(this, SIGNAL(enabledChanged()),
                     this, SLOT(changedSlot()));
    QObject::connect(this, SIGNAL(modeChanged()),
                     this, SLOT(changedSlot()));
    QObject::connect(this, SIGNAL(availableModesChanged()),
                     this, SLOT(changedSlot()));
    QObject::connect(this, SIGNAL(orientationChanged()),
                     this, SLOT(changedSlot()));
    QObject::connect(this, SIGNAL(scaleChanged()),
                     this, SLOT(changedSlot()));
    QObject::connect(this, SIGNAL(uncommittedChangesChanged()),
                     this, SLOT(changedSlot()));
}

QString Display::name() const
{
    return m_name;
}

QString Display::type() const
{
    return m_type;
}

bool Display::mirrored() const
{
    return m_mirrored;
}

bool Display::connected() const
{
    return m_connected;
}

bool Display::enabled() const
{
    return m_enabled;
}

QString Display::mode() const
{
    return m_mode;
}

QStringList Display::availableModes() const
{
    return m_availableModes;
}

Display::Orientation Display::orientation() const
{
    return m_orientation;
}

double Display::scale() const
{
    return m_scale;
}

bool Display::uncommittedChanges() const
{
    return m_uncommittedChanges;
}

void Display::setName(const QString &name)
{
    m_name = name;
    Q_EMIT nameChanged();
}

void Display::setType(const QString &type)
{
    m_type = type;
    Q_EMIT typeChanged();
}

void Display::setMirrored(const bool &mirrored)
{
    m_mirrored = mirrored;
    Q_EMIT mirroredChanged();
}

void Display::setConnected(const bool &connected)
{
    m_connected = connected;
    Q_EMIT connectedChanged();
}

void Display::setEnabled(const bool &enabled)
{
    m_enabled = enabled;
    Q_EMIT enabledChanged();
}

void Display::setMode(const QString &mode)
{
    m_mode = mode;
    Q_EMIT modeChanged();
}

void Display::setAvailableModes(const QStringList &availableModes)
{
    m_availableModes = availableModes;
    Q_EMIT availableModesChanged();
}

void Display::setOrientation(const Display::Orientation &orientation)
{
    m_orientation = orientation;
    Q_EMIT orientationChanged();
}

void Display::setScale(const double &scale)
{
    m_scale = scale;
    Q_EMIT scaleChanged();
}

void Display::setUncommitedChanges(const bool uncommittedChanges)
{
    m_uncommittedChanges = uncommittedChanges;
    Q_EMIT uncommittedChangesChanged();
}

void Display::changedSlot()
{
    Q_EMIT displayChanged(this);
}
