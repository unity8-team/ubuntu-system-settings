var sims = [];

function add (sim) {
    sims.push(sim);
    root.simsLoaded++;
}

function getAll () {
    return sims;
}

function get (n) {
    return getAll()[n];
}

function getCount () {
    return getAll().length;
}

function getPresent () {
    var present = [];
    getAll().forEach(function (sim) {
        if (sim.present) {
            present.push(sim);
        } else {
            return;
        }
    });
    return present;
}

function getPresentCount () {
    return getPresent().length;
}

function createQML () {
    var component = Qt.createComponent("Ofono.qml");

    sims.forEach(function (sim) {
        sim.destroy();
    });
    sims = [];

    root.modemsSorted.forEach(function (path, index) {
        var sim = component.createObject(root, {
            path: path,
            name: phoneSettings.simNames[path] ?
                phoneSettings.simNames[path] :
                "SIM " + (index + 1)
        });
        if (sim === null) {
            console.warn('Failed to create Sim qml:',
                component.errorString());
        } else {
            Sims.add(sim);
        }
    });
}

