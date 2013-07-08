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
#include <QFile>
#include <QtCore/QSettings>
#include <hybris/properties/properties.h>

StorageAbout::StorageAbout(QObject *parent) :
    QObject(parent)
{

}

QString StorageAbout::OsVersion()
{
    if (m_OsVersion.isEmpty() || m_OsVersion.isNull())
    {
        QString OsRelease = "/etc/os-release";
        QFile file(OsRelease);
        if (file.exists())
        {
            QSettings settings(OsRelease, QSettings::IniFormat);
            m_OsVersion = QString("%1 %2").arg(settings.value("NAME").toString()).arg(settings.value("VERSION_ID").toString());
        }
        else
            qWarning("Can't read os-release informations");
    }

    return m_OsVersion;
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

StorageAbout::~StorageAbout() {
}
