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

#include "fakeoutput.h"
#include "fakeoutputmode.h"

#include "display.h"
#include "helpers.h"

#include <QSharedPointer>
#include <QSignalSpy>
#include <QTest>

Q_DECLARE_METATYPE(QSharedPointer<DisplayPlugin::Output>)

using namespace DisplayPlugin;

class TstDisplay : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase()
    {
        qRegisterMetaType<const Display*>("const Display*");
    }
    void test_instantiation_data()
    {
        QTest::addColumn<QSharedPointer<Output>>("output");

        {
            auto output = QSharedPointer<Output>(new FakeOutput);
            QTest::newRow("defaults") << output;
        }
        {
            auto output = QSharedPointer<Output>(new FakeOutput);
            getFakeOutput(output)->m_enabled = true;
            getFakeOutput(output)->m_type = Enums::OutputType::OutputTypeEdp;
            getFakeOutput(output)->m_connectionState = Enums::ConnectionState::Connected;
            getFakeOutput(output)->m_powerMode = Enums::PowerMode::SuspendMode;
            getFakeOutput(output)->m_orientation = Enums::Orientation::LandscapeInvertedModeOrientation;
            getFakeOutput(output)->m_id = 42;
            QTest::newRow("example screen") << output;
        }
        {
            auto output = QSharedPointer<Output>(new FakeOutput);

            auto mode1 = QSharedPointer<OutputMode>(new FakeOutputMode);
            auto mode2 = QSharedPointer<OutputMode>(new FakeOutputMode);
            QList<QSharedPointer<OutputMode>> modes({mode1, mode2});

            getFakeOutput(output)->m_modes = modes;
            QTest::newRow("with modes") << output;
        }
    }
    void test_instantiation()
    {
        QFETCH(QSharedPointer<Output>, output);
        Display display(output);
        QCOMPARE(display.enabled(), output->isEnabled());
        QCOMPARE(display.powerMode(), output->getPowerMode());
        QCOMPARE(display.orientation(), output->getOrientation());
        // Test Connected, etc.

        QCOMPARE(display.mode(), output->getCurrentMode());
        QCOMPARE(display.availableModes(), output->getAvailableModes());
    }
    void test_modes()
    {
        auto output = QSharedPointer<Output>(new FakeOutput);
        Display display(output);
    }
    void test_changes_emit_signal()
    {
        auto output = QSharedPointer<Output>(new FakeOutput);
        getFakeOutput(output)->m_enabled = false;

        Display display(output);
        QSignalSpy changedSpy(&display, SIGNAL(displayChanged(const Display*)));
        display.setEnabled(true);
        QTRY_COMPARE(changedSpy.count(), 2);
    }
    void test_new_display_no_uncommitted_changes()
    {
        auto output = QSharedPointer<Output>(new FakeOutput);
        Display display(output);
        QVERIFY(!display.uncommittedChanges());
    }
    void test_changed_display_has_uncommitted_changes()
    {
        auto output = QSharedPointer<Output>(new FakeOutput);
        getFakeOutput(output)->m_enabled = false;

        Display display(output);
        display.setEnabled(true);
        QVERIFY(display.uncommittedChanges());
    }
private:
    FakeOutput* getFakeOutput(const QSharedPointer<Output> &output)
    {
        return (FakeOutput*) output.data();
    }
};

QTEST_GUILESS_MAIN(TstDisplay)
#include "tst_displayplugin_display.moc"
