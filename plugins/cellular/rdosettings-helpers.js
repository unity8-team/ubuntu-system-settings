
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

function getCurrentlySelectedKey () {
    var sI = techPrefSelector.selectedIndex;
    return techPrefModel.get(sI).key;
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
    var rdoPref = rdoSettings.technologyPreference;
    // if preference changes, but the user has chosen one already,
    // revert it
    if (sI > 0) {
        console.warn('pref change [rdo=', rdoPref, '], [ui=', getCurrentlySelectedKey(), ']');
        rdoPref = getCurrentlySelectedKey();
    } else {
        // if the modem wants umts or lte, normalize it (set to any)
        console.warn('pref change [rdo=', rdoPref, ' => ', normalizeKey(rdoPref), '], [ui=', getCurrentlySelectedKey(), ']');
        rdoPref = normalizeKey(rdoPref);
    }
}

function poweredChanged (powered) {
    var rdoPref = rdoSettings.technologyPreference;
    console.warn('power change [power=', powered, '], [rdo=', rdoPref, ']');
    if (powered) {
        if (rdoPref === '') {
            return;
        } else {
            techPrefSelector.selectedIndex = keyToIndex(normalizeKey(rdoPref));
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
