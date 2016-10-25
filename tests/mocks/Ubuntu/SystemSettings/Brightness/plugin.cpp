/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
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

#include "plugin.h"
#include "MockBrightness.h"
#include "MockAethercastDevice.h"
#include "MockAethercastDeviceModel.h"
#include "MockAethercastDisplays.h"

#include "MockDisplay.h"
#include "MockDisplayModel.h"

#include <QtQml>

void BackendPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Ubuntu.SystemSettings.Brightness"));
    qmlRegisterType<MockBrightness>(uri, 1, 0, "UbuntuBrightnessPanel");
    qmlRegisterType<MockAethercastDevice>(uri, 1, 0, "AethercastDevice");
    qmlRegisterType<MockAethercastDisplays>(uri, 1, 0, "AethercastDisplays");

    qmlRegisterUncreatableType<MockDisplay>(uri, 1, 0, "Display", "Not to be instantiated directly.");
    qmlRegisterType<MockDisplayModel>(uri, 1, 0, "DisplayModel");
}
