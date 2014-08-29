/*
 * Copyright (C) 2013,2014 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Michael Terry <michael.terry@canonical.com>
 *         Iain Lane <iain.lane@canonical.com>
 */

#include "securityprivacy.h"
#include <QtCore/QProcess>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusConnectionInterface>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusVariant>
#include <act/act.h>

// FIXME: need to do this better including #include "../../src/i18n.h"
// and linking to it
#include <libintl.h>
QString _(const char *text)
{
    return QString::fromUtf8(dgettext(0, text));
}

#define AS_INTERFACE "com.ubuntu.AccountsService.SecurityPrivacy"
#define AS_TOUCH_INTERFACE "com.ubuntu.touch.AccountsService.SecurityPrivacy"

void managerLoaded(GObject    *object,
                   GParamSpec *pspec,
                   gpointer    user_data);

SecurityPrivacy::SecurityPrivacy(QObject* parent)
  : QObject(parent),
    m_manager(act_user_manager_get_default()),
    m_user(nullptr)
{
    connect (&m_accountsService,
             SIGNAL (propertyChanged (QString, QString)),
             this,
             SLOT (slotChanged (QString, QString)));

    connect (&m_accountsService,
             SIGNAL (nameOwnerChanged()),
             this,
             SLOT (slotNameOwnerChanged()));

    if (m_manager != nullptr) {
        g_object_ref(m_manager);

        gboolean loaded;
        g_object_get(m_manager, "is-loaded", &loaded, nullptr);

        if (loaded)
            managerLoaded();
        else
            g_signal_connect(m_manager, "notify::is-loaded",
                             G_CALLBACK(::managerLoaded), this);
    }
}

SecurityPrivacy::~SecurityPrivacy()
{
    if (m_user != nullptr) {
        g_signal_handlers_disconnect_by_data(m_user, this);
        g_object_unref(m_user);
    }

    if (m_manager != nullptr) {
        g_signal_handlers_disconnect_by_data(m_manager, this);
        g_object_unref(m_manager);
    }
}

void SecurityPrivacy::slotChanged(QString interface,
                                  QString property)
{
    if (interface == AS_INTERFACE) {
        if (property == "EnableLauncherWhileLocked") {
            Q_EMIT enableLauncherWhileLockedChanged();
        } else if (property == "EnableIndicatorsWhileLocked") {
            Q_EMIT enableIndicatorsWhileLockedChanged();
        }
    } else if (interface == AS_TOUCH_INTERFACE) {
        if (property == "MessagesWelcomeScreen") {
            Q_EMIT messagesWelcomeScreenChanged();
        } else if (property == "StatsWelcomeScreen") {
            Q_EMIT statsWelcomeScreenChanged();
        }
    }
}

void SecurityPrivacy::slotNameOwnerChanged()
{
    // Tell QML so that it refreshes its view of the property
    Q_EMIT messagesWelcomeScreenChanged();
    Q_EMIT statsWelcomeScreenChanged();
    Q_EMIT enableLauncherWhileLockedChanged();
    Q_EMIT enableIndicatorsWhileLockedChanged();
}

bool SecurityPrivacy::getStatsWelcomeScreen()
{
    return m_accountsService.getUserProperty(AS_TOUCH_INTERFACE,
                                             "StatsWelcomeScreen").toBool();
}

void SecurityPrivacy::setStatsWelcomeScreen(bool enabled)
{
    if (enabled == getStatsWelcomeScreen())
        return;

    m_accountsService.setUserProperty(AS_TOUCH_INTERFACE,
                                      "StatsWelcomeScreen",
                                      QVariant::fromValue(enabled));
    Q_EMIT(statsWelcomeScreenChanged());
}

bool SecurityPrivacy::getMessagesWelcomeScreen()
{
    return m_accountsService.getUserProperty(AS_TOUCH_INTERFACE,
                                             "MessagesWelcomeScreen").toBool();
}

void SecurityPrivacy::setMessagesWelcomeScreen(bool enabled)
{
    if (enabled == getMessagesWelcomeScreen())
        return;

    m_accountsService.setUserProperty(AS_TOUCH_INTERFACE,
                                      "MessagesWelcomeScreen",
                                      QVariant::fromValue(enabled));
    Q_EMIT(messagesWelcomeScreenChanged());
}

bool SecurityPrivacy::getEnableLauncherWhileLocked()
{
    return m_accountsService.getUserProperty(AS_INTERFACE,
                                             "EnableLauncherWhileLocked").toBool();
}

void SecurityPrivacy::setEnableLauncherWhileLocked(bool enabled)
{
    if (enabled == getEnableLauncherWhileLocked())
        return;

    m_accountsService.setUserProperty(AS_INTERFACE,
                                      "EnableLauncherWhileLocked",
                                      QVariant::fromValue(enabled));
    Q_EMIT enableLauncherWhileLockedChanged();
}

bool SecurityPrivacy::getEnableIndicatorsWhileLocked()
{
    return m_accountsService.getUserProperty(AS_INTERFACE,
                                             "EnableIndicatorsWhileLocked").toBool();
}

void SecurityPrivacy::setEnableIndicatorsWhileLocked(bool enabled)
{
    if (enabled == getEnableIndicatorsWhileLocked())
        return;

    m_accountsService.setUserProperty(AS_INTERFACE,
                                      "EnableIndicatorsWhileLocked",
                                      QVariant::fromValue(enabled));
    Q_EMIT enableIndicatorsWhileLockedChanged();
}

SecurityPrivacy::SecurityType SecurityPrivacy::getSecurityType()
{
    if (m_user == nullptr || !act_user_is_loaded(m_user))
        return SecurityPrivacy::Passphrase; // we need to return something

    if (act_user_get_password_mode(m_user) == ACT_USER_PASSWORD_MODE_NONE)
        return SecurityPrivacy::Swipe;
    else if (m_accountsService.getUserProperty(AS_INTERFACE,
                                               "PasswordDisplayHint").toInt() == 1)
        return SecurityPrivacy::Passcode;
    else
        return SecurityPrivacy::Passphrase;
}

bool SecurityPrivacy::setDisplayHint(SecurityType type)
{
    if (!m_accountsService.setUserProperty(AS_INTERFACE, "PasswordDisplayHint",
                                           (type == SecurityPrivacy::Passcode) ? 1 : 0)) {
        return false;
    }

    Q_EMIT securityTypeChanged();
    return true;
}

bool SecurityPrivacy::setPasswordMode(SecurityType type)
{
    ActUserPasswordMode newMode = (type == SecurityPrivacy::Swipe) ?
                                  ACT_USER_PASSWORD_MODE_NONE :
                                  ACT_USER_PASSWORD_MODE_REGULAR;

    /* We call SetPasswordMode directly over DBus ourselves, rather than rely
       on the act_user_set_password_mode call, because that call gives no
       feedback! How hard would it have been to add a bool return? Ah well. */

    QString path = "/org/freedesktop/Accounts/User" + QString::number(geteuid());
    QDBusInterface iface("org.freedesktop.Accounts",
                         path,
                         "org.freedesktop.Accounts.User",
                         QDBusConnection::systemBus());

    QDBusReply<void> success = iface.call("SetPasswordMode", newMode);
    return success.isValid();
}

bool SecurityPrivacy::setPasswordModeWithPolicykit(SecurityType type, QString password)
{
    // SetPasswordMode will involve a check with policykit to see
    // if we have admin authorization.  Since Touch doesn't have a general
    // policykit agent yet (and the design for this panel involves asking for
    // the password up from anyway), we will spawn our own agent just for this
    // call.  It will only authorize one request for this pid and it will use
    // the password we pass it via stdin.  We can drop this helper code when
    // Touch has a real policykit agent and/or the design for this panel
    // changes.
    //
    // The reason we do this as a separate helper rather than in-process is
    // that glib's thread signal handling (needed to not block on the agent)
    // and QProcess's signal handling conflict.  They seem to get in each
    // other's way for the same signals.  So we just do this out-of-process.

    // But first, see if we have cached authentication
    if (setPasswordMode(type))
        return true;

    QProcess polkitHelper;
    polkitHelper.setProgram(HELPER_EXEC);
    polkitHelper.start();
    polkitHelper.write(password.toUtf8() + "\n");
    polkitHelper.closeWriteChannel();

    while (polkitHelper.canReadLine() || polkitHelper.waitForReadyRead()) {
        QString output = polkitHelper.readLine();
        if (output == "ready\n")
            break;
    }

    bool success = setPasswordMode(type);

    polkitHelper.waitForFinished();

    return success;
}

QString SecurityPrivacy::setPassword(QString oldValue, QString value)
{
    QByteArray passwdData;
    if (!oldValue.isEmpty())
        passwdData += oldValue.toUtf8() + '\n';
    passwdData += value.toUtf8() + '\n' + value.toUtf8() + '\n';

    QProcess pamHelper;
    pamHelper.setProgram("/usr/bin/passwd");
    pamHelper.start();
    pamHelper.write(passwdData);
    pamHelper.closeWriteChannel();
    pamHelper.setReadChannel(QProcess::StandardError);

    pamHelper.waitForFinished();
    if (pamHelper.state() == QProcess::Running || // after 30s!
        pamHelper.exitStatus() != QProcess::NormalExit ||
        pamHelper.exitCode() != 0) {
        QString output = QString::fromUtf8(pamHelper.readLine());
        if (output.isEmpty()) {
            return "Internal error: could not run passwd";
        } else {	
            // Grab everything on first line after the last colon.  This is because
            // passwd will bunch it up like so:
            // "(current) UNIX password: Enter new UNIX password: Retype new UNIX password: You must choose a longer password"
            return output.section(':', -1).trimmed();
        }
    }

    return "";
}

QString SecurityPrivacy::badPasswordMessage(SecurityType type)
{
    switch (type) {
        case SecurityPrivacy::Passcode:
            return _("Incorrect PIN code. Try again.");
        case SecurityPrivacy::Passphrase:
            return _("Incorrect passphrase. Try again.");
        default:
        case SecurityPrivacy::Swipe:
            return _("Could not set security mode");
    }
}

bool SecurityPrivacy::trySetSecurity(SecurityType type)
{
    if (m_user == NULL || !act_user_is_loaded(m_user))
        return false;

    // We only support setting swipe without more information
    if (type != SecurityPrivacy::Swipe)
        return false;

    SecurityType oldType = getSecurityType();
    if (type == oldType)
        return true; // nothing to do

    if (!setDisplayHint(type))
        return false;

    if (!setPasswordMode(type)) {
        setDisplayHint(oldType);
        return false;
    }

    return true;
}

QString SecurityPrivacy::setSecurity(QString oldValue, QString value, SecurityType type)
{
    if (m_user == nullptr || !act_user_is_loaded(m_user))
        return "Internal error: user not loaded";
    else if (type == SecurityPrivacy::Swipe && !value.isEmpty())
        return "Internal error: trying to set password with swipe mode";

    SecurityType oldType = getSecurityType();
    if (type == oldType && value == oldValue)
        return ""; // nothing to do

    // We need to set three pieces of metadata:
    //
    // 1) PasswordDisplayHint
    // 2) AccountsService password mode (i.e. is user in nopasswdlogin group)
    // 3) The user's actual password
    //
    // If we fail any one of them, the whole thing is a wash and we try to roll
    // the already-changed metadata pieces back to their original values.

    if (!setDisplayHint(type)) {
        return _("Could not set security display hint");
    }

    if (type == SecurityPrivacy::Swipe) {
        if (!setPasswordModeWithPolicykit(type, oldValue)) {
            setDisplayHint(oldType);
            return badPasswordMessage(oldType);
        }
    } else {
        QString errorText = setPassword(oldValue, value);
        if (!errorText.isEmpty()) {
            setDisplayHint(oldType);
            // Special case this common message because the one PAM gives is so awful
            if (errorText == dgettext("Linux-PAM", "Authentication token manipulation error"))
                return badPasswordMessage(oldType);
            else
                return errorText;
        }
        if (!setPasswordModeWithPolicykit(type, value)) {
            setDisplayHint(oldType);
            setPassword(value, oldValue);
            return badPasswordMessage(oldType);
        }
    }

    return "";
}

void securityTypeChanged(GObject    *object,
                         GParamSpec *pspec,
                         gpointer    user_data)
{
    Q_UNUSED(object);
    Q_UNUSED(pspec);

    SecurityPrivacy *plugin(static_cast<SecurityPrivacy *>(user_data));
    Q_EMIT plugin->securityTypeChanged();
}

void SecurityPrivacy::userLoaded()
{
    if (act_user_is_loaded(m_user)) {
        g_signal_handlers_disconnect_by_data(m_user, this);

        g_signal_connect(m_user, "notify::password-mode", G_CALLBACK(::securityTypeChanged), this);
        Q_EMIT securityTypeChanged();
    }
}

void userLoaded(GObject    *object,
                GParamSpec *pspec,
                gpointer    user_data)
{
    Q_UNUSED(object);
    Q_UNUSED(pspec);

    SecurityPrivacy *plugin(static_cast<SecurityPrivacy *>(user_data));
    plugin->userLoaded();
}

void SecurityPrivacy::managerLoaded()
{
    gboolean loaded;
    g_object_get(m_manager, "is-loaded", &loaded, nullptr);

    if (loaded) {
        g_signal_handlers_disconnect_by_data(m_manager, this);

        m_user = act_user_manager_get_user_by_id(m_manager, geteuid());

        if (m_user != nullptr) {
            g_object_ref(m_user);

            if (act_user_is_loaded(m_user))
                userLoaded();
            else
                g_signal_connect(m_user, "notify::is-loaded",
                                 G_CALLBACK(::userLoaded), this);
        }
    }
}

void managerLoaded(GObject    *object,
                   GParamSpec *pspec,
                   gpointer    user_data)
{
    Q_UNUSED(object);
    Q_UNUSED(pspec);

    SecurityPrivacy *plugin(static_cast<SecurityPrivacy *>(user_data));
    plugin->managerLoaded();
}

#include "securityprivacy.moc"
