var sims = [];

function add (sim) {
    sims.push(sim);
    // console.warn('added sim', sim);
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
        // console.warn('foreach in present')
        if (sim.present) {
            // console.warn('sim present');
            present.push(sim);
        } else {
            // console.warn('sim not present');
            return;
        }
    });
    // console.warn('getPresent', present, present.length);
    return present;
}

function getPresentCount () {
    return getPresent().length;
}
