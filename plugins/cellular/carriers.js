var _netopCache = {}
var _allowedOps = []

/*
Returns an array of OfonoNetworkOperator objects,
that a non-forbidden status.

@returns Array of OfonoNetworkOperator elements
*/
function getAllowedOps () {
    return _allowedOps;
}

/*
Given OfonoNetworkRegistration operators,
it will create a OfonoNetworkOperator QML object for each unseen
operator.

@operator Array of String
@returns undefined
*/
function updateOperatorQML (operators) {
    d.__suppressActivation = true;
    _garbageCollect(operators);
    _createQml(operators);
    _allowedOps = _getAllowedOps();
    d.__suppressActivation = false;
}

function _getAllowedOps () {
    var allowed = [];
    var path;

    // Go through cache of netop objects, find those that are not forbidden
    for (path in _netopCache) {
        if (_netopCache.hasOwnProperty(path)) {
            if (_netopCache[path].status !== "forbidden") {
                allowed.push(_netopCache[path]);
            }
        }
    }
    // Sort all modems on modem network code before returning them
    return allowed.sort(function (a, b) {
        if (a.mnc < b.mnc) return -1;
        if (a.mnc > b.mnc) return 1;
        return 0;
    });
}

/*
Returns an index of the provided network operator.

Returns a negative number if no current operator was found.
@path String a operator path
@returns Number index of the operator
*/
function getOpIndex (path) {
    console.warn('getOpIndex arg', path);
    if (!_netopCache.hasOwnProperty(path)) {
        console.warn('getOpIndex ret', -1);
        return -1;
    }
    console.warn('getOpIndex ret (late)', _allowedOps.indexOf(_netopCache[path]));
    return _allowedOps.indexOf(_netopCache[path]);
}

/*
Sets the current operator. It does this by calling
registerOperator on the operator QML object.

@path String operator path of the new current operator
@returns undefined
*/
function setCurrentOp (path) {
    console.warn('Registering', _netopCache[path].name);
    _netopCache[path].registerOperator();
    root.operatorsChanged('setCurrentOp');
}

/* Call this to check and remove QML cache elements that
do not appear in operator list, newOps. */
function _garbageCollect (newOps) {
    var path;
    for (path in _netopCache) {
        if (_netopCache.hasOwnProperty(path)) {
            /* Found path that was not in the new operator list,
            let's remove it */
            if (newOps.indexOf(path) === -1) {
                console.warn('Destroyed path for path', _netopCache[path].operatorPath, _netopCache[path].name);
                _netopCache[path].destroy();
            }
        }
    }
}

function getOpName (path) {
    console.warn('getOpName', path);
    var name = "";
    if (_netopCache.hasOwnProperty(path)) {
        name = _netopCache[path].name;
    } else {
        console.warn('_netopCache did not have', path);
        name = path;
    }
    return name;
}

/* Creates QML objects for each path in paths. */
function _createQml (paths) {
    paths.forEach(function (path, i) {
        if (!_netopCache.hasOwnProperty(path)) {
            _netopCache[path] = netOp.createObject(root, {
                'operatorPath': path
            });
            console.warn('_createQml created', path);
        }
    });
}

function operatorsChanged () {
    console.warn('operatorsChanged');
    var curOp = sim.netReg.currentOperatorPath;
    CHelper.updateOperatorQML(sim.netReg.networkOperators);
    carrierSelector.model = CHelper.getAllowedOps();
    if (curOp) {
        carrierSelector.selectedIndex = CHelper.getOpIndex(curOp);
    }
    if (curOp) {
        curOpLabel.text = CHelper.getOpName(curOp);
    }
}
