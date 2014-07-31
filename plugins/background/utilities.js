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
 * Authors:
 * Ken VanDine <ken.vandine@canonical.com>
 *
 */

function setBackground(uri) {
    backgroundPanel.backgroundFile = backgroundPanel.prepareBackgroundFile(uri, true);
}

function revertBackgroundToDefault () {
    setBackground(Qt.resolvedUrl(mainPage.defaultBackground));
    uArtGrid.state = "";
}

function deSelectBackgrounds (repeater) {
    for (var i=0, j=repeater.count; i < j; i++) {
        repeater.itemAt(i).state = "";
    }
}

function getSelected (repeater) {
    var s = 0;
    for (var i=0, j=repeater.count; i < j; i++) {
        if (repeater.itemAt(i).state === "selected") {
            s++;
        }
    }
    return s;
}
