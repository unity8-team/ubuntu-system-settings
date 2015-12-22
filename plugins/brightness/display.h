/*
 * Copyright (C) 2015 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Jonas G. Drange <jonas.drange@canonical.com>
 *
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <QDBusInterface>
#include <QObject>
#include <QScreen>
#include <mir_toolkit/client_types.h>

struct Display : QObject
{
    Q_OBJECT

public:

    Display() {}
    ~Display() {}
    Display(MirDisplayOutput *output);
    Q_PROPERTY( QString name
                READ name
                CONSTANT )
    Q_PROPERTY( bool enabled
                READ enabled
                WRITE setEnabled
                NOTIFY enabledChanged )
    Q_PROPERTY( bool connected
                READ connected
                NOTIFY connectedChanged )
    Q_PROPERTY( QStringList availableModes
                READ availableModes
                NOTIFY availableModesChanged )
    Q_PROPERTY( QString mode
                READ mode
                WRITE setMode
                NOTIFY modeChanged )
    Q_PROPERTY( Orientation orientation
                READ orientation
                WRITE setOrientation
                NOTIFY orientationChanged )

    enum Orientation { Normal, Left, Inverted, Right };
    Q_ENUMS(Orientation)

    QString name() const;

    bool enabled() const;
    void setEnabled(const bool &enabled);

    bool connected() const;

    QStringList availableModes() const;

    QString mode() const;
    void setMode(const QString &mode);

    Orientation orientation() const;
    void setOrientation(const Orientation &orientation);

    MirDisplayOutput * output() const;

Q_SIGNALS:
    void enabledChanged();
    void connectedChanged();
    void availableModesChanged();
    void modeChanged();
    void orientationChanged();

private:

    void updateModes();
    void updateOrientation();
    void updateSizes();

    MirDisplayOutput *m_mirOutput;
    QString m_name;
    QSize m_size;
    QSizeF m_physicalSize;
    qreal m_refreshRate;
    MirOrientation m_orientation;
    QStringList m_availableModes;
    int m_currentMode;
    bool m_enabled;
    bool m_connected;
};

Q_DECLARE_METATYPE(Display*)

#endif // DISPLAY_H
