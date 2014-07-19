/* return key or 'any' if key matches 'lte' or 'umts'
The UI currently does not support umts/lte only
*/

function normalizeKey (k) {
    if (k === 'lte' || k === 'umts') {
        console.warn("normalizeKey saw", k);
        return 'any';
    } else {
        return k;
    }
}

function singleIndexToKey (i) {
    if (i === 0) {
        return 'off';
    } else if (i === 1) {
        return 'gsm';
    } else {
        return 'any';
    }
}

function singleSimKeyToIndex (k) {
    if (k === 'off') {
        return 0;
    } else if (k === 'gsm') {
        return 1;
    } else if (k === 'any' || k === 'lte' || k === 'umts') {
        return 2;
    } else {
        return -1;
    }
}

function dualIndexToKey (i) {
    if (i === 0) {
        return 'gsm';
    } else {
        return 'any';
    }
}

function dualKeyToIndex (k) {
    if (k === 'gsm') {
        return 0;
    } else {
        return 1;
    }
}

function dualTechSelectorClicked (index) {
    console.warn('dualTechSelectorClicked setting TechnologyPreference to', dualIndexToKey(index));
    getSelectedSim().radioSettings.technologyPreference = dualIndexToKey(index);
}

function simSelectorClicked (i) {
    console.warn('simSelectorClicked', i);
    sim1.connMan.powered = (i === 1);
    sim2.connMan.powered = (i === 2);
}
