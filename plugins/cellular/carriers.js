var _pathToQml = {}
var _allowedOps = []

/*
Returns an array of OfonoNetworkOperator objects,
that has a non-forbidden status.

@returns Array of OfonoNetworkOperator elements
*/
function getAllowedOps () {
    return _allowedOps;
}

/*
Given an array of paths, it will create a OfonoNetworkOperator
QML object for each unseen operator.

It will also delete any QML objects that are not in operators.

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

    for (path in _pathToQml) {
        if (_pathToQml.hasOwnProperty(path)) {
            if (_pathToQml[path].status !== "forbidden") {
                allowed.push(_pathToQml[path]);
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
Returns the index of path in our list of
allowed operators.

Returns a negative number if no current operator was found.

@path String a operator path
@returns Number index of the operator
*/
function getOpIndex (path) {
    console.warn('getOpIndex arg', path);
    if (!_pathToQml.hasOwnProperty(path)) {
        console.warn('getOpIndex ret', -1);
        return -1;
    }
    console.warn('getOpIndex ret (late)', _allowedOps.indexOf(_pathToQml[path]));
    return _allowedOps.indexOf(_pathToQml[path]);
}

/*
Sets the current operator. It does this by calling
registerOperator on the operator QML object.

@path String operator path of the new current operator
@returns undefined
*/
function setCurrentOp (path) {
    console.warn('Registering', _pathToQml[path].name);
    _pathToQml[path].registerOperator();
    root.operatorsChanged('setCurrentOp');
}

/* Call this to check and remove QML cache elements that
do not appear in operator list, newOps. */
function _garbageCollect (newOps) {
    var path;
    for (path in _pathToQml) {
        if (_pathToQml.hasOwnProperty(path)) {
            /* Found path that was not in the new operator list,
            let's remove it */
            if (newOps.indexOf(path) === -1) {
                console.warn('Destroyed path for path', _pathToQml[path].operatorPath, _pathToQml[path].name);
                _pathToQml[path].destroy();
                delete _pathToQml[path];
            }
        }
    }
}

function getOpName (path) {
    console.warn('getOpName', path);
    var name = "";
    if (_pathToQml.hasOwnProperty(path)) {
        name = _pathToQml[path].name;
    } else {
        throw new TypeError('OperatorPath', path, 'not in cache');
    }
    return name;
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
