/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014-2016 Canonical Ltd.
 *
 * Contact: Diego Sarmentero <diego.sarmentero@canonical.com>
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

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.DownloadManager 1.2
import Ubuntu.SystemSettings.Update 1.0

ListItem {
    id: root
    objectName: "entryComponent-updates"
    property int updatesAvailable: {
        var imageUpdateCount = SystemImage.checkTarget() ? 1 : 0;
        return updatesRep.count + imageUpdateCount;
    }
    height: (updatesAvailable > 0) || showAllUI ? layout.height : 0
    onClicked: main.loadPluginByName("system-update");
    divider.colorFrom: "#EEEEEE"
    divider.colorTo: "#EEEEEE"
    divider.height: units.dp(1)

    ListItemLayout {
        id: layout
        title.text: i18n.tr(model.displayName)
        Icon {
            SlotsLayout.position: SlotsLayout.Leading;
            SlotsLayout.padding { top: 0; bottom: 0 }
            source: model.icon
            height: units.gu(2.5)
        }
        Label {
            text: updatesAvailable > 0 ? updatesAvailable : ""
        }
        ProgressionSlot {}
    }

    DownloadManager {
        onDownloadFinished: {
            UpdateManager.model.setInstalled(
                download.metadata.custom.identifier,
                download.metadata.custom.revision
            );
        }
    }

    Repeater {
        width: 1
        height: 1
        id: updatesRep
        model: UpdateManager.clickUpdates
        Item { width: 1; height: 1 }
    }

    Behavior on height { UbuntuNumberAnimation {} }
}
