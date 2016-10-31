#include "display.h"
#include "helpers.h"

#include <QDebug>

Display::Display(QObject *parent)
    : QObject(parent)
{
    initialize();
    // TODO: this constructor shouldn't exist.
}

Display::Display(MirDisplayOutput &output, QObject *parent)
    : QObject(parent)
{
    m_type = DisplayPlugin::Helpers::mirTypeToString(output.type);
    setConnected(output.connected);
    setEnabled(output.used);

    auto modes = QList<DisplayMode>();
    for(uint j = 0; j < output.num_modes; j++) {
        MirDisplayMode mode = output.modes[j];
        // modes.append(QString("%1x%2x%3").arg(mode.horizontal_resolution)
        //                                 .arg(mode.vertical_resolution)
        //                                 .arg(mode.refresh_rate));
        // if (output.current_mode == j) {
        //     setMode()
        // }
    }
    m_modes = modes;
    m_physicalWidthMm = output.physical_width_mm;
    m_physicalHeightMm = output.physical_height_mm;
    m_name = QString("%1").arg(DisplayPlugin::Helpers::mirTypeToString(output.type));

    initialize();
}

void Display::initialize()
{
    QObject::connect(this, SIGNAL(mirroredChanged()),
                     this, SLOT(changedSlot()));
    QObject::connect(this, SIGNAL(connectedChanged()),
                     this, SLOT(changedSlot()));
    QObject::connect(this, SIGNAL(enabledChanged()),
                     this, SLOT(changedSlot()));
    QObject::connect(this, SIGNAL(modeChanged()),
                     this, SLOT(changedSlot()));
    QObject::connect(this, SIGNAL(modesChanged()),
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

uint Display::mode() const
{
    return m_modes.indexOf(m_mode);
}

QStringList Display::modes() const
{
    QStringList modes;
    Q_FOREACH(const DisplayMode &mode, m_modes) {
        modes.append(mode.toString());
    }
    return modes;
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

uint Display::physicalWidthMm() const
{
    return m_physicalWidthMm;
}

uint Display::physicalHeightMm() const
{
    return m_physicalHeightMm;
}

void Display::setMirrored(const bool &mirrored)
{
    if (m_mirrored != mirrored) {
        m_mirrored = mirrored;
        Q_EMIT mirroredChanged();
    }
}

void Display::setConnected(const bool &connected)
{
    if (m_connected != connected) {
        m_connected = connected;
        Q_EMIT connectedChanged();
    }
}

void Display::setEnabled(const bool &enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        Q_EMIT enabledChanged();
    }
}

void Display::setMode(const uint &mode)
{
    m_mode = m_modes.value(mode);
    Q_EMIT modeChanged();
}

void Display::setOrientation(const Display::Orientation &orientation)
{
    if (m_orientation != orientation) {
        m_orientation = orientation;
        Q_EMIT orientationChanged();
    }
}

void Display::setScale(const double &scale)
{
    if (m_scale != scale) {
        m_scale = scale;
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
    qWarning() << "changedSlot";
    Q_EMIT displayChanged(this);
}
