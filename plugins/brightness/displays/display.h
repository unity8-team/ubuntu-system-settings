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

#ifndef DISPLAY_H
#define DISPLAY_H

#include "enums.h"
#include "output/output.h"
#include "../../../src/i18n.h"

#include <mir_toolkit/mir_client_library.h>

#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QSharedPointer>
#include <QVariantMap>

namespace DisplayPlugin
{
class Q_DECL_EXPORT DisplayMode
{
public:
    DisplayMode() {};
    explicit DisplayMode(const MirDisplayMode &mirMode)
        : vertical_resolution(mirMode.vertical_resolution)
        , horizontal_resolution(mirMode.horizontal_resolution)
        , refresh_rate(mirMode.refresh_rate) {};
    uint vertical_resolution = 0;
    uint horizontal_resolution = 0;
    double refresh_rate = 0.0;
    Q_INVOKABLE QString toString() const;
    bool operator==(const DisplayMode &other) const;
};

class Q_DECL_EXPORT Display : public QObject
{
    Q_OBJECT
    Q_PROPERTY(uint id READ id CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString type READ type CONSTANT)
    Q_PROPERTY(bool mirrored READ mirrored WRITE setMirrored
               NOTIFY mirroredChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled
               NOTIFY enabledChanged)
    Q_PROPERTY(uint mode READ mode WRITE setMode
               NOTIFY modeChanged)
    Q_PROPERTY(QStringList modes READ modes
               NOTIFY modesChanged)
    Q_PROPERTY(Enums::Orientation orientation READ orientation WRITE setOrientation
               NOTIFY orientationChanged)
    Q_PROPERTY(double scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(bool uncommittedChanges READ uncommittedChanges
               NOTIFY uncommittedChangesChanged)
    Q_PROPERTY(uint physicalWidthMm READ physicalWidthMm CONSTANT)
    Q_PROPERTY(uint physicalHeightMm READ physicalHeightMm CONSTANT)

Q_SIGNALS:
    void mirroredChanged();
    void connectedChanged();
    void enabledChanged();
    void modeChanged();
    void modesChanged();
    void orientationChanged();
    void scaleChanged();
    void uncommittedChangesChanged();
    void displayChanged(const Display *display);

public:
    explicit Display(QObject *parent = Q_NULLPTR);
    explicit Display(QSharedPointer<Output> output, QObject *parent = Q_NULLPTR);
    explicit Display(const uint &id);
    ~Display() {};

    uint id() const;
    QString name() const;
    QString type() const;
    bool mirrored() const;
    bool connected() const;
    bool enabled() const;
    uint mode() const;
    QList<DisplayMode> availableModes() const;
    QStringList modes() const;
    Enums::Orientation orientation() const;
    double scale() const;
    bool uncommittedChanges() const;
    uint physicalWidthMm() const;
    uint physicalHeightMm() const;
    Enums::PowerMode powerMode() const;

    void setMirrored(const bool &mirrored);
    void setEnabled(const bool &enabled);
    void setMode(const uint &mode);
    void setOrientation(const Enums::Orientation &orientation);
    void setScale(const double &scale);

protected:
    void setUncommitedChanges(const bool uncommittedChanges);
    void setConnected(const bool &connected);
    void storeConfiguration();

    uint m_id = 0;
    QString m_name = QString::null;
    QString m_type = QString::null;
    bool m_mirrored = false;
    bool m_connected = false;
    bool m_enabled = false;
    DisplayMode m_mode;
    QList<DisplayMode> m_modes;
    Enums::Orientation m_orientation = Enums::Orientation::NormalOrientation;
    double m_scale = 1;
    bool m_uncommittedChanges = false;
    uint m_physicalWidthMm = 0;
    uint m_physicalHeightMm = 0;
    Enums::PowerMode m_powerMode = Enums::PowerMode::OffMode;
    QSharedPointer<Output> m_output = QSharedPointer<Output>(Q_NULLPTR);

protected slots:
    void changedSlot();

private:
    void initialize();
    bool hasChanged() const;

    QVariantMap m_storedConfig;
};

} // DisplayPlugin

Q_DECLARE_METATYPE(DisplayPlugin::Display*)
// Q_DECLARE_METATYPE(DisplayPlugin::Display::Orientation)

#endif // DISPLAY_H
