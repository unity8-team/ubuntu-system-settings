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

#include "helpers.h"
#include <QProcessEnvironment>

namespace UpdatePlugin
{

QString Helpers::getFrameworksDir()
{
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    return environment.value("FRAMEWORKS_FOLDER",
            QStringLiteral("/usr/share/click/frameworks/"));
}

std::vector<std::string> Helpers::getAvailableFrameworks()
{
    std::vector<std::string> result;
    for (auto f : listFolder(getFrameworksDir().toStdString(), "*.framework")) {
        result.push_back(f.substr(0, f.size() - 10));
    }
    return result;
}

std::string Helpers::getArchitecture()
{
    static const std::string deb_arch
        { architectureFromDpkg() };
    return deb_arch;
}

std::vector<std::string> Helpers::listFolder(const std::string &folder,
                                             const std::string &pattern)
{
    std::vector<std::string> result;

    QDir dir(QString::fromStdString(folder), QString::fromStdString(pattern),
            QDir::Unsorted, QDir::Readable | QDir::Files);
    QStringList entries = dir.entryList();
    for (int i = 0; i < entries.size(); ++i) {
        QString filename = entries.at(i);
        result.push_back(filename.toStdString());
    }

    return result;
}

std::string Helpers::architectureFromDpkg()
{
    QString program("dpkg");
    QStringList arguments;
    arguments << "--print-architecture";
    QProcess archDetector;
    archDetector.start(program, arguments);
    if (!archDetector.waitForFinished()) {
        qWarning() << "Architecture detection failed.";
    }
    auto output = archDetector.readAllStandardOutput();
    auto ostr = QString::fromUtf8(output);

    return ostr.trimmed().toStdString();
}

QString Helpers::clickMetadataUrl()
{
    QString url = QStringLiteral(
        "https://search.apps.ubuntu.com/api/v1/click-metadata"
    );
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    return environment.value("URL_APPS", url);
}

bool Helpers::isIgnoringCredentials()
{
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    /* If IGNORE_CREDENTIALS is set to something other than an empty string,
    we should ignore credentials. */
    return environment.value("IGNORE_CREDENTIALS", "") != "";
}

QString Helpers::whichClick()
{
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    return environment.value("CLICK_COMMAND", QStringLiteral("click"));
}

QString Helpers::whichPkcon()
{
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    return environment.value("PKCON_COMMAND", QStringLiteral("pkcon"));
}
} // UpdatePlugin
