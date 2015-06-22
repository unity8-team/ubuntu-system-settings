function checked (value) {
    console.warn('checking...', value);
    if (value) {
        if (item.cachedRuleValue) {
            console.warn('\t had cached rule, requesting change...');
            requestRule(item.cachedRuleValue);
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
            requestRule('');
        }
    }
}

/**
 * @return {Boolean} whether or not value passes client side verification.
 */
function requestRule (value) {
    if (value === item.callForwarding[item.rule]) {
        console.warn('Value did not change.');
        return false;
    }

    console.warn('requesting rule', value, '...');
    item.callForwarding[item.rule] = value;
    d._pending = true;
    return true;
}

function editingChanged () {
{
    console.warn('editingChanged');
    if (d._editing) {
        item.enteredEditMode();
        field.forceActiveFocus();
        console.warn('firing editing');
    } else {
        console.warn('firing stoppedEditing');
        item.leftEditMode();
    }
}
}

function ruleChanged (property) {
    console.warn(item.rule + 'Changed', property);
    check.checked = callForwarding[rule] !== "";
}

function ruleComplete (success) {
    console.warn(item.rule + 'Complete', success);
    d._pending = false;
    d._editing = false;
    if (!success) {
        d._failed = true;
    }
}
