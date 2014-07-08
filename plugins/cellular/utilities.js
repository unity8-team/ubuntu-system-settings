
/*
return number index of key in the technology preference model
LTE, UMTS and unknown ("") will return ANY

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
                techPrefSelector.selectedIndex = ;
            }
        }
    } else {
        if (i > 0) {
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
    console.warn(interfaces);
    if(interfaces.indexOf('org.ofono.RadioSettings') >= 0) {
        console.warn('online: org.ofono.RadioSettings');
        console.warn('interfacesChanged so setting selectedIndex to', techPrefKeyToIndex(rdoSettings.technologyPreference));
        techPrefSelector.enabled = true;
        techPrefSelector.selectedIndex = techPrefKeyToIndex(rdoSettings.technologyPreference)
    } else {
        console.warn('offline: org.ofono.RadioSettings');
        techPrefSelector.enabled = false;
    }
}
