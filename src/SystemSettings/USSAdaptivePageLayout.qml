/*
 * This file is part of system-settings
 *
 * Copyright (C) 2017 Canonical Ltd.
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

import QtQuick 2.0
import Ubuntu.Components 1.3

AdaptivePageLayout {
    id: layout

    function addFileToNextColumnSync(parentObject, resolvedUrl, properties) {
        return addComponentToNextColumnSync(parentObject,
                                            Qt.createComponent(resolvedUrl),
                                            properties);
    }

    function addComponentToNextColumnSync(parentObject, component, properties) {
        if (typeof(properties) === 'undefined') {
            properties = {}
        }

        var incubator = layout.addPageToNextColumn(parentObject, component, properties)
        incubator.forceCompletion()
        return incubator.object
    }
}
