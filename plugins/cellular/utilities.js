
/*
return index of k in techPrefModel
API docs: http://git.kernel.org/cgit/network/ofono/ofono.git/plain/doc/radio-settings-api.txt
*/
function techPrefKeyToIndex (k) {
    console.warn('techPrefKeyToIndex key:', k)
    for (var i=0; i < techPrefModel.count; i++) {
        if (techPrefModel.get(i).key === k) {
            console.warn('techPrefKeyToIndex return:', i)
            return i;
        }
    }


    // we did not find a suitable ui item
    return -1;
}

function preferenceChanged (preference) {
    console.warn('preferenceChanged', preference);
    if(preference === 'lte' || preference === 'umts') {
        rdoSettings.technologyPreference = 'any';
        return;
    }
    // only change the selection if preference (Radio Settings interface available)
    // and the modem is powered
    if(preference !== '' && connMan.powered) {
        console.warn('preferenceChanged, setting selectedIndex to', techPrefKeyToIndex(preference));
        techPrefSelector.selectedIndex = techPrefKeyToIndex(preference);
    }
}

function poweredChanged (powered) {
    var i = techPrefSelector.selectedIndex;
    var newIndex = techPrefKeyToIndex(rdoSettings.technologyPreference);
    console.warn('poweredChanged', powered, i);
    if(powered) {
        if (i === 0) {
            if (newIndex >= 0) {
                console.warn('poweredChanged, setting selectedIndex to', newIndex);
                techPrefSelector.selectedIndex = newIndex;
            }
        }
    } else {
        if (i > 0) {
            console.warn('poweredChanged, setting selectedIndex to', 0);
            techPrefSelector.selectedIndex = 0;
        }
    }
}


function selectedIndexChanged (index) {
    if (index === 0) {
        console.warn('onSelectedIndexChanged wanted to power down');
        connMan.powered = false;
    } else {
        connMan.powered = true;
        console.warn('onSelectedIndexChanged wanted to power up');
        console.warn('onSelectedIndexChanged setting user pref', techPrefModel.get(index).key);
        rdoSettings.technologyPreference = techPrefModel.get(index).key;
    }
}

function interfacesChanged (interfaces) {

    var pref;

    if(interfaces.indexOf('org.ofono.RadioSettings') >= 0) {
        pref = rdoSettings.technologyPreference;
        if (pref === 'umts' || pref === 'lte') {
            console.warn('interfacesChanged[RadioSettings online]: saw', pref, 'setting to any');
            rdoSettings.technologyPreference = 'any';
        } else if (pref === '') {
            console.warn('interfacesChanged[RadioSettings online]: saw empty pref');
            return;
        } else {
            console.warn('interfacesChanged[RadioSettings online]: saw ok pref', pref, 'setting to', techPrefKeyToIndex(rdoSettings.technologyPreference));
            techPrefSelector.selectedIndex = techPrefKeyToIndex(rdoSettings.technologyPreference)
            techPrefSelector.enabled = true;
        }

    } else {
        console.warn('offline: org.ofono.RadioSettings');
        techPrefSelector.enabled = false;
    }
}
