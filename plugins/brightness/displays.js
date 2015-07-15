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
 */

var _pathToQml = {};


/**
 * Get QML for a path.
 *
 * @param {String} path
 * @return {Display|null} qml from path or null if none found
*/
function getQML (path) {
    console.warn('getQML', path, '...');
    if (!_pathToQml.hasOwnProperty(path)) {
        console.warn('getQML returns null');
        return null;
    } else {
        console.warn('getQML returns', _pathToQml[path]);
        return _pathToQml[path];
    }
}

/**
 * Given an array of paths, it will create and associate
 * QML object for each new path.
 *
 * It will also delete any QML that is not in given list of paths.
 *
 * @param {Array} paths, array of paths
 */
function updateQML (paths) {
    console.warn('updateQML', paths);
    _garbageCollect(paths);
    _createQml(paths);
}

/**
 * Destroys QML.
 *
 * @param {String} path of object to delete
 * @return {Boolean} deletion successful
 */
function deleteQML (path) {
    console.warn('deleteQML', path);
    var qml;
    var i;
    if (!_pathToQml.hasOwnProperty(path)) {
        return false;
    } else {
        qml = _pathToQml[path];

        for (i = 0; i < displaysModel.count; i++) {
            if (qml.contextPath == displaysModel.get(i).path) {
                displaysModel.remove(i);
                break;
            }
        }

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
    console.warn('_garbageCollect', paths);
    var path;
    for (path in _pathToQml) {
        if (_pathToQml.hasOwnProperty(path)) {
            if (paths.indexOf(path) === -1) {
                deleteQML(path);
            }
        }
    }
}

/**
 * Creates QML for list in paths.
 *
 * @param {Array:String} list of paths
 * @param {String} path
 */
function _createQml (paths) {
    console.warn('_createQml', paths);
    var qml;
    paths.forEach(function (path, i) {
        if (!_pathToQml.hasOwnProperty(path)) {
            qml = createQML(path);
            _pathToQml[path] = qml;
        }
    });
}

/**
 * Creates qml object from a given path.
 * Since the components are all local, this will always return an object.
 *
 * @param {String} path
 * @return {Display} qml that was created
*/
function createQML (path) {
    console.warn('createQML', path);
    var qml;
    if (!_pathToQml.hasOwnProperty(path)) {
        qml = displayComponent.createObject(root, {
            'path': path
        });
        displaysModel.append({'path': path, 'display': qml});
        return qml;
    } else {
        return _pathToQml[path];
    }
}

/**
 * Handler for when displays change.
 *
 * @param {Array:String} paths after change
 */
function displaysChanged (paths) {
    console.warn('displaysChanged', paths);
    updateQML(paths);
}
