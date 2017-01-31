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
    // void test_create_mir_display()
    // {
    //     MirDisplayConfiguration conf;

    //     MirDisplayMode mode;
    //     mode.horizontal_resolution = 1280;
    //     mode.vertical_resolution = 1024;
    //     mode.refresh_rate = 60;

    //     MirDisplayMode mode1;
    //     mode1.horizontal_resolution = 640;
    //     mode1.vertical_resolution = 320;
    //     mode1.refresh_rate = 30;

    //     MirDisplayMode modes[2] = { mode, mode1 };
    //     MirDisplayMode modes1[1] = { mode1 };

    //     MirDisplayOutput output;
    //     output.num_modes = 2;
    //     output.modes = modes;
    //     output.current_mode = 1;
    //     output.type = MirDisplayOutputType::mir_display_output_type_edp;
    //     output.connected = true;
    //     output.used = true;
    //     output.orientation = MirOrientation::mir_orientation_left;
    //     output.physical_width_mm = 10;
    //     output.physical_height_mm = 20;
    //     output.power_mode = MirPowerMode::mir_power_mode_on;

    //     MirDisplayOutput output1;
    //     output1.num_modes = 1;
    //     output1.modes = modes1;
    //     output1.current_mode = 0;
    //     output1.type = MirDisplayOutputType::mir_display_output_type_edp;
    //     output1.connected = false;
    //     output1.used = false;
    //     output1.orientation = MirOrientation::mir_orientation_normal;
    //     output1.physical_width_mm = 10;
    //     output1.physical_height_mm = 20;
    //     output1.power_mode = MirPowerMode::mir_power_mode_off;

    //     MirDisplayOutput outputs[2] = { output, output1 };

    //     conf.num_outputs = 2;
    //     conf.outputs = outputs;

    //     for (uint i = 0; i < conf.num_outputs; i++) {
    //         MirDisplayOutput output = conf.outputs[i];
    //         Display display(output);
    //         QCOMPARE(display.mode(), output.current_mode);
    //         QCOMPARE((uint) display.modes().size(), output.num_modes);
    //         QCOMPARE(display.enabled(), (bool) output.used);
    //         QCOMPARE(display.connected(), (bool) output.connected);
    //         QCOMPARE(display.id(), output.output_id);
    //         QCOMPARE(display.physicalWidthMm(), output.physical_width_mm);
    //         QCOMPARE(display.physicalHeightMm(), output.physical_height_mm);
    //         QCOMPARE(display.powerMode(), Helpers::mirPowerModeToPowerMode(output.power_mode));
    //         QCOMPARE(display.mode(), output.current_mode);
    //         QVERIFY(!display.uncommittedChanges());

    //         int j = 0;
    //         Q_FOREACH(DisplayMode mode, display.availableModes()) {
    //             MirDisplayMode origMode = output.modes[j];
    //             QCOMPARE(mode.vertical_resolution, origMode.vertical_resolution);
    //             QCOMPARE(mode.horizontal_resolution, origMode.horizontal_resolution);
    //             QCOMPARE(mode.refresh_rate, origMode.refresh_rate);
    //             j++;
    //         }
    //     }
    // }
    // void test_changes_emit_signal()
    // {
    //     Display display;
    //     QSignalSpy changedSpy(&display, SIGNAL(displayChanged(const Display*)));
    //     display.setMirrored(true);
    //     QTRY_COMPARE(changedSpy.count(), 2);
    // }
    // void test_new_display_no_uncommitted_changes()
    // {
    //     Display display;
    //     QVERIFY(!display.uncommittedChanges());
    // }
    // void test_changed_display_has_uncommitted_changes()
    // {
    //     Display display;
    //     display.setMirrored(true);
    //     QVERIFY(display.uncommittedChanges());
    // }
private:
    FakeOutput* getFakeOutput(const QSharedPointer<Output> &output)
    {
        return (FakeOutput*) output.data();
    }
};

QTEST_GUILESS_MAIN(TstDisplay)
#include "tst_displayplugin_display.moc"
