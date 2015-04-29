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
    console.warn('updateContext', name.text, accessPointName.text, username.text, password.text);
    ctx.name = name.text;
    ctx.accessPointName = accessPointName.text;
    ctx.username = username.text;
    ctx.password = password.text;
    ctx.messageCenter = messageCenter.text;
    ctx.messageProxy = messageProxy.text + (port.text ? ':' + port.text : '');
}

/**
 * Populates editor with values from a OfonoContextConnection.
 *
 * @param {OfonoContextConnection} qml to use as reference
*/
function populate (ctx) {
    console.warn('populate');
    name.text = ctx.name;
    accessPointName.text = ctx.accessPointName;
    username.text = ctx.username;
    password.text = ctx.password;
    messageCenter.text = ctx.messageCenter;
    messageProxy.text = ctx.messageProxy;

    if (manager.isComboContext(ctx)) {
        typeSel.selectedIndex = typeToIndex('internet+mms');
    } else {
        typeSel.selectedIndex = typeToIndex(ctx.type);
    }
}

/**
 * Handler for when a user saves a context.
*/
function saving () {
    console.warn('saving...', contextQML);
    var model;
    var type;

    // Edit or new?
    if (contextQML) {
        updateContextQML(contextQML);
        root.saved(contextQML);
    } else {

        function createdContextDisconnected () {
            console.warn('createdContextDisconnected');
            this.activeChanged.disconnect(createdContextDisconnected);
            updateContextQML(this);
        }

        // We will create a new context. This is async, so
        // we attach a one time event to addition of contexts.
        // We can't guarantee that the context added is this
        // we just created.

        function updateCreatedContext () {

            var type = indexToType();
            if (type === 'internet+mms') {
                type = 'internet';
            }

            var model = manager.getModelFromType(type);
            var ctx = model.get(model.count - 1).qml;
            var i;
            console.warn('updateCreatedContext', ctx.name, ctx);
            if (ctx.active) {
                ctx.disconnect();
                ctx.activeChanged.connect(createdContextDisconnected.bind(ctx));
            } else {
                updateContextQML(ctx);
                root.saved(ctx);
                model.countChanged.disconnect(updateCreatedContext);
            }

        }
        type = indexToType();

        if (type === 'internet+mms') {
            type = 'internet';
        }

        model = manager.getModelFromType(type);

        model.countChanged.connect(updateCreatedContext);
        manager.createContext(type);
    }
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
function setHttp(link) {
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
    return name.text && accessPointName.text;
}

function typeToIndex (type) {
    if (type === 'internet+mms') return 0;
    if (type === 'internet') return 1;
    if (type === 'mms') return 2;
    if (type === 'ia') return 3;
}

function indexToType () {
    var index = typeSel.selectedIndex;
    if (index === 0) return 'internet+mms';
    if (index === 1) return 'internet';
    if (index === 2) return 'mms';
    if (index === 3) return 'ia';
}
