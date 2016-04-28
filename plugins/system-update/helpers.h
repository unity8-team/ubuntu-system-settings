/*
 * Copyright (C) 2013-2016 Canonical Ltd
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
#ifndef PLUGINS_SYSTEM_UPDATE_HELPERS_H_
#define PLUGINS_SYSTEM_UPDATE_HELPERS_H

#include <sstream>
#include <vector>
#include <QString>
#include <QDebug>
#include <QDir>

namespace UpdatePlugin
{

class Helpers
{
public:
    static QString getFrameworksDir();
    static std::vector<std::string> getAvailableFrameworks();
    static std::string getArchitecture();
    static std::string architectureFromDpkg();
    static std::vector<std::string> listFolder(const std::string &folder,
                                               const std::string &pattern);
    static QString clickMetadataUrl();
    static QString clickTokenUrl(const QString &url);
    static bool isIgnoringCredentials();
    static QString whichClick(); // CLICK_COMMAND
    static QString whichPkcon(); // PKCON_COMMAND
};

} // Namespace UpdatePlugin

#endif // PLUGINS_SYSTEM_UPDATE_HELPERS_H_
