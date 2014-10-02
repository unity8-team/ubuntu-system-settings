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
 * Authors: Sebastien Bacher <sebastien.bacher@canonical.com>
 *
*/

#include "storageabout.h"

#include <QDebug>

#include <gio/gio.h>
#include <gio/gunixmounts.h>
#include <glib.h>
#include <unistd.h>

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
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

static void maybeEmit(MeasureData *data)
{
    if (--(*data->running) == 0)
        Q_EMIT (data->object->sizeReady());

    delete data;
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
            g_error_free(err);
        }
    }

    g_object_unref (file);
    maybeEmit(data);
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

QString StorageAbout::customizationBuildID()
{
    if (m_customizationBuildID.isEmpty() || m_customizationBuildID.isNull())
    {
        QFile file("/custom/build_id");
        if (!file.exists())
            return "";
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        m_customizationBuildID = QString(file.readAll().trimmed());
        file.close();
    }

    return m_customizationBuildID;
}

QString StorageAbout::ubuntuBuildID()
{
    if (m_ubuntuBuildID.isEmpty() || m_ubuntuBuildID.isNull())
    {
        QFile file("/etc/media-info");
        if (!file.exists())
            return "";
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        m_ubuntuBuildID = QString(file.readAll());
        file.close();
    }

    return m_ubuntuBuildID;
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

    QString copyright = "/usr/share/doc/" + subdir + "/copyright";
    QString copyrightText;

    QFile file(copyright);
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

QString StorageAbout::formatSize(quint64 size) const
{
    guint64 g_size = size;

    gchar * formatted_size = g_format_size (g_size);
    QString q_formatted_size = QString::fromLocal8Bit(formatted_size);
    g_free (formatted_size);

    return q_formatted_size;
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

QString StorageAbout::getDevicePath(const QString mount_point)
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

StorageAbout::~StorageAbout() {
    if (m_cancellable) {
        g_cancellable_cancel(m_cancellable);
        g_clear_object(&m_cancellable);
    }
}
