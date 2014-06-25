
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

// // function syncTechPrefUI (powered) {
// //     console.warn('syncTechPrefUI: ', connMan.powered, powered)
// //     if(connMan.powered || powered) {
// //         console.warn('syncTechPrefUI: powered, setting to', rdoSettings.technologyPreference)
// //         techPrefSelector.selectedIndex = techPrefKeyToIndex(rdoSettings.technologyPreference)
// //     }
// // }

function preferenceChanged (preference) {
    var userPref = systemSettingsSettings.cellularDataTechnologyPreference;

    // preference changed, but it differs from what the user wants
    // revert it
    if (preference !== userPref) {
        console.warn('preferenceChanged: new pref', preference, 'unlike user pref', userPref)
        rdoSettings.technologyPreference = userPref;
        return;
    } else {
        // assume it was request by user, do nothing
        console.warn('preferenceChanged: new pref', preference, 'matches user pref');
        return;
    }

}

// function preferenceChanged (preference) {
//     var newIndex = techPrefKeyToIndex(preference);
//     console.warn('onTechnologyPreferenceChanged', preference, newIndex);
//     if (newIndex > 0) {
//         techPrefSelector.selectedIndex = techPrefKeyToIndex(preference);
//     }

// }

function poweredChanged (powered) {
    var userPrefIndex = techPrefKeyToIndex(systemSettingsSettings.cellularDataTechnologyPreference);
    console.warn('Powered changed', powered, 'userPrefIndex', userPrefIndex, 'systemSettingsSettings.cellularDataTechnologyPreference', systemSettingsSettings.cellularDataTechnologyPreference);

    // modem powered on before setting ready
    if(userPrefIndex < 0) {
        return;
    }

    // if powering up, make sure we switch to the correct setting
    if(powered && (techPrefSelector.selectedIndex === 0)) {
        techPrefSelector.selectedIndex = userPrefIndex;
    }

    // powering down, only make a change if necessary
    if(!powered && (techPrefSelector.selectedIndex > 0)) {
        techPrefSelector.selectedIndex = 0;
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
        systemSettingsSettings.cellularDataTechnologyPreference = techPrefModel.get(index).key;
    }
}


// // function selectedIndexChanged (index) {
// //     var key = techPrefModel.get(selectedIndex).key;

// //     if (key === 'off') {
// //         //connMan.powered = false;
// //         console.warn('onSelectedIndexChanged wanted to power down')
// //     } else {
// //         //connMan.powered = true;
// //         console.warn('onSelectedIndexChanged wanted to power Up')
// //         console.warn('onSelectedIndexChanged changing technologyPreference to', key)
// //         rdoSettings.technologyPreference = key
// //     }
// // }

// // function selectedIndexChanged (index) {

// //     var key = techPrefModel.get(index).key;
// //     console.warn('selectedIndex changed', index);
// //     if (key === 'off') {
// //         connMan.powered = false;
// //         console.warn('onSelectedIndexChanged wanted to power down')
// //     } else if (key !== "") {
// //         connMan.powered = true;
// //         console.warn('onSelectedIndexChanged wanted to power Up')
// //         console.warn('onSelectedIndexChanged changing technologyPreference to', key)
// //         rdoSettings.technologyPreference = key
// //     } else {
// //         console.warn('onSelectedIndexChanged wanted to nuke rdoSettings', index, key)
// //     }
// // }


// function selectedIndexChanged (index) {
//     console.warn('Index changed', index);
//     if (index === 0) {
//         connMan.powered = false;
//     } else {
//         // set new radioSetting
//         rdoSettings.technologyPreference = techPrefModel.get(index).key
//     }

// }
