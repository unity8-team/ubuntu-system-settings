function check (value) {
    console.warn('checking...', value);
    if (value) {
        if (cachedRuleValue) {
            console.warn('\t had cached rule, requesting change...');
            requestRule(cachedRuleValue);
        } else {
            d._editing = true;
            console.warn('\t editing...');
        }
    } else {
        if (d._editing) {
            console.warn('\t was editing. Stopping editing.');
            d._editing = false;
        } else {
            console.warn('\t disabling rule..');
            requestRule("");
        }
    }
}

/**
 * @return {Boolean} whether or not value passes client side verification.
 */
function requestRule (value) {
    if (value === callForwarding[ruleName]) {
        console.warn('Value did not change.');
        return false;
    }
    clientRule = value;
    console.warn('requesting rule', value, '...');
    callForwarding[ruleName] = value;
    d._pending = true;
    return true;
}
