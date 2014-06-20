
/*
return number index of key in the technology preference model
LTE, UMTS and unknown ("") will return ANY

API docs: http://git.kernel.org/cgit/network/ofono/ofono.git/plain/doc/radio-settings-api.txt
*/
function techPrefKeyToIndex (k) {
    for (var i=0; i < techPrefModel.count; i++) {
        if (techPrefModel.get(i).key === k) {
            return i;
        }
    }

    // make settings "lte", "umts" and all other unknown techs,
    // synonymous with the "any" setting
    return techPrefModel.get(techPrefModel.count - 1); // any
}
