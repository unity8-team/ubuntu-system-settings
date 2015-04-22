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

function updateContext (ctx) {
    console.warn('updateContext', accessPointName.text, username.text, password.text);
    ctx.accessPointName = accessPointName.text;
    ctx.username = username.text;
    ctx.password = password.text;

    if (ctx.type === 'mms') {
        ctx.messageCenter = messageCenter.text;
        ctx.messageProxy = messageProxy.text + (port.text ? ':' + port.text : '');
    }
}

function populate (ctx) {
    console.warn('populate');
    accessPointName.text = ctx.accessPointName;
    username.text = ctx.username;
    password.text = ctx.password;

    if (isMms) {
        messageCenter.text = ctx.messageCenter;
        messageProxy.text = ctx.messageProxy;
    }
}

function activateButtonPressed () {
    console.warn('activateButtonPressed');
    // Do we have a custom context?
    var ctx = manager.getCustomContext(contextModel.type);

    if (ctx) {
        // We have a custom context we want to change.
        // We cannot change it if it is active. If active,
        // we need to defer changing it until it has been
        // deactivated.
        if(ctx.active) {
            console.warn('Deactivating context before changing.');
            ctx.disconnect();
        }
        console.warn('Changing context');
        updateContext(ctx);

        root.activated(ctx);
    } else {
        // We will create a new context. This is async, so
        // we attach a one time event to addition of contexts.
        // We can't guarantee that the context added is this
        // we just created.

        function updateCreatedContext () {
            var ctx = contextModel.get(contextModel.count - 1).qml;
            var i;
            console.warn('updateCreatedContext', ctx.name, ctx);
            ctx.disconnect();
            updateContext(ctx);

            // Update selected index of suggestions list
            copyFromMms.selectedIndex = -1;
            copyFromInternet.selectedIndex = -1;
            for (i = 0; i < suggestions.model.count; i++) {
                if (suggestions.model.get(i).qml === ctx) {
                    suggestions.selectedIndex = i;
                }
            }

            contextModel.countChanged.disconnect(updateCreatedContext);
            root.activated(ctx);
        }

        contextModel.countChanged.connect(updateCreatedContext);
        manager.createContext(contextModel.type);
    }
}

function hasProtocol (link) {
    return link.search(/^http[s]?\:\/\//) == -1;
}

function setHttp(link) {
    if (hasProtocol(link)) {
        link = 'http://' + link;
    }
    return link;
}

function isChanged () {
    // We had no suggestion, so we have nothing to compare it to.
    if (!suggestion) {
        return true;
    }

    var refData = [suggestion.accessPointName,
                   suggestion.username,
                   suggestion.password];
    var formData = [accessPointName.text,
                    username.text,
                    password.text];
    var i;

    // If we are comparing mms, add more data.
    if (isMms) {
        refData.push(suggestion.messageCenter);
        formData.push(messageCenter.text);

        refData.push(suggestion.messageProxy);
        formData.push(messageProxy.text + (port.text ? ':' + port.text : ''));
    }

    // Compare the arrays.
    i = refData.length;
    while(i--) {
        if (refData[i] !== formData[i]) return true;
    }
    return false;
}

function isValid () {
    return accessPointName.text;
}

function prePopulate () {
    if (contextModel.current) {
        suggestion = contextModel.current;
        Editor.populate(contextModel.current);
        // Update selectedIndex of suggestsions
        for (var i = 0; i < suggestions.model.count; i++) {
            if (suggestions.model.get(i).qml === contextModel.current) {
                suggestions.selectedIndex = i;
            }
        }
    }
}
