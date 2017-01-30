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

#include "brightness.h"
#include "fakemirclient.h"
#include "fakeoutput.h"
#include "fakeoutputmode.h"

#include <QSignalSpy>
#include <QTest>

using namespace DisplayPlugin;

class TstBrightness : public QObject
{
    Q_OBJECT
private slots:
    void init()
    {
        // m_mode1.vertical_resolution = 1280;
        // m_mode1.horizontal_resolution = 1024;
        // m_mode1.refresh_rate = 60;

        // m_mode2.vertical_resolution = 640;
        // m_mode2.horizontal_resolution = 320;
        // m_mode2.refresh_rate = 30;

        // m_modes[0] = m_mode1;
        // m_modes[1] = m_mode2;

        // m_output.modes = m_modes;
        // m_output.num_modes = 2;
        // m_output.current_mode = 0;
        // m_output.output_id = 0;
        // m_output.type = MirDisplayOutputType::mir_display_output_type_edp;
        // m_output.connected = true;
        // m_output.used = true;
        // m_output.orientation = MirOrientation::mir_orientation_left;

        // m_outputs[0] = m_output;
        // m_conf.num_outputs = 1;
        // m_conf.outputs = m_outputs;

        // m_mirClient = new FakeMirClient();
        // m_mirClient->connected = true;

        // m_mirClient->conf = &m_conf;
        // m_instance = new Brightness(QDBusConnection::systemBus(),
        //                             m_mirClient);
    }
    void cleanup()
    {
        // QSignalSpy destroyedSpy(m_instance, SIGNAL(destroyed(QObject*)));
        // m_instance->deleteLater();
        // QTRY_COMPARE(destroyedSpy.count(), 1);
    }
    void test_null_config_when_connected()
    {
        FakeMirClient mc;
        mc.connected = true;
        mc.conf = nullptr;
        Brightness b(QDBusConnection::systemBus(), &mc);
        QCOMPARE(b.allDisplays()->rowCount(), 0);
    }
    void test_not_connected_to_mir()
    {
        FakeMirClient mc;
        mc.connected = false;
        Brightness b(QDBusConnection::systemBus(), &mc);
        QCOMPARE(b.allDisplays()->rowCount(), 0);
    }
    void test_some_config_populates_model()
    {
        FakeMirClient mc;
        mc.connected = true;

        QList<QSharedPointer<Output>> list;
        QSharedPointer<Output> output(new FakeOutput);
        list << output;

        mc.m_outputs = list;
        // MirDisplayConfig conf;
        // conf.num_outputs = 1;

        // MirDisplayOutput output;
        // output.num_modes = 0;

        // MirDisplayOutput outputs[1];
        // conf.outputs = outputs;
        // conf.outputs[0] = output;
        // mc.conf = &conf;

        Brightness b(QDBusConnection::systemBus(), &mc);
        QCOMPARE(b.allDisplays()->rowCount(), 1);
    }
    // void test_a_display_was_configured()
    // {
    //     auto model = (DisplayModel*) m_instance->allDisplays();
    //     auto display = model->getById(0);

    //     QCOMPARE(display->type(), QString("edp"));
    // }
    // void test_apply_config_to_display()
    // {
    //     auto model = (DisplayModel*) m_instance->allDisplays();
    //     auto display = model->getById(0);
    //     display->setMode(1);
    //     display->setEnabled(false);
    //     display->setOrientation(Display::Orientation::NormalOrientation);

    //     m_instance->applyDisplayConfiguration();

    //     MirDisplayOutput output = m_mirClient->conf->outputs[0];
    //     QCOMPARE(output.current_mode, (uint) 1);
    //     QCOMPARE((bool) output.used, false);
    //     QCOMPARE(output.orientation, MirOrientation::mir_orientation_normal);
    // }

private:
    FakeMirClient *m_mirClient = nullptr;
    Brightness *m_instance = nullptr;

    // MirDisplayConfig m_conf;
    // MirDisplayOutput m_output;
    // MirDisplayOutput m_outputs[1];
    // MirDisplayMode m_mode1;
    // MirDisplayMode m_mode2;
    // MirDisplayMode m_modes[2];
};

QTEST_GUILESS_MAIN(TstBrightness)
#include "tst_brightness.moc"
