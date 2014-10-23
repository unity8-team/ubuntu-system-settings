
/* Destroys all operator objects supplied in remove from list */
function destroyOperatorObjects (list, remove) {
    remove.forEach(function (currentValue, index, array) {
        if (list[currentValue] === undefined) {
            throw new ReferenceError('destroyOperatorObjects: cannot remove' +
                'an undefined operator.');
        }
        list[currentValue].destroy();
        delete list[currentValue];
        console.warn('destroyOperatorObjects', currentValue);
    });
}

/* Create operator objects for all in create, and adds them to list */
function createOperatorObjects (list, create) {
    create.forEach(function (currentValue, index, array) {
        // just asserting to verify the logic
        // remove once proven functional
        if (list[currentValue] !== undefined) {
            throw new ReferenceError('createOperatorObjects: cannot create' +
                'an undefined operator.');
        }

        list[currentValue] = netOp.createObject(root, {
            'operatorPath': currentValue
        });
        console.warn('createOperatorObjects', currentValue);
    });
}

/* Asserting to verify the logic,
removing once proven functional –– Wellark */
function carriersInvariant (ops, tmp) {
    var opsLength = Object.keys(ops).length;
    if (opsLength !== tmp.length) {
        throw new Error('Failed to assert that operators' +
            ' and tmp are equal', opsLength, '!==', tmp.length);
    }
    tmp.forEach(function (currentValue, index, array) {
        if (ops[currentValue] === undefined)
            throw new ReferenceError('Operators had undefined operator.');
    });
}


function buildLists () {
    d.__suppressActivation = true;
    var oN = new Array();

    for (var i in operators) {
        var tempOp = operators[i];
        if (tempOp.status === "forbidden")
            continue
        oN.push(tempOp.name);
    }
    operatorNames = oN;

    var cur = operators[sim.netReg.currentOperatorPath];
    carrierSelector.selectedIndex = cur === undefined ? -1 : operatorNames.indexOf(cur.name);
    d.__suppressActivation = false;
}


function updateNetworkOperators () {
    var tmp = sim.netReg.networkOperators;
    var added = tmp.filter(function (i) {
        return operators[i] === undefined;
    });
    var removed = Object.keys(operators).filter(function (i) {
        return tmp.indexOf(i) === -1;
    })

    destroyOperatorObjects(operators, removed);
    createOperatorObjects(operators, added);
    carriersInvariant(operators, tmp);

    buildLists();
}
