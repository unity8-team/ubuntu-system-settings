var _CUSTOM_INTERNET_CONTEXT_NAME = '___ubuntu_custom_apn_internet';
var _CUSTOM_MMS_CONTEXT_NAME = '___ubuntu_custom_apn_mms';

/**
 * Updates a context with new values.
 *
 * @param {QOfonoConnectionContext} context
 * @param {Object} values dict with new values
 */
function updateContext (context, values) {
    var messageProxy;

    console.warn('updateContext', values.accessPointName, values.messageCenter,
        values.messageProxy, values.port, values.useraccessPointName,
        values.password, values.type);

    if (typeof values.accessPointName !== 'undefined') {
        context.accessPointName = values.accessPointName;
    }

    if (typeof values.messageCenter !== 'undefined') {
        context.messageCenter = values.messageCenter;
    }

    if (typeof values.messageProxy !== 'undefined') {
        messageProxy = values.messageProxy;

        if (messageProxy !== '') {
             messageProxy = messageProxy + ':' + values.port;
        }
        context.messageProxy = messageProxy;
    }

    if (typeof values.username !== 'undefined') {
        context.username = values.username;
    }

    if (typeof values.password !== 'undefined') {
        context.password = values.password;
    }

    if (typeof values.type !== 'undefined') {
        context.type = values.type;
    }

    if (context.type === 'internet') {
        context.name = CUSTOM_INTERNET_CONTEXT_NAME();
    } else if (context.type === 'mms') {
        context.name = CUSTOM_MMS_CONTEXT_NAME();
    }
}

/**
 * Exposes the custom internet context name.
 *
 * @return {String} custom internet context name
 */
function CUSTOM_INTERNET_CONTEXT_NAME () {
    return _CUSTOM_INTERNET_CONTEXT_NAME;
}

/**
 * Exposes the custom mms context name.
 *
 * @return {String} custom mms context name
 */
function CUSTOM_MMS_CONTEXT_NAME () {
    return _CUSTOM_MMS_CONTEXT_NAME;
}
