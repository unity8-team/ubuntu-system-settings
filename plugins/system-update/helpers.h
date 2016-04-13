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
#ifndef UPDATEHELPERS_H
#define UPDATEHELPERS_H

#include <sstream>

namespace UpdatePlugin {

class Helpers
{
public:
    static std::vector<std::string> getAvailableFrameworks();
    static std::string getArchitecture();
    static std::string architectureFromDpkg();
    static std::vector<std::string> listFolder(const std::string &folder,
                                               const std::string &pattern);
    static QString clickMetadataURL(); // "https://search.apps.ubuntu.com/api/v1/click-metadata";
    static QString clickTokenUrl(const QString &url);
    static bool isIgnoringCredentials(); // IGNORE_CREDENTIALS
    static QString whichClick(); // // CLICK_COMMAND

};

} // Namespace UpdatePlugin

#endif // UPDATEHELPERS_H
