/*
 * This file is part of system-settings
 *
 * Copyright (C) 2015 Canonical Ltd.
 *
 * Contact: Jonas G. Drange <jonas.drange@canonical.com>
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
 *
 * This is a collection of functions to help the custom apn editor.
 */

/**
 * Updates the given OfonoContextConnection by using values from the editor.
 *
 * @param {OfonoContextConnection} qml to be updated
*/
function updateContextQML (ctx) {
    var toType = indexToType(typeSel.selectedIndex);
    toType = toType === 'internet+mms' ? 'internet' : toType;
    ctx.disconnect();
    ctx.name = name.text;
    ctx.type = toType;
    ctx.accessPointName = accessPointName.text;
    ctx.username = username.text;
    ctx.password = password.text;
    ctx.messageCenter = messageCenter.visible ? messageCenter.text : '';
    ctx.messageProxy = messageProxy.visible ? messageProxy.text + (port.text ? ':' + port.text : '') : '';
}

/**
 * Populates editor with values from a OfonoContextConnection.
 *
 * @param {OfonoContextConnection} qml to use as reference
*/
function populate (ctx) {
    name.text = ctx.name;
    accessPointName.text = ctx.accessPointName;
    username.text = ctx.username;
    password.text = ctx.password;
    messageCenter.text = ctx.messageCenter;
    messageProxy.text = ctx.messageProxy;

    if (ctx.isCombined) {
        typeSel.selectedIndex = typeToIndex('internet+mms');
    } else {
        typeSel.selectedIndex = typeToIndex(ctx.type);
    }
}

/**
 * Handler for when a user saves a context.
*/
function saving () {
    var model;
    var type;
    root.saving();

    // Edit or new? Create a context if it does not exist.
    if (contextQML) {
        updateContextQML(contextQML);
        root.saved();
    } else {
        type = indexToType();
        if (type === 'internet+mms') type = 'internet';
        root.requestContextCreation(type);
    }
}

/**
 * Handler for new contexts.
 *
 * @param {OfonoContextConnection} new context
*/
function newContext (context) {
    // Start a timer that will update the context.
    // Ofono and libqofono seems to be very unreliable
    // when it comes to how a context is created,
    // so we just wait a couple of seconds until we're
    // sure the context exists and can be edited.
    updateContext.ctx = context;
    updateContext.start();
}

/**
 * Checks whether or not a link has a http protocol prefix.
 *
 * @param {String} link to check
*/
function hasProtocol (link) {
    return link.search(/^http[s]?\:\/\//) == -1;
}

/**
 * Prepend http:// to a link if there isn't one.
 *
 * @param {String} link to add http:// to
 * @return {String} changed link
*/
function setHttp (link) {
    if (hasProtocol(link)) {
        link = 'http://' + link;
    }
    return link;
}

/**
 * Asks whether or not the values in the editor is valid or not.
 *
 * @return {Boolean} whether or not the editor is valid
*/
function isValid () {
    if (isMms || isCombo) {
        return name.text &&
               accessPointName.text &&
               messageCenter.text;
    } else {
        return name.text &&
               accessPointName.text;
    }
}

/**
 * Given a type, this asks what index of the type selector
 * it corresponds to.
 *
 * @param {String} type to check
 * @return {Number} of index
*/
function typeToIndex (type) {
    if (type === 'internet+mms') return 0;
    if (type === 'internet') return 1;
    if (type === 'mms') return 2;
    if (type === 'ia') return 3;
}

/**
 * Given an index, we ask what type this index corresponds to.
 *
 * @param {Number} [optional] index to check
 * @return {String} type it corresponds to
*/
function indexToType (index) {
    if (typeof index === 'undefined') {
        index = typeSel.selectedIndex;
    }
    if (index === 0) return 'internet+mms';
    if (index === 1) return 'internet';
    if (index === 2) return 'mms';
    if (index === 3) return 'ia';
}

function ready () {
    _edgeReady = true;
}

function makeMeVisible(item) {
    if (!_edgeReady || !item) {
        return;
    }

    root.activeItem = item;

    var position = scrollArea.contentItem.mapFromItem(item, 0, root.activeItem.y);

    // check if the item is already visible
    var bottomY = scrollArea.contentY + scrollArea.height;
    var itemBottom = position.y + item.height; // extra margin
    if (position.y >= scrollArea.contentY && itemBottom <= bottomY) {
        return;
    }

    // if it is not, try to scroll and make it visible
    var targetY = itemBottom - scrollArea.height;
    if (targetY >= 0 && position.y) {
        scrollArea.contentY = targetY;
    } else if (position.y < scrollArea.contentY) {
        // if it is hidden at the top, also show it
        scrollArea.contentY = position.y;
    }
    scrollArea.returnToBounds();
}
