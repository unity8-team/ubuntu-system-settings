// Map of path to OfonoNetworkOperator objects
var _pathToQml = {}

/*
Given an array of paths, it will create a OfonoNetworkOperator
QML object for each unseen operator.

It will also delete any QML objects that are not in operators.

@param paths Array of operator paths
@return undefined
*/
function updateOperatorQML (paths) {
    _garbageCollect(paths);
    _createQml(paths);
}

/* Check and remove QML cache elements that
do not appear in operator list, paths. */
function _garbageCollect (paths) {
    var path;
    for (path in _pathToQml) {
        if (_pathToQml.hasOwnProperty(path)) {
            if (paths.indexOf(path) === -1) {
                /* Found path that was not in the new operator list,
                let's remove it */
                console.warn('Destroyed path for path', _pathToQml[path].operatorPath, _pathToQml[path].name);
                _pathToQml[path].destroy();
                delete _pathToQml[path];
            }
        }
    }
}

/* Creates QML objects for each path in paths. */
function _createQml (paths) {
    paths.forEach(function (path, i) {
        if (!_pathToQml.hasOwnProperty(path)) {
            _pathToQml[path] = netOp.createObject(root, {
                'operatorPath': path
            });
            console.warn('_createQml created', path);
        }
    });
}

/*
Takes a list of paths and returns
OfonoNetworkOperator objects for each path.

@param paths Array of operator paths
@param ignore Array of operator paths to ignore
@return Array of OfonoNetworkOperators
*/
function getOps (paths, ignore) {
    var ret = [];
    ignore = ignore || [];
    paths.forEach(function (op) {
        var ofonoOp = getOrCreateOpQml(op);
        if (ignore.indexOf(op) >= 0) {
            console.warn('ignored', ofonoOp.name);
            return;
        } else if (ofonoOp.status === "forbidden") {
            console.warn('forbade', ofonoOp.name);
            return;
        }
        ret.push(ofonoOp);
    });
    return ret;
}

/*
Safe to be called with an empty path
@param path String an operator path
@return OfonoNetworkOperator or null if no QML exist for path
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

Since the OfonoNetworkOperator component is local
we can guarantee that it will be returned.

@param path String an operator path
@return OfonoNetworkOperator
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

@param path String operator to register
@return undefined
*/
function setOp (path) {
    var op = getOrCreateOpQml(path);
    op.registerOperator();
}
