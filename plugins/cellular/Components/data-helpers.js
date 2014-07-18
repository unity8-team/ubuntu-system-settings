// maps keys to index of preference selector
function getIndexMap () {
    return sims.length === 2 ? {
        'gsm': 0,
        'any': 1,
        'lte': 1,
        'umts': 1,
        '': -1
    } : {
        'off': 0,
        'gsm': 1,
        'any': 2,
        'lte': 2,
        'umts': 2,
        '': -1
    };
}

/* return index of key k using data in techPref.model */
function keyToIndex (k) {
    console.warn('keyToIndex for key', k, getIndexMap()[k], '(have sims.lenght)', sims.length);
    return getIndexMap()[k];
}

function indexToKey (i) {
    var indices = getIndexMap();
    for (var k in indices) {
        if (indices.hasOwnProperty(k)) {
            if (indices[k] === i) {
                console.warn('indexToKey for i', i, k);
                return k;
            }
        }
    }
    console.warn('indexToKey for i', i, '[empty]');
    return ''
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
    console.log('poweredChanged rdoKey', rdoKey);
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
    console.warn('techSelectorClicked', index);
    // if the user selects a TechnologyPreference, update RadioSettings
    if (index > 0) {
        radioSettings.technologyPreference = indexToKey(index);
        console.warn('techSelectorClicked setting TechnologyPreference to', indexToKey(index));
    }
}

function dualTechSelectorClicked (index) {
    console.warn('dualTechSelectorClicked setting TechnologyPreference to', indexToKey(index));
    radioSettings.technologyPreference = indexToKey(index);
}

function simSelectorClicked (i) {
    console.warn('simSelectorClicked', i);
    sim1.connMan.powered = (i === 1);
    sim2.connMan.powered = (i === 2);
}
