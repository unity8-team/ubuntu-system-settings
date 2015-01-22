// Map of path to OfonoNetworkOperator objects
var _pathToQml = {}

/*
Given an array of paths, it will create and associate
an OfonoNetworkOperator QML object for each new path.

It will also delete any QML that is not in given list of paths.

@param {Array} paths - Array of operator paths
@return {undefined}
*/
function updateOperatorQML (paths) {
    _garbageCollect(paths);
    _createQml(paths);
}

function _garbageCollect (paths) {
    var path;
    for (path in _pathToQml) {
        if (_pathToQml.hasOwnProperty(path)) {
            if (paths.indexOf(path) === -1) {
                _pathToQml[path].destroy();
                delete _pathToQml[path];
            }
        }
    }
}

function _createQml (paths) {
    paths.forEach(function (path, i) {
        if (!_pathToQml.hasOwnProperty(path)) {
            _pathToQml[path] = netOp.createObject(root, {
                'operatorPath': path
            });
        }
    });
}

/*
Takes a list of paths and returns
OfonoNetworkOperator objects for each path.

This function will create OfonoNetworkOperator
objects.

@param {Array} paths - Array of operator paths
@param {Array} ignore - Array of operator paths to ignore
@return {Array} of OfonoNetworkOperators
*/
function getOps (paths, ignore) {
    var ret = [];
    ignore = ignore || [];
    paths.forEach(function (op) {
        var ofonoOp = getOrCreateOpQml(op);
        if (ignore.indexOf(op) >= 0) {
            return;
        } else if (ofonoOp.status === "forbidden") {
            return;
        }
        ret.push(ofonoOp);
    });
    return ret;
}

/*
@param path String an operator path
@return {Object|null} OfonoNetworkOperator|null - null if no QML exist for path
*/
function getOp (path) {
    if (_pathToQml.hasOwnProperty(path)) {
        return _pathToQml[path];
    } else {
        return null;
    }
}

/*
Returns an operator. Before returning it sees
if we have created QML for this operator path
before. QML is created if not.

It is guaranteed that a QML object will be returned.

@param {String} path - an operator path
@return {Object} OfonoNetworkOperator - the created qml
*/
function getOrCreateOpQml (path) {
    if (getOp(path)) {
        return getOp(path);
    } else {
        _createQml([path]);
        return getOp(path);
    }
}

/*
Registers operator on path

@param {String} path - operator to register
@return {undefined}
*/
function setOp (path) {
    var op = getOrCreateOpQml(path);
    op.registerOperator();
}
