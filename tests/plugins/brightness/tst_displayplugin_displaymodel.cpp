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

#include "display.h"
#include "displaymodel.h"

#include <QSignalSpy>
#include <QTest>

Q_DECLARE_METATYPE(DisplayPlugin::DisplayModel::Roles)

using namespace DisplayPlugin;

class TstDisplayModel : public QObject
{
    Q_OBJECT
private:
    QSharedPointer<Display> createDisplay(const uint &id = 1)
    {
        auto display = QSharedPointer<Display>(new Display(id));
        return display;
    }
private slots:
    void init()
    {
        m_instance = new DisplayModel();
    }
    void cleanup()
    {
        QSignalSpy destroyedSpy(m_instance, SIGNAL(destroyed(QObject*)));
        m_instance->deleteLater();
        QTRY_COMPARE(destroyedSpy.count(), 1);
    }
    // void test_roles_data()
    // {
    //     QTest::addColumn<DisplayModel::Roles>("role");
    //     QTest::addColumn<QVariant>("value");
    //     QTest::addColumn<QSharedPointer<Display>>("display");

    //     {
    //         auto display = createDisplay();
    //         DisplayModel::Roles role(DisplayModel::MirroredRole);
    //         QVariant value(false);
    //         display->setMirrored(false);
    //         QTest::newRow("MirroredRole") << role << value << display;
    //     }
    //     {
    //         auto display = createDisplay();
    //         DisplayModel::Roles role(DisplayModel::ConnectedRole);
    //         QVariant value(false);
    //         QTest::newRow("ConnectedRole") << role << value << display;
    //     }
    //     {
    //         auto display = createDisplay();
    //         DisplayModel::Roles role(DisplayModel::EnabledRole);
    //         QVariant value(true);
    //         display->setEnabled(true);
    //         QTest::newRow("EnabledRole") << role << value << display;
    //     }
    //     {
    //         auto display = createDisplay();
    //         DisplayModel::Roles role(DisplayModel::ModeRole);
    //         QVariant value((uint) 0);
    //         display->setMode(0);
    //         QTest::newRow("ModeRole") << role << value << display;
    //     }
    //     {
    //         auto display = createDisplay();
    //         DisplayModel::Roles role(DisplayModel::OrientationRole);
    //         QVariant value((uint) Display::Orientation::PortraitInvertedModeOrientation);
    //         display->setOrientation(Display::Orientation::PortraitInvertedModeOrientation);
    //         QTest::newRow("OrientationRole") << role << value << display;
    //     }
    //     {
    //         auto display = createDisplay();
    //         DisplayModel::Roles role(DisplayModel::ScaleRole);
    //         QVariant value(2);
    //         display->setScale(2);
    //         QTest::newRow("ScaleRole") << role << value << display;
    //     }
    //     {
    //         auto display = createDisplay();
    //         DisplayModel::Roles role(DisplayModel::UncommittedChangesRole);
    //         QVariant value(true);
    //         display->setMirrored(true); // this will make the target value true.
    //         QTest::newRow("UncommittedChangesRole") << role << value << display;
    //     }
    // }
    // void test_roles()
    // {
    //     QFETCH(DisplayModel::Roles, role);
    //     QFETCH(QVariant, value);
    //     QFETCH(QSharedPointer<Display>, display);

    //     m_instance->addDisplay(display);
    //     QCOMPARE(m_instance->data(m_instance->index(0), role), value);
    // }
    // void test_add_display()
    // {
    //     auto display = createDisplay();

    //     QSignalSpy insertedSpy(m_instance, SIGNAL(rowsAboutToBeInserted(const QModelIndex&, int, int)));
    //     m_instance->addDisplay(display);
    //     QTRY_COMPARE(insertedSpy.count(), 1);
    //     QCOMPARE(m_instance->rowCount(), 1);
    // }
    // void test_update_display()
    // {
    //     auto display = createDisplay();
    //     display->setMirrored(false);
    //     m_instance->addDisplay(display);

    //     QSignalSpy dataChangedSpy(m_instance, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)));
    //     display->setMirrored(false);
    //     m_instance->addDisplay(display);
    //     QTRY_COMPARE(dataChangedSpy.count(), 1);
    //     QCOMPARE(m_instance->rowCount(), 1);
    // }
private:
    DisplayModel *m_instance;
};

QTEST_GUILESS_MAIN(TstDisplayModel)
#include "tst_displayplugin_displaymodel.moc"

