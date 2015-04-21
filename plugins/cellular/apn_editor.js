
function updateContext (ctx) {
    console.warn('updateContext', accessPointName.text, username.text, password.text);
    ctx.accessPointName = accessPointName.text;
    ctx.username = username.text;
    ctx.password = password.text;

    if (ctx.type === 'mms') {
        context.messageCenter = messageCenter.text;
        context.messageProxy = messageProxy.text + (port.text ? ':' + port.text : '');
    }
}

function populate (context) {
    console.warn('populate');
    accessPointName.text = context.accessPointName;
    username.text = context.username;
    password.text = context.password;

    if (isMms) {
        messageCenter.text = context.messageCenter;
        messageProxy.text = context.messageProxy;
    }
}

function activateButtonPressed () {
    console.warn('activateButtonPressed');
    // Do we have a custom context?
    var ctx = apnLib.getCustomContext(contextModel.type);

    if (ctx) {
        // We have a custom context we want to change.
        // We cannot change it if it is active. If active,
        // we need to defer changing it until it has been
        // deactivated.
        if(ctx.active) {
            console.warn('Deactivating context before changing.');
            ctx.disconnect();
        }
        console.warn('Changing context');
        updateContext(ctx);

        root.activated(ctx.contextPath, ctx.type);
    } else {
        // We will create a new context. This is async, so
        // we attach a one time event to addition of contexts.
        // We can't guarantee that the context added is this
        // we just created.

        function updateCreatedContext () {
            var ctx = contextModel.get(contextModel.count - 1).qml;
            var i;
            console.warn('updateCreatedContext', ctx.name, ctx);
            ctx.disconnect();
            updateContext(ctx);

            // Update selected index of suggestions list
            copyFromMms.selectedIndex = -1;
            copyFromInternet.selectedIndex = -1;
            for (i = 0; i < suggestions.model.count; i++) {
                if (suggestions.model.get(i).qml === ctx) {
                    suggestions.selectedIndex = i;
                }
            }

            contextModel.countChanged.disconnect(updateCreatedContext);
            root.activated(ctx.contextPath, ctx.type);
        }

        contextModel.countChanged.connect(updateCreatedContext);
        apnLib.createContext(contextModel.type);
    }
}

// function checkContextAppeared () {
//     console.warn('checkContextAppeared');
//     var ctx = apnLib.getCustomContext(contextModel.type);
//     var i;
//     if (ctx) {
//         root.suggestion = ctx;

//         // Update selected index of suggestions list
//         copyFromMms.selectedIndex = -1;
//         copyFromInternet.selectedIndex = -1;
//         for (i = 0; i < suggestions.model.count; i++) {
//             if (suggestions.model.get(i).qml === ctx) {
//                 suggestions.selectedIndex = i;
//             }
//         }


//         // The context should be deactivated here, but if
//         // not, we deactivate it.
//         if (ctx.active) {
//             ctx.disconnect();
//         }
//         updateContext(ctx);
//         waitForCreatedContextTimer.stop();
//         console.warn('Saw new model, trying to activate.');
//         root.activated(ctx.contextPath, ctx.type);
//     } else {
//         console.warn('No created model yet...');

//         // If a we're waiting for a custom context, and it's
//         // not created yet, maybe the manager failed to change
//         // the name (which we use to identify custom contexts).
//         // This currently only happens using ofono-phonesim.
//         var defaultOfonoName;
//         if (isMms) {
//             defaultOfonoName = 'MMS';
//         } else if (isInternet) {
//             defaultOfonoName = 'Internet';
//         } else if (isIa) {
//             defaultOfonoName = 'IA';
//         }
//         for (i = 0; i < contextModel.count; i++) {
//             if (contextModel.get(i).qml.name === defaultOfonoName) {
//                 apnLib.contextNameChanged.call(contextModel.get(i).qml, defaultOfonoName);
//                 break;
//             }
//         }
//     }

//     if (waitForCreatedContextTimer.waited > 30) {
//         root.failed();
//         waitForCreatedContextTimer.stop();
//     }
//     waitForCreatedContextTimer.waited++;
// }

function hasProtocol (link) {
    return link.search(/^http[s]?\:\/\//) == -1;
}

function setHttp(link) {
    if (hasProtocol(link)) {
        link = 'http://' + link;
    }
    return link;
}

function isChanged () {
    // We had no suggestion, so we have nothing to compare it to.
    if (!suggestion) {
        return true;
    }

    var refData = [suggestion.accessPointName,
                   suggestion.username,
                   suggestion.password];
    var formData = [accessPointName.text,
                    username.text,
                    password.text];
    var i;

    // If we are comparing mms, add more data.
    if (isMms) {
        refData.push(suggestion.messageCenter);
        formData.push(messageCenter.text);

        refData.push(suggestion.messageProxy);
        formData.push(messageProxy.text + (port.text ? ':' + port.text : ''));
    }

    // Compare the arrays.
    i = refData.length;
    while(i--) {
        if (refData[i] !== formData[i]) return true;
    }
    return false;
}

function isValid () {
    return accessPointName.text;
}

function prePopulate () {
    if (contextModel.current) {
        suggestion = contextModel.current;
        Editor.populate(contextModel.current);
        // Update selectedIndex of suggestsions
        for (var i = 0; i < suggestions.model.count; i++) {
            if (suggestions.model.get(i).qml === contextModel.current) {
                suggestions.selectedIndex = i;
            }
        }
    }
}
