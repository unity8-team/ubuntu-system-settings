/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QDebug>
#include <QFile>
#include <QObject>
#include <QProcess>
#include <QTest>
#include <QString>
#include "securityprivacy.h"

class PasswordTest: public QObject
{
    Q_OBJECT

public:
    PasswordTest() {};

private Q_SLOTS:
    void init();
    void testNewFilePermissions();
    void testOldFilePermissions();
    void testFileContents();
    void testMakeSecurityValue();
    void testSecurityValueMatches();
};

void PasswordTest::init()
{
    QCOMPARE(QProcess::execute("rm -f " + qgetenv("HOME") + "/.unity8-greeter-demo"), 0);
}

void PasswordTest::testNewFilePermissions()
{
    QFile passwd(qgetenv("HOME") + "/.unity8-greeter-demo");
    QVERIFY(!passwd.exists());

    SecurityPrivacy panel;
    QVERIFY(passwd.exists());
    QCOMPARE(passwd.permissions(), QFileDevice::ReadOwner |
                                   QFileDevice::WriteOwner |
                                   QFileDevice::ReadUser |
                                   QFileDevice::WriteUser);
}

void PasswordTest::testOldFilePermissions()
{
    QFile passwd(qgetenv("HOME") + "/.unity8-greeter-demo");
    QVERIFY(!passwd.exists());

    passwd.open(QIODevice::ReadWrite);
    passwd.setPermissions(QFileDevice::ReadGroup | QFileDevice::WriteGroup);

    SecurityPrivacy panel;
    QVERIFY(passwd.exists());
    QCOMPARE(passwd.permissions(), QFileDevice::ReadOwner |
                                   QFileDevice::WriteOwner |
                                   QFileDevice::ReadUser |
                                   QFileDevice::WriteUser);
}

void PasswordTest::testFileContents()
{
    SecurityPrivacy panel;
    panel.setSecurityType(SecurityPrivacy::Passphrase);
    panel.setSecurityValue("test");

    QFile passwd(qgetenv("HOME") + "/.unity8-greeter-demo");
    QVERIFY(passwd.open(QIODevice::ReadOnly));
    QRegExp regexp("\\[testuser\\]\npassword=keyboard\npasswordValue=$6$*$*\n", Qt::CaseSensitive, QRegExp::WildcardUnix);
    regexp.setMinimal(true);
    QVERIFY(regexp.exactMatch(passwd.readAll()));
}

void PasswordTest::testMakeSecurityValue()
{
    QCOMPARE(SecurityPrivacy::makeSecurityValue("GsxQABd.", "test"), QString("$6$GsxQABd.$yPRjjnkXwOSj4F53jJLnV6dXs/TodrSVL7BQ0BPfJAZ8Iqskvlpa.NpzJQdG4hEFPZjhKhqEOKt7t/ZDyVv340"));

    QString noSalt = SecurityPrivacy::makeSecurityValue(QString(), "test");
    QStringList noSaltParts = noSalt.split('$');
    QCOMPARE(noSaltParts.length(), 4);
    QCOMPARE(noSaltParts[0], QString(""));
    QCOMPARE(noSaltParts[1], QString("6"));
    QCOMPARE(noSalt, SecurityPrivacy::makeSecurityValue(noSaltParts[2], QString("test")));
}

void PasswordTest::testSecurityValueMatches()
{
    SecurityPrivacy panel;

    panel.setSecurityType(SecurityPrivacy::Swipe);
    QVERIFY(panel.securityValueMatches("invalid"));
    panel.setSecurityValue("swipe");
    QVERIFY(panel.securityValueMatches("invalid2"));

    panel.setSecurityType(SecurityPrivacy::Passcode);
    panel.setSecurityValue("1234");
    QVERIFY(!panel.securityValueMatches("4321"));
    QVERIFY(panel.securityValueMatches("1234"));

    panel.setSecurityType(SecurityPrivacy::Passphrase);
    panel.setSecurityValue("passphrase");
    QVERIFY(!panel.securityValueMatches("invalid"));
    QVERIFY(panel.securityValueMatches("passphrase"));
}

QTEST_MAIN(PasswordTest)
#include "tst_password.moc"
