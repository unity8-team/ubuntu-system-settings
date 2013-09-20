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

#include <QDebug>

#include <gio/gio.h>
#include <glib.h>

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QVariant>
#include "storageabout.h"
#include <hybris/properties/properties.h>

struct MeasureData {
    uint *finished;
    StorageAbout *object;
    quint64 *size;
    MeasureData (uint *finished,
                 StorageAbout *object,
                 quint64 *size):
        finished(finished),
        object(object),
        size(size) {}
};

static void measure_file(const char * filename,
                         GAsyncReadyCallback callback,
                         gpointer user_data)
{
    GFile *file = g_file_new_for_path (filename);

    g_file_measure_disk_usage_async (
                file,
                G_FILE_MEASURE_NONE,
                G_PRIORITY_LOW,
                NULL, /* cancellable */
                NULL, /* progress_callback */
                NULL, /* progress_data */
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
    // Cheesy method to determine if all the async requests have finished.
    (*data->finished)++;

    if (*data->finished == 4) {
        Q_EMIT (data->object->sizeReady());
        delete data->finished;
    }

    delete data;

}

static void measure_finished(GObject *source_object,
                             GAsyncResult *result,
                             gpointer user_data)
{
    GError *err = NULL;
    GFile *file = G_FILE (source_object);

    MeasureData *data = (MeasureData *) user_data;

    guint64 *size = (guint64 *) data->size;

    g_file_measure_disk_usage_finish (
                file,
                result,
                size,
                NULL, /* num_dirs */
                NULL, /* num_files */
                &err);

    if (err != NULL) {
        qWarning() << "Measuring of" << g_file_get_path (file) << "failed:"
                      << err->message;
    }

    g_object_unref (file);
    maybeEmit(data);
}

StorageAbout::StorageAbout(QObject *parent) :
    QObject(parent),
    m_clickModel(),
    m_clickFilterProxy(&m_clickModel)
{
    uint *finished = new uint(0);

    measure_special_file(
                G_USER_DIRECTORY_VIDEOS,
                measure_finished,
                new MeasureData(finished, this, &m_moviesSize));

    measure_special_file(
                G_USER_DIRECTORY_MUSIC,
                measure_finished,
                new MeasureData(finished, this, &m_audioSize));

    measure_special_file(
                G_USER_DIRECTORY_PICTURES,
                measure_finished,
                new MeasureData(finished, this, &m_picturesSize));

    measure_file(
                g_get_home_dir(),
                measure_finished,
                new MeasureData(finished, this, &m_homeSize));
}

QString StorageAbout::serialNumber()
{
    static char serialBuffer[PROP_NAME_MAX];

    if (m_serialNumber.isEmpty() || m_serialNumber.isNull())
    {
        property_get("ro.serialno", serialBuffer, "");
        m_serialNumber = QString(serialBuffer);
    }

    return m_serialNumber;
}

QString StorageAbout::vendorString()
{
    static char manufacturerBuffer[PROP_NAME_MAX];
    static char modelBuffer[PROP_NAME_MAX];

    if (m_vendorString.isEmpty() || m_vendorString.isNull())
    {
        property_get("ro.product.manufacturer", manufacturerBuffer, "");
        property_get("ro.product.model", modelBuffer, "");
        m_vendorString = QString("%1 %2").arg(manufacturerBuffer).arg(modelBuffer);
    }

    return m_vendorString;
}

QString StorageAbout::updateDate()
{
    if (m_updateDate.isEmpty() || m_updateDate.isNull())
    {
        QFile file("/userdata/.last_update");
        if (!file.exists())
            return "";
        m_updateDate = QFileInfo(file).created().toString("yyyy-MM-dd");
    }

    return m_updateDate;
}

QString StorageAbout::licenseInfo(const QString &subdir) const
{

    QString copyright = "/usr/share/doc/" + subdir + "/copyright";
    QString copyrightText;

    QFile file(copyright);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
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

    const char * formatted_size = g_format_size (g_size);

    return QString::fromLocal8Bit(formatted_size);
}

StorageAbout::~StorageAbout() {
}
