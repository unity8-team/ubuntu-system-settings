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
#include <QProcessEnvironment>

namespace UpdatePlugin {

std::vector<std::string> Helpers::getAvailableFrameworks()
{

}

std::string Helpers::getArchitecture()
{
    static const std::string deb_arch {architectureFromDpkg()};
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
    if(!archDetector.waitForFinished()) {
        qWarning() << "Architecture detection failed.";
    }
    auto output = archDetector.readAllStandardOutput();
    auto ostr = QString::fromUtf8(output);

    return ostr.trimmed().toStdString();
}


QString Helpers::clickMetadataUrl()
{
    return QStringLiteral("https://search.apps.ubuntu.com/api/v1/click-metadata");
}

QString Helpers::clickTokenUrl(const QString &url)
{
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    return environment.value("CLICK_TOKEN_URL", url);

}

bool Helpers::isIgnoringCredentials()
{
    // IGNORE_CREDENTIALS
    return false;
}

QString Helpers::whichClick()
{
    // CLICK_COMMAND
    return QStringLiteral("click");
}

} // Namespace UpdatePlugin
