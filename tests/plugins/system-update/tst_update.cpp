/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@canonical.com>
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
#include <QtQml>
#include <QtQml/QQmlContext>
#include <QObject>
#include <QTest>
#include <QSignalSpy>
#include <QString>
#include <QScopedPointer>

#include "update.h"

using namespace UpdatePlugin;

//Q_DECLARE_TYPE(Update)

// simple class that makes the protected members public
class PublicUpdate : public Update {
public:
    explicit PublicUpdate(QObject* parent=0) : Update(parent) {}

    using Update::m_binary_filesize;
    using Update::m_click_url;
    using Update::m_clickToken;
    using Update::m_downloadUrl;
    using Update::m_download_progress;
    using Update::m_error;
    using Update::m_icon_url;
    using Update::m_lastUpdateDate;
    using Update::m_local_version;
    using Update::m_packagename;
    using Update::m_remote_version;
    using Update::m_selected;
    using Update::m_systemUpdate;
    using Update::m_title;
    using Update::m_update;
    using Update::m_update_ready;
    using Update::m_update_state;

    using Update::getIgnoreUpdates;
};

class UpdateTest: public QObject
{
    Q_OBJECT

public:
    UpdateTest() {};

private Q_SLOTS:
    void testCompareVersion();

    void testSetSystemUpdate_data();
    void testSetSystemUpdate();
    void testSetSystemUpdateEqual();

    void testSetUpdateRequired_data();
    void testSetUpdateRequired();
    void testSetUpdateRequiredEqual();

    void testSetUpdateState_data();
    void testSetUpdateState();
    void testSetUpdateStateEqual();

    void testSetUpdateReady_data();
    void testSetUpdateReady();

    void testSetSelected_data();
    void testSetSelected();
    void testSetSelectedEqual();

    void testSetBinaryFilesize_data();
    void testSetBinaryFilesize();
    void testSetBinaryFilesizeEqual();

    void testSetDownloadProgress_data();
    void testSetDownloadProgress();
    void testSetDownloadProgressEqual();

    void testSetIconUrl_data();
    void testSetIconUrl();
    void testSetIconUrlEqual();

    void testSetError_data();
    void testSetError();
    void testSetErrorEqual();

    void testSetUpdateAvailable_data();
    void testSetUpdateAvailable();

    void testSetLastUpdateDate_data();
    void testSetLastUpdateDate();
    void testSetLastUpdateDateEqual();

    void testSetClickUrl_data();
    void testSetClickUrl();

    void testSetDownloadUrl_data();
    void testSetDownloadUrl();
    void testSetDownloadUrlEqual();

    void testSetClickToken_data();
    void testSetClickToken();
    void testSetClickTokenEqual();
};

void UpdateTest::testCompareVersion()
{
    Update app;
    app.initializeApplication("package.name", "title", "1.1");
    QCOMPARE(app.updateRequired(), false);
    QString version("1.4");
    app.setRemoteVersion(version);
    QCOMPARE(app.updateRequired(), true);
}

void UpdateTest::testSetSystemUpdate_data()
{
    QTest::addColumn<bool>("previousValue");
    QTest::addColumn<bool>("isSystem");

    QTest::newRow("True Value") << false << true;
    QTest::newRow("False Value") << true << false;
}

void UpdateTest::testSetSystemUpdate()
{
    QFETCH(bool, previousValue);
    QFETCH(bool, isSystem);

    QScopedPointer<PublicUpdate> update(new PublicUpdate());
    update->m_systemUpdate = previousValue;

    QSignalSpy spy(update.data(), SIGNAL(systemUpdateChanged()));
    update->setSystemUpdate(isSystem);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(update->m_systemUpdate, isSystem);
}

void UpdateTest::testSetSystemUpdateEqual()
{
    QScopedPointer<PublicUpdate> update(new PublicUpdate());
    update->m_systemUpdate = true;

    QSignalSpy spy(update.data(), SIGNAL(systemUpdateChanged()));
    update->setSystemUpdate(update->m_systemUpdate);

    QCOMPARE(spy.count(), 0);
}

void UpdateTest::testSetUpdateRequired_data()
{
    QTest::addColumn<bool>("previousValue");
    QTest::addColumn<bool>("state");

    QTest::newRow("True Value") << false << true;
    QTest::newRow("False Value") << true << false;
}

void UpdateTest::testSetUpdateRequired()
{
    QFETCH(bool, previousValue);
    QFETCH(bool, state);

    QScopedPointer<PublicUpdate> update(new PublicUpdate());
    update->m_update = previousValue;

    QSignalSpy spy(update.data(), SIGNAL(updateRequiredChanged()));
    update->setUpdateRequired(state);

    QCOMPARE(update->m_update, state);
    QCOMPARE(spy.count(), 1);
}

void UpdateTest::testSetUpdateRequiredEqual()
{
    bool state = true;
    QScopedPointer<PublicUpdate> update(new PublicUpdate());
    update->m_update = state;

    QSignalSpy spy(update.data(), SIGNAL(updateRequiredChanged()));
    update->setUpdateRequired(update->m_update);

    QCOMPARE(spy.count(), 0);
}

void UpdateTest::testSetUpdateState_data()
{
    QTest::addColumn<bool>("previousValue");
    QTest::addColumn<bool>("state");

    QTest::newRow("True Value") << false << true;
    QTest::newRow("False Value") << true << false;
}

void UpdateTest::testSetUpdateState()
{
    QFETCH(bool, previousValue);
    QFETCH(bool, state);

    QScopedPointer<PublicUpdate> update(new PublicUpdate());

    update->m_update_state = previousValue;
    update->setUpdateState(state);

    QCOMPARE(update->m_update_state, state);
}

void UpdateTest::testSetUpdateStateEqual()
{
    bool state = true;
    QScopedPointer<PublicUpdate> update(new PublicUpdate());
    update->m_update_state = state;

    QSignalSpy spy(update.data(), SIGNAL(updateStateChanged()));
    update->setUpdateState(update->m_update_state);

    QCOMPARE(spy.count(), 0);
}

void UpdateTest::testSetUpdateReady_data()
{
    QTest::addColumn<bool>("previousValue");
    QTest::addColumn<bool>("ready");

    QTest::newRow("True Value") << false << true;
    QTest::newRow("False Value") << true << false;
}

void UpdateTest::testSetUpdateReady()
{
    QFETCH(bool, previousValue);
    QFETCH(bool, ready);

    QScopedPointer<PublicUpdate> update(new PublicUpdate());

    update->m_update = previousValue;
    update->setUpdateAvailable(ready);

    QCOMPARE(update->m_update, ready);
}

void UpdateTest::testSetSelected_data()
{
    QTest::addColumn<bool>("previousValue");
    QTest::addColumn<bool>("selected");

    QTest::newRow("True Value") << false << true;
    QTest::newRow("False Value") << true << false;
}

void UpdateTest::testSetSelected()
{
    QFETCH(bool, previousValue);
    QFETCH(bool, selected);

    QScopedPointer<PublicUpdate> update(new PublicUpdate());

    QSignalSpy spy(update.data(), SIGNAL(selectedChanged()));
    update->m_selected = previousValue;
    update->setSelected(selected);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(update->m_selected, selected);
}

void UpdateTest::testSetSelectedEqual()
{
    bool selected = true;
    QScopedPointer<PublicUpdate> update(new PublicUpdate());
    update->m_selected = selected;

    QSignalSpy spy(update.data(), SIGNAL(selectedChanged()));
    update->setSelected(update->m_selected);

    QCOMPARE(spy.count(), 0);
}

void UpdateTest::testSetBinaryFilesize_data()
{
    QTest::addColumn<int>("size");

    QTest::newRow("first-obj") << 8090;
    QTest::newRow("second-obj") << 2012;
    QTest::newRow("third-obj") << 25111;
    QTest::newRow("last-obj") << 9222;
}

void UpdateTest::testSetBinaryFilesize()
{
    QFETCH(int, size);

    QScopedPointer<PublicUpdate> update(new PublicUpdate());

    QSignalSpy spy(update.data(), SIGNAL(binaryFilesizeChanged()));
    update->setBinaryFilesize(size);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(update->m_binary_filesize, size);
}

void UpdateTest::testSetBinaryFilesizeEqual()
{
    int size = 2390;
    QScopedPointer<PublicUpdate> update(new PublicUpdate());
    update->m_binary_filesize = size;

    QSignalSpy spy(update.data(), SIGNAL(binaryFilesizeChanged()));
    update->setBinaryFilesize(update->m_binary_filesize);

    QCOMPARE(spy.count(), 0);
}

void UpdateTest::testSetDownloadProgress_data()
{
    QTest::addColumn<int>("progress");

    QTest::newRow("first-obj") << 80;
    QTest::newRow("second-obj") << 20;
    QTest::newRow("third-obj") << 25;
    QTest::newRow("last-obj") << 90;
}

void UpdateTest::testSetDownloadProgress()
{
    QFETCH(int, progress);

    QScopedPointer<PublicUpdate> update(new PublicUpdate());

    QSignalSpy spy(update.data(), SIGNAL(downloadProgressChanged()));
    update->setDownloadProgress(progress);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(update->m_download_progress, progress);
}

void UpdateTest::testSetDownloadProgressEqual()
{
    int progress = 90;
    QScopedPointer<PublicUpdate> update(new PublicUpdate());
    update->m_download_progress = progress;

    QSignalSpy spy(update.data(), SIGNAL(downloadProgressChanged()));
    update->setDownloadProgress(update->m_download_progress);

    QCOMPARE(spy.count(), 0);
}

void UpdateTest::testSetIconUrl_data()
{
    QTest::addColumn<QString>("icon");

    QTest::newRow("first-obj") << "www.test.com/first-obj";
    QTest::newRow("second-obj") << "www.test.com/second-obj";
    QTest::newRow("third-obj") << "www.test.com/third-obj";
    QTest::newRow("last-obj") << "www.test.com/last-obj";
}

void UpdateTest::testSetIconUrl()
{
    QFETCH(QString, icon);

    QScopedPointer<PublicUpdate> update(new PublicUpdate());

    QSignalSpy spy(update.data(), SIGNAL(iconUrlChanged()));
    update->setIconUrl(icon);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(update->m_icon_url, icon);
}

void UpdateTest::testSetIconUrlEqual()
{
    auto icon = "www.test.com/icon";
    QScopedPointer<PublicUpdate> update(new PublicUpdate());
    update->m_icon_url = icon;

    QSignalSpy spy(update.data(), SIGNAL(iconUrlChanged()));
    update->setIconUrl(update->m_icon_url);

    QCOMPARE(spy.count(), 0);
}

void UpdateTest::testSetError_data()
{
    QTest::addColumn<QString>("error");

    QTest::newRow("first-obj") << "101";
    QTest::newRow("second-obj") << "404";
    QTest::newRow("third-obj") << "Third error";
    QTest::newRow("last-obj") << "Last click could not be updated.";
}

void UpdateTest::testSetError()
{
    QFETCH(QString, error);

    QScopedPointer<PublicUpdate> update(new PublicUpdate());

    QSignalSpy spy(update.data(), SIGNAL(errorChanged()));
    update->setError(error);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(update->m_error, error);
}

void UpdateTest::testSetErrorEqual()
{
    auto error = "Same error";
    QScopedPointer<PublicUpdate> update(new PublicUpdate());
    update->m_error = error;

    QSignalSpy spy(update.data(), SIGNAL(errorChanged()));
    update->setError(update->m_error);

    QCOMPARE(spy.count(), 0);
}

void UpdateTest::testSetUpdateAvailable_data()
{
    QTest::addColumn<bool>("previousValue");
    QTest::addColumn<bool>("available");

    QTest::newRow("True Value") << false << true;
    QTest::newRow("False Value") << true << false;
}

void UpdateTest::testSetUpdateAvailable()
{
    QFETCH(bool, previousValue);
    QFETCH(bool, available);

    QScopedPointer<PublicUpdate> update(new PublicUpdate());
    update->m_update = previousValue;

    update->setUpdateAvailable(available);

    QCOMPARE(update->m_update, available);
}

void UpdateTest::testSetLastUpdateDate_data()
{
    QTest::addColumn<QString>("date");

    QTest::newRow("first-obj") << "2014-04-12";
    QTest::newRow("second-obj") << "1999-08-20";
    QTest::newRow("third-obj") << "1983-09-30";
    QTest::newRow("last-obj") << "1983-08-03";
}

void UpdateTest::testSetLastUpdateDate()
{
    QFETCH(QString, date);

    QScopedPointer<PublicUpdate> update(new PublicUpdate());

    QSignalSpy spy(update.data(), SIGNAL(lastUpdateDateChanged()));
    update->setLastUpdateDate(date);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(update->m_lastUpdateDate, date);
}

void UpdateTest::testSetLastUpdateDateEqual()
{
    auto date = "2014-04-12";
    QScopedPointer<PublicUpdate> update(new PublicUpdate());
    update->m_lastUpdateDate = date;

    QSignalSpy spy(update.data(), SIGNAL(lastUpdateDateChanged()));
    update->setLastUpdateDate(update->m_lastUpdateDate);

    QCOMPARE(spy.count(), 0);
}

void UpdateTest::testSetClickUrl_data()
{
    QTest::addColumn<QString>("url");

    QTest::newRow("first-obj") << "www.test.com/first-obj";
    QTest::newRow("second-obj") << "www.test.com/second-obj";
    QTest::newRow("third-obj") << "www.test.com/third-obj";
    QTest::newRow("last-obj") << "www.test.com/last-obj";
}

void UpdateTest::testSetClickUrl()
{
    QFETCH(QString, url);

    QScopedPointer<PublicUpdate> update(new PublicUpdate());

    update->setClickUrl(url);

    QCOMPARE(update->m_click_url, url);
}

void UpdateTest::testSetDownloadUrl_data()
{
    QTest::addColumn<QString>("url");

    QTest::newRow("first-obj") << "www.test.com/first-obj";
    QTest::newRow("second-obj") << "www.test.com/second-obj";
    QTest::newRow("third-obj") << "www.test.com/third-obj";
    QTest::newRow("last-obj") << "www.test.com/last-obj";
}

void UpdateTest::testSetDownloadUrl()
{
    QFETCH(QString, url);

    QScopedPointer<PublicUpdate> update(new PublicUpdate());

    QSignalSpy spy(update.data(), SIGNAL(downloadUrlChanged()));
    update->setDownloadUrl(url);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(update->m_downloadUrl, url);
}

void UpdateTest::testSetDownloadUrlEqual()
{
    QScopedPointer<PublicUpdate> update(new PublicUpdate());
    update->m_downloadUrl = "www.test.com";

    QSignalSpy spy(update.data(), SIGNAL(downloadUrlChanged()));
    update->setDownloadUrl(update->m_downloadUrl);

    QCOMPARE(spy.count(), 0);
}

void UpdateTest::testSetClickToken_data()
{
    QTest::addColumn<QString>("token");

    QTest::newRow("first-obj") << "first token";
    QTest::newRow("second-obj") << "second token";
    QTest::newRow("third-obj") << "third token";
    QTest::newRow("last-obj") << "last token";
}

void UpdateTest::testSetClickToken()
{
    QFETCH(QString, token);

    QScopedPointer<PublicUpdate> update(new PublicUpdate());

    QSignalSpy spy(update.data(), SIGNAL(clickTokenChanged()));
    update->setClickToken(token);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(update->m_clickToken, token);
}

void UpdateTest::testSetClickTokenEqual()
{
    QScopedPointer<PublicUpdate> update(new PublicUpdate());
    update->m_clickToken = "token";

    QSignalSpy spy(update.data(), SIGNAL(clickTokenChanged()));
    update->setClickToken(update->m_clickToken);

    QCOMPARE(spy.count(), 0);
}

QTEST_MAIN(UpdateTest)
#include "tst_update.moc"
