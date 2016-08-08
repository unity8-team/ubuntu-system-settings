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

/*
 * This test uses a mock web server.
 */

#include "click/manifest_impl.h"

#include <QSignalSpy>
#include <QTest>
#include <QUrl>

#include <QDebug>

using namespace UpdatePlugin;

Q_DECLARE_METATYPE(QProcess::ProcessError)

class TstClickManifest
    : public QObject
{
    Q_OBJECT
private slots:
    void init()
    {
        m_instance = new Click::ManifestImpl();
    }
    void cleanup()
    {
        QSignalSpy destroyedSpy(m_instance, SIGNAL(destroyed(QObject*)));
        m_instance->deleteLater();
        QTRY_COMPARE(destroyedSpy.count(), 1);
    }
    void testRequestSucceeded()
    {
        QSignalSpy requestSucceededSpy(m_instance, SIGNAL(requestSucceeded(const QJsonArray&)));
        m_instance->request();
        QVERIFY(requestSucceededSpy.wait());
        QList<QVariant> args = requestSucceededSpy.takeFirst();
        QJsonArray res = args.at(0).toJsonArray();
        QCOMPARE(res.size(), 4); // See click.result
    }
    void testFailedProcess_data()
    {
        QTest::addColumn<QProcess::ProcessError>("error");
        QTest::newRow("Failed to start") << QProcess::FailedToStart;
        QTest::newRow("Crashed") << QProcess::Crashed;
        QTest::newRow("Timed out") << QProcess::Timedout;
        QTest::newRow("Write error") << QProcess::WriteError;
        QTest::newRow("Read error") << QProcess::ReadError;
        QTest::newRow("Unknown error") << QProcess::UnknownError;
    }
    void testFailedProcess()
    {
        QFETCH(QProcess::ProcessError, error);
        connect(
            this, SIGNAL(mockProcessError(const QProcess::ProcessError&)),
            m_instance, SLOT(handleProcessError(const QProcess::ProcessError&))
        );
        QSignalSpy requestFailedSpy(m_instance, SIGNAL(requestFailed()));
        Q_EMIT mockProcessError(error);
        QTRY_COMPARE(requestFailedSpy.count(), 1);
    }
    void testFailedParsing()
    {
        /* The process will not be open for reading, so the parsing fails. */
        connect(
            this, SIGNAL(mockProcessSuccess(const int&)),
            m_instance, SLOT(handleProcessSuccess(const int&))
        );
        QSignalSpy requestFailedSpy(m_instance, SIGNAL(requestFailed()));
        Q_EMIT mockProcessSuccess(0);
        QTRY_COMPARE(requestFailedSpy.count(), 1);
    }
Q_SIGNALS:
    void mockProcessSuccess(const int&);
    void mockProcessError(const QProcess::ProcessError&);
private:
    Click::ManifestImpl *m_instance = nullptr;
};

QTEST_GUILESS_MAIN(TstClickManifest)
#include "tst_clickmanifest.moc"

