function build (modems) {
    modems.forEach(createOfonoGroup);
}

function createOfonoGroup (modem, i) {
    console.warn('Creating Ofono Group for', modem, ', naming it modem' + i);
    // var group = OfonoGroup.createObject(root, {
    //     modemPath: modem,
    //     name: 'modem' + i
    // });
}
