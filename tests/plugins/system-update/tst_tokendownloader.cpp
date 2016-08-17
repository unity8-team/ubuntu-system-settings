/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
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

#include "click/tokendownloader.h"
#include "click/tokendownloader_impl.h"
#include "update.h"

#include "plugins/system-update/fakeapiclient.h"

#include <QSharedPointer>
#include <QSignalSpy>
#include <QTest>

using namespace UpdatePlugin;

class TstClickTokenDownloader
    : public QObject
{
    Q_OBJECT
private slots:
    void init()
    {
        m_mockclient = new MockApiClient;
        m_update = QSharedPointer<Update>(new Update);
        m_instance = new Click::TokenDownloaderImpl(m_mockclient, m_update);

        m_mockclient->setParent(m_instance);
    }
    void cleanup()
    {
        m_update.clear();
        QSignalSpy destroyedSpy(m_instance, SIGNAL(destroyed(QObject*)));
        m_instance->deleteLater();
        QTRY_COMPARE(destroyedSpy.count(), 1);
    }
    void testClientNetworkError()
    {
        QSignalSpy downloadFailedSpy(m_instance, SIGNAL(downloadFailed(QSharedPointer<Update>)));
        m_mockclient->mockNetworkError();
        QTRY_COMPARE(downloadFailedSpy.count(), 1);
        QList<QVariant> args = downloadFailedSpy.takeFirst();
        QCOMPARE(args.at(0).value<QSharedPointer<Update> >(), m_update);

    }
    void testClientServerError()
    {
        QSignalSpy downloadFailedSpy(m_instance, SIGNAL(downloadFailed(QSharedPointer<Update>)));
        m_mockclient->mockServerError();
        QTRY_COMPARE(downloadFailedSpy.count(), 1);
        QList<QVariant> args = downloadFailedSpy.takeFirst();
        QCOMPARE(args.at(0).value<QSharedPointer<Update> >(), m_update);

    }
    void testClientCredentialError()
    {
        QSignalSpy downloadFailedSpy(m_instance, SIGNAL(downloadFailed(QSharedPointer<Update>)));
        m_mockclient->mockCredentialError();
        QTRY_COMPARE(downloadFailedSpy.count(), 1);
        QList<QVariant> args = downloadFailedSpy.takeFirst();
        QCOMPARE(args.at(0).value<QSharedPointer<Update> >(), m_update);
    }
    void testEmptyToken()
    {
        QSignalSpy downloadFailedSpy(m_instance, SIGNAL(downloadFailed(QSharedPointer<Update>)));
        m_mockclient->mockTokenRequestSucceeded("");
        QTRY_COMPARE(downloadFailedSpy.count(), 1);
        QList<QVariant> args = downloadFailedSpy.takeFirst();
        QCOMPARE(args.at(0).value<QSharedPointer<Update> >(), m_update);
    }
    void testGoodToken()
    {
        QSignalSpy downloadSucceededSpy(m_instance, SIGNAL(downloadSucceeded(QSharedPointer<Update>)));
        m_mockclient->mockTokenRequestSucceeded("good token");
        QTRY_COMPARE(downloadSucceededSpy.count(), 1);
        QList<QVariant> args = downloadSucceededSpy.takeFirst();
        QCOMPARE(args.at(0).value<QSharedPointer<Update> >(), m_update);
    }
    void testDownload()
    {
        QString targetUrl("http://example.org?signedUrl");
        m_update->setDownloadUrl(targetUrl);
        m_instance->download("signedUrl");
        QCOMPARE(m_mockclient->requestedUrl.toString(), targetUrl);
    }
private:
    MockApiClient *m_mockclient = nullptr;
    Click::TokenDownloaderImpl *m_instance = nullptr;
    QSharedPointer<Update> m_update;
};

QTEST_GUILESS_MAIN(TstClickTokenDownloader)
#include "tst_tokendownloader.moc"
