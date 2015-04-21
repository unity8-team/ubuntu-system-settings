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
 * This is a collection of functions to help dynamic creation and deletion
 * of ofono contexts.
 */

// Map of path to QOfonoConnectionContext objects
var _pathToQml = {};

var _CUSTOM_INTERNET_CONTEXT_NAME = '___ubuntu_custom_apn_internet';
var _CUSTOM_MMS_CONTEXT_NAME = '___ubuntu_custom_apn_mms';
var _CUSTOM_LTE_CONTEXT_NAME = '___ubuntu_custom_apn_ia';

function _getModelFromType (type) {
    var model;
    switch (type) {
        case 'mms':
            model = mmsContexts;
            break;
        case 'internet':
            model = internetContexts;
            break;
        case 'ia':
            model = iaContexts;
            break;
        default:
            throw new Error('Unknown context type ' + type);
    }
    return model;
}

function getCustomMmsContext () {
    return getCustomContext('mms');
}

function getCustomInternetContext () {
    return getCustomContext('mms');
}

function getCustomLteContext () {
    return getCustomContext('ia');
}

function getCustomContext (type) {
    var model = _getModelFromType(type);
    var i;

    for (i=0; i < model.count; i++) {
        if (isNameCustom(model.get(i).qml.name)) {
            return model.get(i).qml;
        }
    }
    return null;
}

/**
 * Given an array of paths, it will create and associate
 * an QOfonoConnectionContext QML object for each new path.
 *
 * It will also delete any QML that is not in given list of paths.
 *
 * @param {Array} paths, array of operator paths
 */
function updateQML (paths) {
    console.warn('updateQML', paths);
    _garbageCollect(paths);
    _createQml(paths);
}

/**
 * Destroys QML and makes sure to remove from the
 * appropriate model.
 *
 * @return {Boolean} deletion successful
 */
function deleteQML (path) {
    var ctx;
    var i;
    if (!_pathToQml.hasOwnProperty(path)) {
        return false;
    } else {
        console.warn('Deleting QML for path', path);
        ctx = _pathToQml[path];

        [mmsContexts, internetContexts, iaContexts].forEach(function (model) {
            for (i = 0; i < model.count; i++) {
                if (ctx.contextPath == model.get(i).path) {
                    model.remove(i);
                    console.warn('Found QML in ListModel, removing', path, 'in', model.label);
                    break;
                }
            }
        });

        _pathToQml[path].destroy();
        delete _pathToQml[path];
        return true;
    }
}

/**
 * Removes QML that no longer exists in list of paths.
 */
function _garbageCollect (paths) {
    var path;
    for (path in _pathToQml) {
        if (_pathToQml.hasOwnProperty(path)) {
            if (paths.indexOf(path) === -1) {
                console.warn('_garbageCollect', path);
                deleteQML(path);
            }
        }
    }
}

/**
 * Creates QML for list in paths.
 *
 * @param {Array:String} list of paths
 * @param {String} path to the modem
 */
function _createQml (paths) {
    console.warn('_createQml', paths);
    var ctx;
    paths.forEach(function (path, i) {
        if (!_pathToQml.hasOwnProperty(path)) {

            ctx = createContextQml(path);

            ctx.nameChanged.connect(contextNameChanged.bind(ctx));
            ctx.activeChanged.connect(contextActiveChanged.bind(ctx));

            // Some context come with a type, others not. Normalize this.
            if (!ctx.type) {
                ctx.typeChanged.connect(typeDetermined.bind(ctx));
            } else {
                addContextToModel(ctx);
            }

            _pathToQml[path] = ctx;
        }
    });
}

function createContextQml (path) {
    if (!_pathToQml.hasOwnProperty(path)) {
        return contextComponent.createObject(root, {
            'contextPath': path,
            'modemPath': sim.path
        });
    } else {
        return _pathToQml[path];
    }
}

function createContext (type) {
    sim.connMan.addContext(type);
}

function addContextToModel(context, type) {
    var data = {
        path: context.contextPath,
        qml: context
    };
    var model;
    console.warn('addContextToModel', type, data.qml, data.path);

    if (typeof type === 'undefined') {
        type = context.type;
    }

    model = _getModelFromType(type);

    if (context.active) {
        model.current = context;
    }

    // If custom, add it to the end of the list.
    if (isNameCustom(context.name)) {
        model.append(data);
    } else {
        model.insert(0, data);
    }
}

function contextRemoved (path) {
    var paths = sim.connMan.contexts.slice(0);
    updatedPaths = paths.filter(function (val) {
        return val !== path;
    });
    _garbageCollect(paths);
}

/**
 * Handler for when a type has been determined.
 * @param {String} type
 */
function typeDetermined (type) {
    console.warn('typeDetermined', type, this.contextPath);
    addContextToModel(this, type);
}

// /**
//  * Handler for newly created contexts.
//  * Also see https://github.com/rilmodem/ofono/blob/master/src/gprs.c#L148
//  * @param {String} type
//  */
// function contextQMLCreated (defName) {
//     console.warn('contextQMLCreated', type, this.contextPath);

//     var data = {
//         path: this.contextPath,
//         qml: this
//     };

//     if (defName === 'Internet') {

//     } else if (defName === "MMS") {

//     } else if (defName === "IA") {

//     }

//     switch (type) {
//         case 'mms':
//             mmsContexts.append(data);
//             break;
//         case 'internet':
//             internetContexts.append(data);
//             break;
//         case 'ia':
//             iaContexts.append(data);
//             break;
//         default:
//             throw new Error('Unknown context type.');
//     }
// }

/**
 * Handler for when a name changes. If we find that the name is one of
 * the ofono defaults, see [1], we assume this has been created by the user,
 * and we treat it as a custom context.
 *
 * [1] https://github.com/rilmodem/ofono/blob/master/src/gprs.c#L148
 */
function contextNameChanged (name) {
    console.warn('contextNameChanged', name, this.contextPath);
    var isCustom = false;
    var newName;

    if (name === 'Internet') {
        newName = _CUSTOM_INTERNET_CONTEXT_NAME;
        isCustom = true;
    } else if (name === "MMS") {
        newName = _CUSTOM_MMS_CONTEXT_NAME;
        isCustom = true;
    } else if (name === "IA") {
        newName = _CUSTOM_LTE_CONTEXT_NAME;
        isCustom = true;
    }

    if (isCustom) {
        this.disconnect();
        this.name = newName;
        console.warn('tried changing name of context to', newName);
    }
}

function userTriedActivating (active) {
    if (active) {
        console.warn('userTriedActivating', active);
        apnEditor.succeeded();
        this.activeChanged.disconnect(userTriedActivating);
    }
}

/**
 * Handler for activity changes in contexts.
 * Sets itself as the 'current' value on the appropriate model, if active.
 * If deactivated, it nullifies the 'current' value.
*/
function contextActiveChanged(active) {

    // We can't do anything when the type is indeterminate.
    if (!this.type) {
        return;
    }

    var model = _getModelFromType(this.type);

    if (active) {
        model.current = this;
    } else {
        if (model.current === this) {
            model.current = null;
        }
    }
}

function contextDeactivatedForEditing (newValues) {
    console.warn('contextDeactivatedForEditing', newValues);
    ctx.accessPointName = newValues;
    this.activeChanged.disconnect(contextDeactivatedForEditing);
}

/**
 * Handler for added contexts.
 */
function contextAdded (path) {
    console.warn('contextAdded');
    _createQml([path]);
}

/**
 * Handler for when contexts change.
 */
function contextsChanged (paths) {
    console.warn('contextsChanged');
    updateQML(paths);
}

/**
 * Handler for when errors are reported from ofono.
 */
function onReportError () {
    console.error(message);
    if (d.editor) {
        editor.failed(message);
    }
}

/**
 * Updates a context with new values.
 *
 * @param {QOfonoConnectionContext} context
 * @param {Object} values dict with new values
 */
function updateContext (context, values) {
    var messageProxy;

    console.warn('updateContext', values.accessPointName, values.messageCenter,
        values.messageProxy, values.port, values.useraccessPointName,
        values.password, values.type);

    if (typeof values.accessPointName !== 'undefined') {
        context.accessPointName = values.accessPointName;
    }

    if (typeof values.messageCenter !== 'undefined') {
        context.messageCenter = values.messageCenter;
    }

    if (typeof values.messageProxy !== 'undefined') {
        messageProxy = values.messageProxy;

        if (messageProxy !== '') {
             messageProxy = messageProxy + ':' + values.port;
        }
        context.messageProxy = messageProxy;
    }

    if (typeof values.username !== 'undefined') {
        context.username = values.username;
    }

    if (typeof values.password !== 'undefined') {
        context.password = values.password;
    }

    if (typeof values.type !== 'undefined') {
        context.type = values.type;
    }

    if (context.type === 'internet') {
        context.name = CUSTOM_INTERNET_CONTEXT_NAME();
    } else if (context.type === 'mms') {
        context.name = CUSTOM_MMS_CONTEXT_NAME();
    }
}

/**
 * Exposes the custom internet context name.
 *
 * @return {String} custom internet context name
 */
function CUSTOM_INTERNET_CONTEXT_NAME () {
    return _CUSTOM_INTERNET_CONTEXT_NAME;
}

/**
 * Exposes the custom mms context name.
 *
 * @return {String} custom mms context name
 */
function CUSTOM_MMS_CONTEXT_NAME () {
    return _CUSTOM_MMS_CONTEXT_NAME;
}

/**
 * Checks if a name is of any custom kind.
 *
 * @return {Boolean} whether or not the name is custom
 */
function isNameCustom (name) {
    if (name === _CUSTOM_MMS_CONTEXT_NAME ||
        name === _CUSTOM_INTERNET_CONTEXT_NAME ||
        name === _CUSTOM_LTE_CONTEXT_NAME) {
        return true;
    } else {
        return false;
    }
}
