/*
 * Copyright (C) 2013 Canonical, Ltd.
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
#include <gcrypt.h>
#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusConnectionInterface>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusVariant>
#include <unistd.h>
#include <sys/types.h>

#define AS_INTERFACE "com.ubuntu.touch.AccountsService.SecurityPrivacy"

SecurityPrivacy::SecurityPrivacy(QObject* parent)
  : QObject(parent),
    m_lockSettings(QDir::home().filePath(".unity8-greeter-demo"),
                   QSettings::NativeFormat)
{
    m_lockSettings.beginGroup(qgetenv("USER"));

    // Ensure that file is 0600
    QFile settingsFile(m_lockSettings.fileName());
    settingsFile.open(QIODevice::ReadWrite);
    settingsFile.setPermissions(QFileDevice::ReadOwner |
                                QFileDevice::WriteOwner);
    settingsFile.close();
    m_lockSettings.sync();

    connect (&m_accountsService,
             SIGNAL (propertyChanged (QString, QString)),
             this,
             SLOT (slotChanged (QString, QString)));

    connect (&m_accountsService,
             SIGNAL (nameOwnerChanged()),
             this,
             SLOT (slotNameOwnerChanged()));
}

void SecurityPrivacy::slotChanged(QString interface,
                                  QString property)
{
    if (interface != AS_INTERFACE)
        return;

    if (property == "MessagesWelcomeScreen") {
        Q_EMIT messagesWelcomeScreenChanged();
    } else if (property == "StatsWelcomeScreen") {
        Q_EMIT statsWelcomeScreenChanged();
    }
}

void SecurityPrivacy::slotNameOwnerChanged()
{
    // Tell QML so that it refreshes its view of the property
    Q_EMIT messagesWelcomeScreenChanged();
    Q_EMIT statsWelcomeScreenChanged();
}

bool SecurityPrivacy::getStatsWelcomeScreen()
{
    return m_accountsService.getUserProperty(AS_INTERFACE,
                                             "StatsWelcomeScreen").toBool();
}

void SecurityPrivacy::setStatsWelcomeScreen(bool enabled)
{
    if (enabled == getStatsWelcomeScreen())
        return;

    m_accountsService.setUserProperty(AS_INTERFACE,
                                      "StatsWelcomeScreen",
                                      QVariant::fromValue(enabled));
    Q_EMIT(statsWelcomeScreenChanged());
}

bool SecurityPrivacy::getMessagesWelcomeScreen()
{
    return m_accountsService.getUserProperty(AS_INTERFACE,
                                             "MessagesWelcomeScreen").toBool();
}

void SecurityPrivacy::setMessagesWelcomeScreen(bool enabled)
{
    if (enabled == getMessagesWelcomeScreen())
        return;

    m_accountsService.setUserProperty(AS_INTERFACE,
                                      "MessagesWelcomeScreen",
                                      QVariant::fromValue(enabled));
    Q_EMIT(messagesWelcomeScreenChanged());
}

SecurityPrivacy::SecurityType SecurityPrivacy::getSecurityType()
{
    QVariant password(m_lockSettings.value("password", "none"));

     if (password == "pin")
        return SecurityPrivacy::Passcode;
    else if (password == "keyboard")
        return SecurityPrivacy::Passphrase;
    else
         return SecurityPrivacy::Swipe;

}

void SecurityPrivacy::setSecurityType(SecurityType type)
{
    QVariant sec;

    switch (type) {
    case SecurityPrivacy::Passcode:
        sec = "pin";
        break;
    case SecurityPrivacy::Passphrase:
        sec = "keyboard";
        break;
    case SecurityPrivacy::Swipe:
    default:
        sec = "none";
        break;
    }

    m_lockSettings.setValue("password", sec);
    m_lockSettings.sync();
    Q_EMIT (securityTypeChanged());
}

bool SecurityPrivacy::securityValueMatches(QString value)
{
    bool result = false;
    QVariant password(m_lockSettings.value("passwordValue", QString()));
    QStringList passwordParts = password.toString().split('$', QString::SkipEmptyParts);

    switch (getSecurityType()) {
    case SecurityPrivacy::Passcode:
    case SecurityPrivacy::Passphrase:
        // We only support passwd type 6 (sha512) for now
        if (passwordParts.length() != 3 || passwordParts[0] != "6")
            return false;
        result = makeSecurityValue(value, passwordParts[1]) == password;
        break;

    case SecurityPrivacy::Swipe:
    default:
        result = true;
        break;
    }

    return result;
}

void SecurityPrivacy::setSecurityValue(QString value)
{
    QString hash = makeSecurityValue(value);
    QByteArray encrypted = makeEncryptedPinValue(value);
    m_lockSettings.setValue("passwordValue", hash);
    m_lockSettings.setValue("passwordEncryptedValue", encrypted);
    m_lockSettings.sync();
    Q_EMIT (securityValueChanged());
}

QString SecurityPrivacy::makeSecurityValue(QString password, QString salt)
{
    // This function makes a /etc/shadow-compatible hash of the user's
    // password.  The goal is to be able to stuff it into /etc/shadow when we
    // actually migrate to PAM.

    // We only support passwd type 6 (sha512) for now
    QString command = "mkpasswd --method=sha-512 --stdin";
    if (!salt.isEmpty())
        command += " --salt=" + salt;

    QProcess process;
    process.start(command);
    process.write(password.toLatin1());
    process.closeWriteChannel();
    process.waitForFinished();

    return QString(process.readAllStandardOutput()).trimmed();
}

QByteArray SecurityPrivacy::makeEncryptedPinValue(QString password, QString salt)
{
    // OK.  So this function.  Like makeSecurityValue above, we are looking to
    // the future, when we migrate to PAM.  The way PIN passwords work in PAM
    // is via the pam_pin module provided by AccountsService.  This module
    // encrypts the user's password with a pin and puts it in a file on the
    // system only readable by root.  So when we migrate to PAM, we are going
    // to want to be able to recreate that encrypted version.  So we record the
    // encrypted version now and keep it in the keyfile.  But we never actually
    // use it.  It's just for eventual migration to PAM.  All the details of
    // this algorithm were just copied from pam_pin, to match what it does.
    // (Note that we're just encrypting the pin with itself here.  Touch
    // doesn't use a separate password and pin.)

    // Initialize gcrypt library (I know it doesn't look like an init function)
    gcry_check_version (GCRYPT_VERSION);

    size_t blockSize;
    gcry_cipher_algo_info (GCRY_CIPHER_AES256, GCRYCTL_GET_BLKLEN, NULL, &blockSize);

    if (salt.isEmpty()) {
        // Grab machine id, we'll use this as a salt
        QFile machineIdFile("/etc/machine-id");
        if (!machineIdFile.open(QIODevice::ReadOnly))
            return QByteArray();
        salt = machineIdFile.readAll(); // do *not* trim
    }

    // Derive key from password
    QByteArray key(256/8, 0);
    if (gcry_kdf_derive(password.toLatin1(), password.length(),
                        GCRY_KDF_PBKDF2, GCRY_MD_SHA1,
                        salt.toLatin1(), salt.length(),
                        100000, key.length(), key.data()))
        return QByteArray();

    // Encrypt password with this key
    gcry_cipher_hd_t hd = NULL;
    if (gcry_cipher_open(&hd, GCRY_CIPHER_AES256, GCRY_CIPHER_MODE_CBC, 0))
        return QByteArray();

    QByteArray iv(blockSize, 0);
    gcry_cipher_setiv(hd, iv.data(), iv.length());
    gcry_cipher_setkey(hd, key.data(), key.length());

    QByteArray padded(password.toLatin1());
    int remainder = padded.length() % blockSize;
    if (remainder != 0) {
        for (int i = 0; i < (int)blockSize - remainder; i++)
            padded.append((char)0);
    }

    QByteArray ciphertext(padded.length(), 0);
    if (gcry_cipher_encrypt(hd, ciphertext.data(), ciphertext.length(),
                            padded.data(), padded.length())) {
        gcry_cipher_close(hd);
        return QByteArray();
    }

    gcry_cipher_close(hd);
    return ciphertext;
}
