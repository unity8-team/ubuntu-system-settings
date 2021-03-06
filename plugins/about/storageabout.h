/*
 * Copyright (C) 2013 Canonical Ltd
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
 * Sebastien Bacher <sebastien.bacher@canonical.com>
 *
*/

#ifndef STORAGEABOUT_H
#define STORAGEABOUT_H

#include <gio/gio.h>
#include <glib.h>

#include <QObject>
#include <QProcess>
#include <QVariant>
#include <QDBusInterface>


class StorageAbout : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QString serialNumber
                READ serialNumber
                CONSTANT)

    Q_PROPERTY( QString vendorString
                READ vendorString
                CONSTANT)

    Q_PROPERTY(QStringList mountedVolumes
               READ getMountedVolumes
               CONSTANT)

    Q_PROPERTY(quint64 moviesSize
               READ getMoviesSize
               NOTIFY sizeReady)

    Q_PROPERTY(quint64 audioSize
               READ getAudioSize
               NOTIFY sizeReady)

    Q_PROPERTY(quint64 picturesSize
               READ getPicturesSize
               NOTIFY sizeReady)

    Q_PROPERTY(quint64 homeSize
               READ getHomeSize
               NOTIFY sizeReady)

    Q_PROPERTY( QString deviceBuildDisplayID
                READ deviceBuildDisplayID
                CONSTANT)

    Q_PROPERTY( QString ubuntuBuildID
                READ ubuntuBuildID
                CONSTANT)

    Q_PROPERTY(bool developerMode
               READ getDeveloperMode
               WRITE setDeveloperMode)
    Q_PROPERTY(bool developerModeCapable
               READ getDeveloperModeCapable
                CONSTANT)

public:
    explicit StorageAbout(QObject *parent = 0);
    ~StorageAbout();
    QString serialNumber();
    QString vendorString();
    QString deviceBuildDisplayID();
    QString ubuntuBuildID();
    Q_INVOKABLE QString licenseInfo(const QString &subdir) const;
    quint64 getMoviesSize();
    quint64 getAudioSize();
    quint64 getPicturesSize();
    quint64 getHomeSize();
    Q_INVOKABLE void populateSizes();
    QStringList getMountedVolumes();
    Q_INVOKABLE QString getDevicePath (const QString mount_point) const;
    Q_INVOKABLE qint64 getFreeSpace (const QString mount_point);
    Q_INVOKABLE qint64 getTotalSpace (const QString mount_point);
    Q_INVOKABLE bool isInternal(const QString &drive) const;
    bool getDeveloperMode();
    void setDeveloperMode(bool newMode);
    bool getDeveloperModeCapable() const;

Q_SIGNALS:
    void sortRoleChanged();
    void sizeReady();

private:
    void prepareMountedVolumes();
    QStringList m_mountedVolumes;
    QString m_serialNumber;
    QString m_vendorString;
    QString m_deviceBuildDisplayID;
    QString m_ubuntuBuildID;
    quint64 m_moviesSize;
    quint64 m_audioSize;
    quint64 m_picturesSize;
    quint64 m_otherSize;
    quint64 m_homeSize;

    QMap<QString, QString> m_mounts;

    QScopedPointer<QDBusInterface> m_propertyService;

    GCancellable *m_cancellable;
};

#endif // STORAGEABOUT_H
