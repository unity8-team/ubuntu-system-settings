/*
 * This file is part of system-settings
 *
 * Copyright (C) 2015 Canonical Ltd.
 *
 * Contact: Ken VanDine <ken.vandine@canonical.com>
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

#include "phone-plugin.h"

#include <QDebug>
#include <SystemSettings/ItemBase>
#include <qofonomanager.h>

using namespace SystemSettings;

class PhoneItem: public ItemBase
{
    Q_OBJECT

public:
    explicit PhoneItem(const QVariantMap &staticData, QObject *parent = 0);
    void setVisibility(bool visible);

private Q_SLOTS:
    void shouldShow(QStringList);
};


PhoneItem::PhoneItem(const QVariantMap &staticData, QObject *parent):
    ItemBase(staticData, parent)
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    if (env.contains(QLatin1String("USS_SHOW_ALL_UI"))) {
        QString showAllS = env.value("USS_SHOW_ALL_UI", QString());

        if(!showAllS.isEmpty()) {
            setVisibility(true);
            return;
        }
    }

    QOfonoManager *mm = new QOfonoManager(this);
    // Hide the plugin if there are no modems present
    setVisibility(mm->modems().length() > 0);
    QObject::connect(mm, SIGNAL(modemsChanged(QStringList)),
                     this, SLOT(shouldShow(QStringList)));

}

void PhoneItem::shouldShow(QStringList modems)
{
    setVisibility(modems.length() > 0);
}

void PhoneItem::setVisibility(bool visible)
{
    setVisible(visible);
}

ItemBase *PhonePlugin::createItem(const QVariantMap &staticData,
                                 QObject *parent)
{
    return new PhoneItem(staticData, parent);
}

#include "phone-plugin.moc"
