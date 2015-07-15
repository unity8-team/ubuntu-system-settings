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

class Display : public QObject
{
    Q_OBJECT
    Q_ENUMS(OrientationMode)
    Q_PROPERTY( QString path
                READ path
                WRITE setPath
                NOTIFY pathChanged )
    Q_PROPERTY( bool enabled
                READ enabled
                WRITE setEnabled
                NOTIFY enabledChanged )
    Q_PROPERTY( QStringList availableResolutions
                READ availableResolutions
                NOTIFY availableResolutionsChanged )
    Q_PROPERTY( QString resolution
                READ resolution
                WRITE setResolution
                NOTIFY resolutionChanged )
    Q_PROPERTY( OrientationMode orientation
                READ orientation
                WRITE setOrientation
                NOTIFY orientationChanged )
    Q_PROPERTY( float scale
                READ scale
                WRITE setScale
                NOTIFY scaleChanged )
    Q_PROPERTY( bool connected
                READ connected
                NOTIFY connectedChanged )

public:
    enum OrientationMode {
        PortraitMode,
        LandscapeMode,
        PortraitInvertedMode,
        LandscapeInvertedMode,
        PortraitAnyMode,
        LandscapeAnyMode,
        AnyMode
    };

    explicit Display(QObject *parent = 0);
    ~Display();

    QString path() const;
    void setPath(const QString &path);

    bool enabled() const;
    void setEnabled(const bool &enabled);

    QStringList availableResolutions() const;

    QString resolution() const;
    void setResolution(const QString &resolution);

    OrientationMode orientation() const;
    void setOrientation(const Display::OrientationMode &mode);

    float scale() const;
    void setScale(const float &scale);

    bool connected() const;

Q_SIGNALS:
    void pathChanged(const QString &path);
    void enabledChanged(const bool enabled);
    void availableResolutionsChanged(const QStringList &resolutions);
    void resolutionChanged(const QString &resolution);
    void orientationChanged(const Display::OrientationMode &mode);
    void scaleChanged(float scale);
    void connectedChanged(bool connected);

private:
    QDBusConnection m_systemBusConnection;
    QDBusInterface m_unityInterface;
    QString m_path;
    bool m_enabled;
    QStringList m_availableResolutions;
    QString m_resolution;
    OrientationMode m_orientation;
    float m_scale;
    bool m_connected;

};

Q_DECLARE_METATYPE (Display::OrientationMode)

#endif // DISPLAY_H
