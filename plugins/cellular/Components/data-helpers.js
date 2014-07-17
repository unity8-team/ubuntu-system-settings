

/* return index of key k using data in techPref.model */
function keyToIndex (k) {
    var ret;
    switch (k) {
        case 'gsm':
            ret = 1;
            break;
        case 'umts':
        case 'lte':
        case 'any':
            ret = 2;
            break;
        default:
            ret = -1;
            break;
    }
    console.warn('keyToIndex for key', k, ret);
    return ret;
}

function indexToKey (i) {
    if (i === 1) {
        return 'gsm';
    } else if (i === 2) {
        return 'any'
    } else {
        return -1
    }
}

/* return currently selected key or null if none selected */
function getSelectedKey () {
    var i = selector.selectedIndex;
    if (i < 0) {
        return null;
    }
    return indexToKey(i);
}

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

/* handler for when RadioSettings TechnologyPreference changes */
function preferenceChanged (preference) {
    var i = selector.selectedIndex;
    var rdoKey = radioSettings.technologyPreference;
    var selKey = getSelectedKey();

    // if preference changes, but the user has chosen one already,
    // make sure the user's setting is respected
    if (i > 0) {
        console.warn('Overriding RadioSettings TechnologyPreference signal', preference, 'with user selection', selKey);
        radioSettings.technologyPreference = selKey;
        return;
    }

    // if the pref changes and the modem is on,
    // normlize and update the UI
    if (connMan.powered) {
        selector.selectedIndex = keyToIndex(normalizeKey(rdoKey));
    } else {
        // if the modem is off,
        // just normalize
        radioSettings.technologyPreference = normalizeKey(rdoKey);
    }
    console.warn('Modem', connMan.powered ? 'online' : 'offline', 'TechnologyPreference', rdoKey);
}

/* handler for when ConnectionManager powered changes */
function poweredChanged (powered) {
    var rdoKey = radioSettings.technologyPreference;
    if (powered) {
        if (rdoKey === '') {
            console.warn('Modem came online but TechnologyPreference is empty');
            return;
        } else {
            console.warn('Modem came online, TechnologyPreference', rdoKey);
            selector.selectedIndex = keyToIndex(normalizeKey(rdoKey));
        }
    } else {
        console.warn('Modem went offline');
        selector.selectedIndex = 0;
    }
}

/* handler for when user clicks the TechnologyPreference item selector */
function techSelectorClicked (index) {
    console.warn('delegateClicked', index);
    // if the user selects a TechnologyPreference, update RadioSettings
    if (index > 0) {
        radioSettings.technologyPreference = indexToKey(index);
        console.warn('delegateClicked setting TechnologyPreference to', indexToKey(index));
    }
}

function dualTechSelectorClicked (index) {

}

function simSelectorClicked (i) {
    console.warn('simSelectorClicked', i);
    sim1.connMan.powered = (i === 1);
    sim2.connMan.powered = (i === 2);
}
