function normalizeKey (k) {
    if (k === 'lte' || k === 'umts') {
        console.warn("normalizeKey saw", k);
        return 'any';
    } else {
        return k;
    }
}

function singleIndexToKey (i) {
    if (i === 0) {
        return 'off';
    } else if (i === 1) {
        return 'gsm';
    } else {
        return 'any';
    }
}

function singleSimKeyToIndex (k) {
    if (k === 'off') {
        return 0;
    } else if (k === 'gsm') {
        return 1;
    } else if (k === 'any' || k === 'lte' || k === 'umts') {
        return 2;
    } else {
        return -1;
    }
}

function dualIndexToKey (i) {
    if (i === 0) {
        return 'gsm';
    } else {
        return 'any';
    }
}

function dualSimKeyToIndex (k) {
    if (k === 'gsm') {
        return 0;
    } else {
        return 1;
    }
}

