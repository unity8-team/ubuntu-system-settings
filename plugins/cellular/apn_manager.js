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
var _CUSTOM_IA_CONTEXT_NAME = '___ubuntu_custom_apn_ia';

/**
 * Get the list model corresponding to a given type.
 *
 * @throws {Error} if the type was not mms|internet|ia
 * @param {String} type of model to get
 * @return {ListModel} model that matches type
*/
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


/**
 * Get the custom MMS QML.
 *
 * @return {OfonoConnectionContext|null} qml or null if none found
*/
function getCustomMmsContextQML () {
    return getCustomContextQML('mms');
}


/**
 * Get the custom Internet QML.
 *
 * @return {OfonoConnectionContext|null} qml or null if none found
*/
function getCustomInternetContextQML () {
    return getCustomContextQML('mms');
}


/**
 * Get the custom IA QML.
 *
 * @return {OfonoConnectionContext|null} qml or null if none found
*/
function getCustomLteContextQML () {
    return getCustomContextQML('ia');
}


/**
 * Get the custom QML for a context of a given type.
 *
 * @param {String} type
 * @return {OfonoConnectionContext|null} qml or null if none found
*/
function getCustomContextQML (type) {
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
 * Get QML for a context path.
 *
 * @param {String} path of context
 * @return {QOfonoConnectionContext|null} qml from path or null if none found
*/
function getContextQML (path) {
    if (!_pathToQml.hasOwnProperty(path)) {
        return null;
    } else {
        return _pathToQml[path];
    }
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
 * @param {String} path of object to delete
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
 *
 * @param {Array:String} paths we use as reference.
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

/**
 * Creates a OfonoConnectionContext qml object from a given path.
 * Since the components are all local, this will always return an object.
 *
 * @param {String} path of context
 * @return {OfonoConnectionContext} qml that was created
*/
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

/**
 * Creates a context of a certain type.
 *
 * @param {String} type of context to be created.
*/
function createContext (type) {
    console.warn('Creating context of type', type);
    sim.connMan.addContext(type);
}


/**
 * Removes a context. We don't remove any QML until we receive signal from
 * ofono that the context was removed, but we disconnect it if active.
 *
 * @param {String} path of context to be removed
*/
function removeContext (path) {
    console.warn('Removing context', path);
    var ctx = getContextQML(path);

    if (ctx && ctx.active) {
        ctx.disconnect();
    }

    sim.connMan.removeContext(path);
}


/**
 * Adds a context to the appropriate model.
 *
 * @param {OfonoConnectionContext} ctx to be added
 * @param {String} [optional] type of context
*/
function addContextToModel(ctx, type) {
    var data = {
        path: ctx.contextPath,
        qml: ctx
    };
    var model;
    console.warn('addContextToModel', type, ctx.name, data.qml, data.path);

    if (typeof type === 'undefined') {
        type = ctx.type;
    }

    model = _getModelFromType(type);

    if (ctx.active) {
        model.current = ctx;
    }

    // If custom, add it to the end of the list.
    if ((type === 'mms' && ctx.name === 'MMS') ||
        (type === 'internet' && ctx.name === 'Internet') ||
        (type === 'ia' && ctx.name === 'IA')) {
        console.warn('addContextToModel adding', ctx.name, 'to end');
        model.append(data);
    } else {
        console.warn('addContextToModel adding', ctx.name, 'first');
        model.insert(0, data);
    }
}

/**
 * Handler for removed contexts.
 *
 * @param {String} path that was removed
*/
function contextRemoved (path) {
    var paths = sim.connMan.contexts.slice(0);
    var updatedPaths = paths.filter(function (val) {
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

/**
 * Handler for when a name changes. If we find that the name is one of
 * the ofono defaults, see [1], we assume this has been created by the user,
 * and we treat it as a custom context.
 *
 * [1] https://github.com/rilmodem/ofono/blob/master/src/gprs.c#L148
 *
 * @param {String} name's new value
 */
function contextNameChanged (name) {
    console.warn('contextNameChanged', name, this.contextPath);
    var isCustom = false;
    var newName;

    if (name === 'Internet') {
        newName = _CUSTOM_INTERNET_CONTEXT_NAME;
        isCustom = true;
    } else if (name === 'MMS') {
        newName = _CUSTOM_MMS_CONTEXT_NAME;
        isCustom = true;
    } else if (name === 'IA') {
        newName = _CUSTOM_IA_CONTEXT_NAME;
        isCustom = true;
    }

    if (isCustom) {
        this.disconnect();
        this.name = newName;
        console.warn('Changing name of context to', newName, this.active);
    }
}

/**
 * Handler for activity changes in contexts.
 * Sets itself as the 'current' value on the appropriate model, if active.
 * If deactivated, it nullifies the 'current' value.
 *
 * @param {Boolean} active's new value
*/
function contextActiveChanged(active) {

    // We can't do anything sensible when the type is undetermined.
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

/**
 * Handler for added contexts.
 *
 * @param {String} path which was added
 */
function contextAdded (path) {
    console.warn('contextAdded', path);
    _createQml([path]);
}

/**
 * Handler for when contexts change.
 *
 * @param {Array:String} paths after change
 */
function contextsChanged (paths) {
    console.warn('contextsChanged', paths);
    updateQML(paths);
}

/**
 * Handler for when errors are reported from ofono.
 *
 * @param {String} message from libqofono
 */
function reportError (message) {
    console.error(message);
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
 * Exposes the custom ia context name.
 *
 * @return {String} custom ia context name
 */
function CUSTOM_IA_CONTEXT_NAME () {
    return _CUSTOM_IA_CONTEXT_NAME;
}

/**
 * Checks if a name is of any custom kind.
 *
 * @return {Boolean} whether or not the name is custom
 */
function isNameCustom (name) {
    if (name === _CUSTOM_MMS_CONTEXT_NAME ||
        name === _CUSTOM_INTERNET_CONTEXT_NAME ||
        name === _CUSTOM_IA_CONTEXT_NAME) {
        return true;
    } else {
        return false;
    }
}

/**
 * Activates the given OfonoConnectionContext. If MMS, we delete all other
 * contexts. If the context is IA or Internet, we use the OfonoActivator
 * to create a connection.
 *
 * @param {OfonoConnectionContext} ctx to be activated
*/
function activateContextQML (ctx) {
    var i;

    ctx.active = true;
    console.warn('activateContext', ctx.name);
    if (ctx.type === 'mms') {

        // Activation of an MMS context is currently the
        // removal of all other contexts. See lp:1361864 and
        // lp:1361864.
        for (i = 0; i < mmsContexts.count; i++) {

            console.warn('activateContext',
                         mmsContexts.get(i).path, ctx.contextPath);

            if (mmsContexts.get(i).path !== ctx.contextPath) {
                removeContext(mmsContexts.get(i).path);
            }
        }


    } else if (ctx.type === 'ia' || ctx.type === 'internet') {
        return activator.activate(ctx.contextPath,
                                  sim.simMng.subscriberIdentity,
                                  sim.path);
    }
}
