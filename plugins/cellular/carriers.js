var _netopCache = {}
var _allowedOperators = []

/*
Returns an array of OfonoNetworkOperator objects,
that a non-forbidden status.

@operators A QStringList of operator paths
@returns Array of OfonoNetworkOperator elements
*/
function allowedOperators (operators) {

    d.__suppressActivation = true;
    _garbageCollect(operators);
    _createQml(operators);
    _allowedOperators = _getAllowedOperators();
    d.__suppressActivation = false;

    return _allowedOperators;
}

function _getAllowedOperators () {
    var allowed = [];
    var operatorQml;

    /* Go through cache of netop objects, find those that are not forbidden */
    for (operatorQml in _netopCache) {
        if (_netopCache.hasOwnProperty(operatorQml)) {
            if (_netopCache[operatorQml].status !== "forbidden") {
                allowed.push(_netopCache[operatorQml]);
                console.warn(_netopCache[operatorQml].mnc)
            }
        }
    }
    // Sort all modems on modem path before returning them
    return allowed.sort(function (a, b) {
        if (a.mnc < b.mnc) return -1;
        if (a.mnc > b.mnc) return 1;
        return 0;
    });
}

/*
Returns an index of the current network operator.
Uses internal list of allowed operators to find the index.

Returns a negative number if no current operator was found.

@returns Number index of the current operator
*/
function getCurrentOperator () {
    var allowed = -1;
    _allowedOperators.forEach(function (op, i) {
        if (op.status === 'current') {
            console.warn('Current op:', op.name);
            allowed = i;
        }
    });
    return allowed;
}

/*
Sets the current operator. It does this by calling
registerOperator on the operator QML object.

@index Number index of the new current operator
@returns undefined
*/
function setCurrentOperator (index) {
    console.warn('Registering', _allowedOperators[index].name);
    _allowedOperators[index].registerOperator();
}

/* Call this to check and remove QML cache elements that
do not appear in operator list, newOperators. */
function _garbageCollect (newOperators) {
    var path;
    for (path in _netopCache) {
        if (_netopCache.hasOwnProperty(path)) {
            /* Found path that was not in the new operator list,
            let's remove it */
            if (newOperators.indexOf(path) === -1) {
                console.warn('Destroyed path for path', _netopCache[path].operatorPath, _netopCache[path].name);
                _netopCache[path].destroy();
            }
        }
    }
}

/* Creates QML objects for each path in paths. */
function _createQml (paths) {
    paths.forEach(function (path, i) {
        if (!_netopCache.hasOwnProperty(path)) {
            _netopCache[path] = netOp.createObject(root, {
                'operatorPath': path
            });
            console.warn('_createQml created', path);
        } else {
            console.warn('_createQml ignored', path);
        }

    });
}
