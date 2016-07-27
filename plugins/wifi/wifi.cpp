/*
 * Copyright (C) 2016 Canonical Ltd
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
 */

#include "wifi.h"

#ifdef ANDROID_PROPERTIES
#include <hybris/properties/properties.h>
#endif

Wifi::Wifi(QObject *parent)
    : QObject(parent)
{}

bool Wifi::wapiSupported() const
{
#ifdef ANDROID_PROPERTIES
    char wapi[PROP_VALUE_MAX];
    property_get("ubuntu.wapi.supported", wapi, "0");
    return (strcmp(wapi, "0") > 0);
#else
    return false;
#endif
}
