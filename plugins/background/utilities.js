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

function setBackground(homeScreen, uri) {
    console.log ("setBackground: " + uri);
    console.log ("systemSettingsSettings.backgroundDuplicate: " + systemSettingsSettings.backgroundDuplicate);
    if (systemSettingsSettings.backgroundDuplicate) {
        updateBoth(uri);
    } else {
        if (homeScreen) {
            updateHome(uri);
            systemSettingsSettings.backgroundSetLast = "home";
        } else {
            updateWelcome(uri);
            systemSettingsSettings.backgroundSetLast = "welcome";
        }
    }
    //pageStack.pop();
}

function updateWelcome(uri) {
    console.log ("updateWelcome: " + uri);
    backgroundPanel.backgroundFile = uri;
}

function updateHome(uri) {
    console.log ("updateHome: " + uri);
    background.pictureUri = uri;
}

function updateBoth(uri) {
    console.log ("updateBoth: " + uri);
    updateWelcome(uri);
    updateHome(uri);
}
