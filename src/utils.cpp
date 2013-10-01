/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@canonical.com>
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

#include "debug.h"
#include "utils.h"

#include <QUrl>
#include <QUrlQuery>


typedef QPair<QString,QString> StringPair;

namespace SystemSettings {

void parsePluginOptions(const QStringList &arguments, QString &defaultPlugin,
                        QVariantMap &pluginOptions)
{
    for (int i = 1; i < arguments.count(); i++) {
        const QString &argument = arguments.at(i);
        if (argument.startsWith("settings://")) {
            QUrl urlArgument(argument);
            /* Find out which plugin is required. If the first component of the
             * path is "system", just skip it. */
            QStringList pathComponents =
                urlArgument.path().split('/', QString::SkipEmptyParts);
            int pluginIndex = 0;
            if (pathComponents.value(pluginIndex, "") == "system")
                pluginIndex++;
            defaultPlugin = pathComponents.value(pluginIndex, QString());
            /* Convert the query parameters into options for the plugin */
            QUrlQuery query(urlArgument);
            Q_FOREACH(const StringPair &pair, query.queryItems()) {
                pluginOptions.insert(pair.first, pair.second);
            }
        } else if (!argument.startsWith('-')) {
            defaultPlugin = argument;
        } else if (argument == "--option" && i + 1 < arguments.count()) {
            QStringList option = arguments.at(++i).split("=");
            // If no value is given, insert an empty string
            pluginOptions.insert(option.at(0), option.value(1, ""));
        }
    }
}

} // namespace
