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

// Map of path to QOfonoContextConnection objects
var _pathToQml = {};
var _totalContext = 0;
var _validContexts = 0;

/**
 * Get the list model corresponding to a given type.
 *
 * @throws {Error} if the type was not mms|internet|ia
 * @param {String} type of model to get
 * @return {ListModel} model that matches type
*/
function getModelFromType (type) {
    var model;
    switch (type) {
        case 'mms':
            model = mmsContexts;
            break;
        case 'internet':
        case 'internet+mms':
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
 * Get QML for a context path.
 *
 * @param {String} path of context
 * @return {QOfonoContextConnection|null} qml from path or null if none found
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
 * an QOfonoContextConnection QML object for each new path.
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
                    console.warn('Found QML in ListModel, removing', path, 'in', model.title);
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
                _totalContext--;
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
    console.warn('_createQml...');
    var ctx;
    paths.forEach(function (path, i) {
        if (!_pathToQml.hasOwnProperty(path)) {

            ctx = createContextQml(path);
            console.warn('_createQml created', path, ctx.name, ctx.type);

            // Some contexts have a name, others do not. Normalize this.
            if (!ctx.name) {
                ctx.nameChanged.connect(contextNameChanged.bind(ctx));
            } else {
                contextNameChanged.bind(ctx)(ctx.name);
            }

            // Some context come with a type, others not. Normalize this.
            if (!ctx.type) {
                ctx.typeChanged.connect(contextTypeChanged.bind(ctx));
            } else {
                addContextToModel(ctx);
            }

            ctx.validChanged.connect(contextValidChanged.bind(ctx));
            ctx.activeChanged.connect(contextActiveChanged.bind(ctx));

            _pathToQml[path] = ctx;
            _totalContext++;
        }
    });
}

/**
 * Creates a OfonoContextConnection qml object from a given path.
 * Since the components are all local, this will always return an object.
 *
 * @param {String} path of context
 * @return {OfonoContextConnection} qml that was created
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
 * Adds a context to the appropriate model. If the context to be added is found
 * in another model, which will happen if the user changes type of the context,
 * we remove it from the old model and add it to the new.
 *
 * @param {OfonoContextConnection} ctx to be added
 * @param {String} [optional] type of context
*/
function addContextToModel (ctx, type) {
    var data = {
        path: ctx.contextPath,
        qml: ctx
    };
    var model;
    var oldModel;
    var haveContext;
    console.warn('addContextToModel', type, ctx.name, data.qml, data.path);

    // We will move a model if it already exist.
    [mmsContexts, internetContexts, iaContexts].forEach(function (m) {
        var i;
        for (i = 0; i < m.count && !haveContext; i++) {
            if (ctx.contextPath == m.get(i).path) {
                haveContext = m.get(i);
                oldModel = m;
                console.warn('addContextToModel: Found existing context in ListModel, moving...', haveContext.path, 'in', m.title);
                break;
            }
        }
    });

    if (typeof type === 'undefined') {
        type = ctx.type;
    }

    if (haveContext && oldModel) {
        oldModel.remove(haveContext);
    }

    model = getModelFromType(type);
    model.append(data);
}

/**
 * Removes a context from the appropriate model.
 *
 * @param {OfonoContextConnection} ctx to be removed
 * @param {String} [optional] type of context
*/
function removeContextFromModel (ctx, type) {
    var model = getModelFromType(type);
    var i;

    if (typeof type === 'undefined') {
        type = ctx.type;
    }

    for (i = 0; i < model.count; i++) {
        if (model.get(i).path === ctx.contextPath) {
            model.remove(i);
            return;
        }
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
 * Handler for when a type has been determined. If a contex changes type,
 * we need to move it to the correct model.
 * Note that “this' refers to the context on which type changed.
 *
 * @param {String} type
 */
function contextTypeChanged (type) {
    console.warn('contextTypeChanged', type, this.contextPath);
    addContextToModel(this, type);
}

/**
 * Handler for when validity of context changes.
 * Note that “this' refers to the context on which valid changed.
 *
 * @param {Boolean} valid
 */
function contextValidChanged (valid) {
    console.warn('contextValidChanged', this.valid, valid, this.contextPath);
    if (valid) {
        _validContexts++;
    } else {
        _validContexts--;
    }

    if (_validContexts === _totalContext) {
        console.warn('_validContexts === _totalContext, firing ready...');
        root.ready();
    }
}

/**
 * Handler for when active changes.
 * Note that “this' refers to the context on which active changed.
 *
 * @param {Boolean} active
 */
function contextActiveChanged (active) {
    console.warn('contextActiveChanged', this.active, active, this.contextPath);
    if (active) {
        checkPreferred();
    }
}

/**
 * This is code that is supposed to identify new contexts that user creates.
 * If we think the context is new, and the editor page is open, we notify it.
 *
 * Note that “this' refers to the context on which name changed.
 *
 * @param {String} name's new value
*/
function contextNameChanged (name) {
    console.warn('contextNameChanged', name);
    switch (name) {
        case 'Internet':
        case 'IA':
        case 'MMS':
            if (editor) {
                console.warn('We saw what we thought was ofono default. Notifying editor...');
                editor.newContext(this);
            }
            break;
    }
    this.nameChanged.disconnect(contextNameChanged);
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
    checkPreferred();
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
 * Set Preferred on a context. A side effect of this, if value is true, is
 * that all other contexts of the same type will get de-preferred. If an
 * MMS context is preferred, all other MMS contexts are de-preferred.
 *
 * There is also a case where if you prefer an MMS context when there already
 * is a preferred Internet+MMS (combined) context. In this case we prompt the
 * user what to do.
 *
 * Note: If triggered by a CheckBox, the “this” argument will be the CheckBox.
 *
 * @param {OfonoContextConnection} context to prefer
 * @param {Boolean} new preferred value
 * @param {Boolean} whether or not to force this action, even though it may
 *                  decrease the level of connectivity of the user.
*/
function setPreferred (context, value, force) {
    console.warn('setPreferred...', this, context.name);
    var conflictingContexts = getConflictingContexts(context);
    var mmsPreferralCausesCombinedDePreferral;
    var internetPreferralCausesCombinedDePreferral;

    // The context is about to be de-preferred.
    if (!value) {
        if (force) {
            context.preferred = false;
        } else {
            PopupUtils.open(disableContextWarning, root, {
                context: context
            });
            this.checked = true;
        }
        return;
    }

    // If user is preferring standalone Internet or standalone MMS context,
    // over a combined context, we will give a warning, if not forced.
    conflictingContexts.forEach(function (ctxC) {
        if (ctxC.isCombined) {
            if (context.type === 'mms') {
                mmsPreferralCausesCombinedDePreferral = ctxC;
            }

            if (context.type === 'internet') {
                internetPreferralCausesCombinedDePreferral = ctxC;
            }
        }
    });

    if (mmsPreferralCausesCombinedDePreferral && !force) {
        PopupUtils.open(disablesInternetWarning, root, {
            combined: mmsPreferralCausesCombinedDePreferral,
            mms: context
        });
        this.checked = false;
        return;
    } else if (internetPreferralCausesCombinedDePreferral && !force) {
        PopupUtils.open(disablesMMSWarning, root, {
            combined: internetPreferralCausesCombinedDePreferral,
            internet: context
        });
        this.checked = false;
        return;
    }

    conflictingContexts.forEach(function (ctx) {
        ctx.preferred = false;
    });

    console.warn('Setting', context.name, 'as preferred');
    context.preferred = true;
}

/**
 * Reset apn configuration.
 */
function reset () {
    // If cellular data is on, we need to turn it off. The reset itself,
    // as well as turning cellular data back on, is done by the use of a
    // Connection component and connManPoweredChanged.
    if (sim.connMan.powered) {
        console.warn('sat restorePowered target');
        restorePowered.target = sim.connMan;
        sim.connMan.powered = false;
    } else {
        connManPoweredChanged(sim.connMan.powered);
    }
}


/**
 * Handler for when powered changed. This handler is attached to a signal by
 * a Connections component in PageChooseApn.qml.
 */
function connManPoweredChanged (powered) {
    console.warn('poweredChangedForReset', powered);
    if (!powered) {

        // We want to fire the ready signal again, once we've reset, but
        // the reset contexts won't necessarily fire 'validChanged' signals,
        // so we manually set valid contexts to 0.
        _validContexts = 0;
        root.ready.connect(ready);

        sim.connMan.resetContexts();

        // If restorePowered had a target, we know to turn cellular
        // data back on.
        if (restorePowered.target) {
            console.warn('had restorePowered target, powering up..');
            sim.connMan.powered = true;
        }
    }
    restorePowered.target = null;
}

/**
 * Checks if there are preferred contexts. If there are none,
 * we prefer the active one.
 */
function checkPreferred () {
    var models = [internetContexts, iaContexts, mmsContexts];

    models.forEach(function (model) {
        var i;
        var ctx;
        var activeCtx;

        // Find active contexts in model.
        for (i = 0; i < model.count; i++) {
            ctx = model.get(i).qml;
            console.warn('checking if', ctx.contextPath, 'is preferred...');

            if (ctx.active) {
                activeCtx = ctx;
            }
        }

        if (activeCtx && getConflictingContexts(activeCtx).length === 0) {
            activeCtx.preferred = true;
            console.warn(activeCtx.name, 'is now preferred in', model.title);
        }
    });
}

/**
 * Gives a list of conflicting contexts, i.e. contexts that are preferred
 * and will create problems† if preferred at the same time as the given
 * context.
 *
 * † Multiple preferred contexts of the same type will cause undefined
 * Nuntium and NetworkManager behaviour.
 *
 * @param {OfonoContextConnection|String} context to be preferred and to check
                                          conflicts against, or type as string
 * @return {Array:OfonoContextConnection} list of OfonoContextConnection that
 *                                        are in conflict, excluding itself
 *
 */
function getConflictingContexts (context) {
    var type;
    var conflicts = [];
    var i;
    var ctxI;
    var typeModel;

    if (typeof context === 'string') {
        type = context;
    } else {
        type = context.isCombined ? 'internet+mms' : context.type;
    }

    switch (type) {
        // A combined context will conflict with internet contexts and MMS
        // contexts.
        case 'internet+mms':
            [internetContexts, mmsContexts].forEach(function (model) {
                var i;
                for (i = 0; i < model.count; i++) {
                    ctxI = model.get(i).qml;
                    if (ctxI.preferred) {
                        conflicts.push(ctxI);
                    }
                }
            });
            break;
        // An MMS context will conflict with other MMS contexts, as well as
        // combined contexts.
        case 'mms':
            for (i = 0; i < mmsContexts.count; i++) {
                ctxI = mmsContexts.get(i).qml;
                if (ctxI.preferred) {
                    conflicts.push(ctxI);
                }
            }
            for (i = 0; i < internetContexts.count; i++) {
                ctxI = internetContexts.get(i).qml;
                if (ctxI.isCombined && ctxI.preferred) {
                    conflicts.push(ctxI);
                }
            }
            break;
        case 'internet':
        case 'ia':
            typeModel = getModelFromType(type);
            for (i = 0; i < typeModel.count; i++) {
                ctxI = typeModel.get(i).qml;
                if (ctxI.preferred) {
                    conflicts.push(ctxI);
                }
            }
            break;
        default:
            throw new Error('Can\'t resolve conflicts for type' + type);
    }
    console.warn('Conflicting contexts...');

    conflicts.forEach(function (c) {console.warn("\t" + c.name);});
    return conflicts;
}

function ready () {
    console.warn('fired ready');
    checkPreferred();
    root.ready.disconnect(ready);
}
