/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013-2016 Canonical Ltd.
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

#ifndef SYSTEM_UPDATE_HELPERS_H
#define SYSTEM_UPDATE_HELPERS_H

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
    static QString clickMetadataUrl();
    static bool isIgnoringCredentials();
    static QString whichClick();
    static QString whichPkcon();
private:
    static std::string architectureFromDpkg();
    static std::vector<std::string> listFolder(const std::string &folder,
                                               const std::string &pattern);
};
} // UpdatePlugin

#endif // SYSTEM_UPDATE_HELPERS_H
