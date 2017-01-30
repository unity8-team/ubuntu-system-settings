/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: William Hua <william.hua@canonical.com>
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
import SystemSettings 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItem
import Ubuntu.SystemSettings.LanguagePlugin 1.0

ItemPage {
    id: root
    title: i18n.tr("Keyboard layouts")
    flickable: subsetView

    property var plugin
    property bool currentLayoutsDraggable: false
    property bool draggingCurrentLayouts: false
    property double originalContentY: 0

    // Empty height + ThinDivider height
    readonly property double listItemHeight: units.gu(6) + units.dp(2)

    SubsetView {
        id: subsetView

        clip: true

        anchors.fill: parent

        section.property: "subset"
        section.delegate: ListItem.Standard {
            text: section == "true" ? i18n.tr("Current layouts:") : i18n.tr("All layouts available:")

            // Fade out if it's “All layouts available” and we're draggingCurrentLayouts
            opacity: section != "true" && draggingCurrentLayouts ? 0 : 1
        }

        model: plugin.keyboardLayoutsModel
        delegate: KeyboardLayoutItem {
            id: item
            anchors {
                left: parent.left
                right: parent.right
            }

            Behavior on height { enabled: visible; UbuntuNumberAnimation { } }

            name: model.language
            shortName: model.icon
            checked: model.checked
            enabled: model.enabled

            draggable: (currentLayoutsDraggable &&
                        model.subset &&
                        subsetView.model.subset.length > 1)

            visible: root.draggingCurrentLayouts ? model.subset : true
            opacity: root.draggingCurrentLayouts ? 0.75 : 1

            onCheckedChanged: {
                var element = model.index < subsetView.model.subset.length ?
                              subsetView.model.subset[model.index] :
                              model.index - subsetView.model.subset.length

                plugin.keyboardLayoutsModel.setChecked(element, checked, checked ? 0 : subsetView.delay)

                checked = Qt.binding(function() { return model.checked })
            }

            onDragStarted: {

                // If the element is not checked, refuse dragging.
                if (!model.checked) {
                    return;
                }
                root.draggingCurrentLayouts = true;

                // Force scroll to top
                subsetView.contentY = -listItemHeight

                dragger.target = dragItem;
                dragger.maximumX = units.gu(1);
                dragger.minimumX = units.gu(1);

                dragger.minimumY = listItemHeight + (listItemHeight / 2)
                dragger.maximumY = listItemHeight * (0.5 + subsetView.model.subset.length)

                dragItem.name = name;
                dragItem.shortName = shortName;
                dragItem.checked = checked;
                dragItem.enabled = enabled;
                dragItem.originalY = mapToItem(root, 0, 0).y;
                dragItem.originalIndex = index;
                dragItem.y = dragItem.originalY;
                dragItem.x = units.gu(1);
                dragItem.visible = true;
                dragItem.elementToShrink = item;
            }

            onDragFinished: {
                root.draggingCurrentLayouts = false;
                dragger.target = undefined;
                dragItem.visible = false;
                if (dragMarker.visible && dragMarker.index != index) {
                    plugin.requestCurrentLayoutMove(dragItem.originalIndex, dragMarker.index);
                }
                dragMarker.visible = false;
                dragItem.elementToShrink.height = listItemHeight;
                dragItem.elementToShrink.clip = false;
                dragItem.elementToShrink = null;
            }
        }
    }

    ListItem.ThinDivider {
        id: dragMarker
        visible: false
        property int index: {
            var midOfDragItem = dragItem.y - (dragItem.height / 2) - listItemHeight;
            var origi = Math.round(midOfDragItem / listItemHeight)
            var i = Math.round(midOfDragItem / listItemHeight)
            if (i < 0) i = 0;
            if (i >= subsetView.model.subset.length - 1) {
                i = subsetView.model.subset.length - 1;
            }
            return i;
        }
        y: ((index + 2) * listItemHeight) - height / 2
        height: units.gu(1)
    }

    KeyboardLayoutItem {
        id: dragItem

        property real originalY
        property int originalIndex
        property var elementToShrink: null

        objectName: "dragItem"
        visible: false
        opacity: 0.9
        style: Rectangle {
            color: Theme.palette.selected.background
        }
        onYChanged: {
            if (!dragMarker.visible && Math.abs(y - originalY) >= height / 2) {
                dragMarker.visible = true;
                dragItem.elementToShrink.clip = true;
                elementToShrink.height = 0.01;
            }
        }
    }
}
