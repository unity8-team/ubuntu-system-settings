
/*
return index of k in techPrefModel
API docs: http://git.kernel.org/cgit/network/ofono/ofono.git/plain/doc/radio-settings-api.txt
*/
function keyToIndex (k) {
    console.warn('keyToIndex key:', k)
    for (var i=0; i < techPrefModel.count; i++) {
        if (indexToKey(i) === k) {
            console.warn('keyToIndex return:', i)
            return i;
        }
    }
    // we did not find a suitable ui item
    return -1;
}

function indexToKey (i) {
    return techPrefModel.get(i).key;
}

function getSelectedKey () {
    var sI = techPrefSelector.selectedIndex;
    var model = techPrefModel.get(sI);
    return model ? model.key : null;
}

function normalizeKey (k) {
    if (k === 'lte' || k === 'umts') {
        return 'any';
    } else {
        return k;
    }
}

function preferenceChanged (preference) {
    var sI = techPrefSelector.selectedIndex;
    var rdoKey = rdoSettings.technologyPreference;

    // if preference changes, but the user has chosen one already,
    // make sure the user's setting is respected
    if (sI > 0) {
        console.warn('pref change [rdo=', rdoKey, '], [ui=', getSelectedKey(), ']');
        rdoKey = getSelectedKey();
        return;
    }

    // if the pref changes and the modem is on,
    // normlize and update the UI
    if (connMan.powered) {
        sI = keyToIndex(normalizeKey(rdoKey));
    } else {
        // if the modem is off,
        // just normalize
        rdoKey = normalizeKey(rdoKey);
    }

}

function poweredChanged (powered) {
    var rdoKey = rdoSettings.technologyPreference;
    console.warn('power change [power=', powered, '], [rdo=', rdoKey, ']');
    if (powered) {
        if (rdoKey === '') {
            return;
        } else {
            techPrefSelector.selectedIndex = keyToIndex(normalizeKey(rdoKey));
        }
    } else {
        techPrefSelector.selectedIndex = 0;
    }
}

function delegateClicked (index) {
    console.warn('delegate clicked [index=', index, '], [power=', connMan.powered, '], [rdo=', rdoSettings.technologyPreference, ']');
    if (index > 0) {
        rdoSettings.technologyPreference = indexToKey(index);
    }
}
