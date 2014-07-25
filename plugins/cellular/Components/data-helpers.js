function singleSimKeyToIndex (k) {
    if (k === 'gsm') {
        return 1;
    } else if (k === 'any' || k === 'lte' || k === 'umts') {
        return 2;
    } else {
        return -1;
    }
}

function singleSimIndexToKey (i) {
    return i === 1 ? 'gsm' : 'any';
}

function dualSimKeyToIndex (k) {
    if (k === 'gsm') {
        return 0;
    } else {
        return 1;
    }
}

