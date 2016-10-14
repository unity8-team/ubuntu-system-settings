/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * isInternal() is Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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
 * Authors: Sebastien Bacher <sebastien.bacher@canonical.com>
 *
*/

#include "storageabout.h"

#include <QDebug>

#include <mntent.h>
#include <sys/stat.h>

#include <gio/gio.h>
#include <gio/gunixmounts.h>
#include <glib.h>

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QtCore/QStorageInfo>
#include <QtCore/QSharedPointer>
#include <QtGlobal>
#include <QProcess>
#include <QVariant>
#include <hybris/properties/properties.h>
#include <QDBusReply>

namespace {
    const QString PROPERTY_SERVICE_PATH = "/com/canonical/PropertyService";
    const QString PROPERTY_SERVICE_OBJ = "com.canonical.PropertyService";
}

struct MeasureData {
    QSharedPointer<quint32> running;
    StorageAbout *object;
    quint64 *size;
    GCancellable *cancellable;
    MeasureData (QSharedPointer<quint32> running,
                 StorageAbout *object,
                 quint64 *size,
                 GCancellable *cancellable):
        running(running),
        object(object),
        size(size),
        cancellable(cancellable){
            ++(*running);
        }
};

static void measure_file(const char * filename,
                         GAsyncReadyCallback callback,
                         gpointer user_data)
{
    auto *data = static_cast<MeasureData *>(user_data);

    GFile *file = g_file_new_for_path (filename);

    g_file_measure_disk_usage_async (
                file,
                G_FILE_MEASURE_NONE,
                G_PRIORITY_LOW,
                data->cancellable, /* cancellable */
                nullptr, /* progress_callback */
                nullptr, /* progress_data */
                callback,
                user_data);

}

static void measure_special_file(GUserDirectory directory,
                                 GAsyncReadyCallback callback,
                                 gpointer user_data)
{
    measure_file (g_get_user_special_dir (directory), callback, user_data);
}

static void measure_finished(GObject *source_object,
                             GAsyncResult *result,
                             gpointer user_data)
{
    GError *err = nullptr;
    GFile *file = G_FILE (source_object);

    auto data = static_cast<MeasureData *>(user_data);

    guint64 *size = (guint64 *) data->size;

    g_file_measure_disk_usage_finish (
                file,
                result,
                size,
                nullptr, /* num_dirs */
                nullptr, /* num_files */
                &err);

    if (err != nullptr) {
        if (g_error_matches (err, G_IO_ERROR, G_IO_ERROR_CANCELLED)) {
            delete data;
            g_clear_object (&file);
            g_error_free (err);
            err = nullptr;
            return;
        } else {
            qWarning() << "Measuring of" << g_file_get_path (file)
                       << "failed:" << err->message;
            g_error_free (err);
            err = nullptr;
        }
    }

    if (--(*data->running) == 0)
        Q_EMIT (data->object->sizeReady());

    delete data;
    g_clear_object (&file);
}

StorageAbout::StorageAbout(QObject *parent) :
    QObject(parent),
    m_clickModel(),
    m_clickFilterProxy(&m_clickModel),
    m_moviesSize(0),
    m_audioSize(0),
    m_picturesSize(0),
    m_otherSize(0),
    m_homeSize(0),
    m_propertyService(new QDBusInterface(PROPERTY_SERVICE_OBJ,
        PROPERTY_SERVICE_PATH,
        PROPERTY_SERVICE_OBJ,
        QDBusConnection::systemBus())),
    m_cancellable(nullptr)
{
}

QString StorageAbout::serialNumber()
{

    if (m_serialNumber.isEmpty() || m_serialNumber.isNull())
    {
        char serialBuffer[PROP_VALUE_MAX];
        property_get("ro.serialno", serialBuffer, "");
        m_serialNumber = QString(serialBuffer);
    }

    return m_serialNumber;
}

QString StorageAbout::vendorString()
{
    if (m_vendorString.isEmpty() || m_vendorString.isNull())
    {
        char manufacturerBuffer[PROP_VALUE_MAX];
        char modelBuffer[PROP_VALUE_MAX];
        property_get("ro.product.manufacturer", manufacturerBuffer, "");
        property_get("ro.product.model", modelBuffer, "");
        m_vendorString = QString("%1 %2").arg(manufacturerBuffer).arg(modelBuffer);
    }

    return m_vendorString;
}

QString StorageAbout::deviceBuildDisplayID()
{

    if (m_deviceBuildDisplayID.isEmpty() || m_deviceBuildDisplayID.isNull())
    {
        char serialBuffer[PROP_VALUE_MAX];
        property_get("ro.build.display.id", serialBuffer, "");
        m_deviceBuildDisplayID = QString(serialBuffer);
    }

    return m_deviceBuildDisplayID;
}

QString StorageAbout::ubuntuBuildID()
{
    if (m_ubuntuBuildID.isEmpty() || m_ubuntuBuildID.isNull())
    {
        QFile file(qgetenv("SNAP").append("/etc/media-info"));
        if (!file.exists())
            return "";
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        m_ubuntuBuildID = QString(file.readAll());
        file.close();
    }

    return m_ubuntuBuildID;
}

bool StorageAbout::getDeveloperModeCapable() const
{
    QDBusReply<bool> reply = m_propertyService->call("GetProperty", "adb");

    if (reply.isValid())
        return true;
    return false;
}

bool StorageAbout::getDeveloperMode()
{
    QDBusReply<bool> reply = m_propertyService->call("GetProperty", "adb");

    if (reply.isValid()) {
        return reply.value();
    } else {
        qWarning("devMode: no reply from dbus property service");
        return false;
    }
}

void StorageAbout::setDeveloperMode(bool mode)
{
    m_propertyService->call("SetProperty", "adb", mode);
}

QString StorageAbout::licenseInfo(const QString &subdir) const
{

    QString copyright = "doc/" + subdir + "/copyright";
    QString copyrightText;

    QString copyrightFile = QStandardPaths::locate(
        QStandardPaths::GenericDataLocation, copyright,
        QStandardPaths::LocateFile
    );
    if (copyrightFile.isEmpty()) {
        return QString();
    }

    QFile file(copyrightFile);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();
    copyrightText = QString(file.readAll());
    file.close();
    return copyrightText;
}

QAbstractItemModel *StorageAbout::getClickList()
{
    return &m_clickFilterProxy;
}

ClickModel::Roles StorageAbout::getSortRole()
{
    return (ClickModel::Roles) m_clickFilterProxy.sortRole();
}

void StorageAbout::setSortRole(ClickModel::Roles newRole)
{
    m_clickFilterProxy.setSortRole(newRole);

    m_clickFilterProxy.sort(0, newRole == ClickModel::InstalledSizeRole ?
                                Qt::DescendingOrder :
                                Qt::AscendingOrder);
    m_clickFilterProxy.invalidate();
    Q_EMIT(sortRoleChanged());
}

quint64 StorageAbout::getClickSize() const
{
    return m_clickModel.getClickSize();
}

quint64 StorageAbout::getMoviesSize()
{
    return m_moviesSize;
}

quint64 StorageAbout::getAudioSize()
{
    return m_audioSize;
}
quint64 StorageAbout::getPicturesSize()
{
    return m_picturesSize;
}

quint64 StorageAbout::getHomeSize()
{
    return m_homeSize;
}

void StorageAbout::populateSizes()
{
    quint32 *running = new quint32(0);
    QSharedPointer<quint32> running_ptr(running);

    if (!m_cancellable)
        m_cancellable = g_cancellable_new();

    measure_special_file(
                G_USER_DIRECTORY_VIDEOS,
                measure_finished,
                new MeasureData(running_ptr, this, &m_moviesSize,
                                m_cancellable));

    measure_special_file(
                G_USER_DIRECTORY_MUSIC,
                measure_finished,
                new MeasureData(running_ptr, this, &m_audioSize,
                                m_cancellable));

    measure_special_file(
                G_USER_DIRECTORY_PICTURES,
                measure_finished,
                new MeasureData(running_ptr, this, &m_picturesSize,
                                m_cancellable));

    measure_file(
                g_get_home_dir(),
                measure_finished,
                new MeasureData(running_ptr, this, &m_homeSize,
                                m_cancellable));
}

QStringList StorageAbout::getMountedVolumes()
{
    if (m_mountedVolumes.isEmpty())
        prepareMountedVolumes();

    return m_mountedVolumes;
}

void StorageAbout::prepareMountedVolumes()
{
    QStringList checked;

    Q_FOREACH (const QStorageInfo &storage, QStorageInfo::mountedVolumes()) {
        if (storage.isValid() && storage.isReady()) {
            QString drive(storage.rootPath());
            /* Only check devices once */
            if (checked.contains(drive))
                continue;
             
            checked.append(drive);
            QString devicePath(getDevicePath(drive));
            if (devicePath.isEmpty() || m_mountedVolumes.contains(drive))
                continue;

            /* only deal with the device's storage for now, external mounts
               handling would require being smarter on the categories
               computation as well and is not in the current design */
            if (isInternal(drive)) {
                m_mountedVolumes.append(drive);
            }
        }
    }
}

QString StorageAbout::getDevicePath(const QString mount_point) const
{
    QString s_mount_point;
    GUnixMountEntry * g_mount_point = nullptr;

    if (!mount_point.isNull() && !mount_point.isEmpty()) {
        g_mount_point = g_unix_mount_at(mount_point.toLocal8Bit(), nullptr);
    }

    if (g_mount_point) {
        const gchar * device_path =
            g_unix_mount_get_device_path(g_mount_point);
        s_mount_point = QString::fromLocal8Bit(device_path);

        g_unix_mount_free (g_mount_point);
    }

    return s_mount_point;
 }

/* This function was copied from QtSystems, as it was removed when the
 * QSystemInfo class moved to Qt 5.4.
 *
 * The license terms state, in part:
 *
 * Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
 *
 * GNU General Public License Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl.html.
 *
 */
bool StorageAbout::isInternal(const QString &drive) const
{
    bool ret = false;
    FILE *fsDescription = setmntent(_PATH_MOUNTED, "r");
    struct mntent entry;
    char buffer[512];
    while ((getmntent_r(fsDescription, &entry, buffer, sizeof(buffer))) != NULL) {
        if (drive != QString::fromLatin1(entry.mnt_dir))
            continue;

        if (strcmp(entry.mnt_type, "binfmt_misc") == 0
            || strcmp(entry.mnt_type, "debugfs") == 0
            || strcmp(entry.mnt_type, "devpts") == 0
            || strcmp(entry.mnt_type, "devtmpfs") == 0
            || strcmp(entry.mnt_type, "fusectl") == 0
            || strcmp(entry.mnt_type, "none") == 0
            || strcmp(entry.mnt_type, "proc") == 0
            || strcmp(entry.mnt_type, "ramfs") == 0
            || strcmp(entry.mnt_type, "securityfs") == 0
            || strcmp(entry.mnt_type, "sysfs") == 0
            || strcmp(entry.mnt_type, "tmpfs") == 0
            || strcmp(entry.mnt_type, "cifs") == 0
            || strcmp(entry.mnt_type, "ncpfs") == 0
            || strcmp(entry.mnt_type, "nfs") == 0
            || strcmp(entry.mnt_type, "nfs4") == 0
            || strcmp(entry.mnt_type, "smbfs") == 0
            || strcmp(entry.mnt_type, "iso9660") == 0) {
            ret = false;
            break;
        }

        if (strcmp(entry.mnt_type, "rootfs") == 0
            || strcmp(entry.mnt_type, "ext4") == 0) {
            ret = true;
            break;
        }

        // Now need to guess if it's InternalDrive or RemovableDrive
        QString fsName = QDir(entry.mnt_fsname).canonicalPath();
        if (fsName.contains(QString(QStringLiteral("mapper")))) {
            struct stat status;
            stat(entry.mnt_fsname, &status);
            fsName = QString(QStringLiteral("/sys/block/dm-%1/removable")).arg(status.st_rdev & 0377);
        } else {
            fsName = fsName.section(QString(QStringLiteral("/")), 2, 3);
            if (!fsName.isEmpty()) {
                if (fsName.length() > 3) {
                    // only take the parent of the device
                    int index_mmc = fsName.indexOf("mmc",0,Qt::CaseInsensitive);
                    if (index_mmc != -1) {
                        QString mmcString;
                        int index_p = fsName.indexOf('p',index_mmc,Qt::CaseInsensitive);
                        mmcString = fsName.mid(index_mmc, index_p - index_mmc);

                        // "removable" attribute is set only for removable media, and we may have internal mmc cards
                        fsName = QString(QStringLiteral("/sys/block/")) + mmcString + QString(QStringLiteral("/device/uevent"));
                        QFile file(fsName);
                        if (file.open(QIODevice::ReadOnly)) {
                            QByteArray buf = file.readLine();
                            while (buf.size() > 0) {
                                if (qstrncmp(buf.constData(), "MMC_TYPE=", 9) == 0) {
                                    if (qstrncmp(buf.constData() + 9, "MMC", 3) == 0)
                                        ret = true;
                                    else if (qstrncmp(buf.constData() + 9, "SD", 2) == 0)
                                        ret = false;
                                    if (ret) {
                                        endmntent(fsDescription);
                                        return ret;
                                    }
                                    break;  // fall back to check the "removable" attribute
                                }
                                buf = file.readLine();
                            }
                        }
                    }
                }
                fsName = QString(QStringLiteral("/sys/block/")) + fsName + QString(QStringLiteral("/removable"));
            }
        }
        QFile removable(fsName);
        char isRemovable;
        if (!removable.open(QIODevice::ReadOnly) || 1 != removable.read(&isRemovable, 1))
            break;
        if (isRemovable == '0')
            ret = true;
        else
            ret = false;
        break;
    }

    endmntent(fsDescription);
    return ret;
}

qint64 StorageAbout::getFreeSpace(const QString mount_point)
{
    QStorageInfo si(mount_point);
    if (si.isValid())
        return si.bytesFree();

    return -1;
}

qint64 StorageAbout::getTotalSpace(const QString mount_point)
{
    QStorageInfo si(mount_point);
    if (si.isValid())
        return si.bytesTotal();

    return -1;
}

StorageAbout::~StorageAbout() {
    if (m_cancellable) {
        g_cancellable_cancel(m_cancellable);
        g_clear_object(&m_cancellable);
    }
}
