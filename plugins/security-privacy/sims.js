var sims = [];

function add (sim) {
    sims.push(sim);
    root.simQmlObjects++;
}

function getAll () {
    return sims;
}

function get (n) {
    return getAll()[n];
}

function getFirstPresent () {
    return getPresent()[0];
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
