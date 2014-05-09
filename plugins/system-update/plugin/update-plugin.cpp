/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
 *
 * Contact: Diego Sarmentero <diego.sarmentero@canonical.com>
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

#include "update-plugin.h"

#include <QDebug>
#include <QStringList>
#include <SystemSettings/ItemBase>

#include "../update_manager.h"

using namespace SystemSettings;
using namespace UpdatePlugin;

class UpdateItem: public ItemBase
{
    Q_OBJECT

public:
    UpdateItem(const QVariantMap &staticData, QObject *parent = 0);
    void setVisibility(bool visible);
    ~UpdateItem();

private Q_SLOTS:
    void changeVisibility(bool downloading);

private:
    UpdateManager m_updateManager;
};

UpdateItem::UpdateItem(const QVariantMap &staticData, QObject *parent):
    ItemBase(staticData, parent),
    m_updateManager(this)
{
    setVisibility(false);
    QObject::connect(&m_updateManager, SIGNAL(updateAvailableFound(bool downloading)),
                  this, SLOT(changeVisibility(bool downloading)));

    m_updateManager.checkUpdates();
}

void UpdateItem::changeVisibility(bool)
{
    setVisibility(true);
}

void UpdateItem::setVisibility(bool visible)
{
    setVisible(visible);
}

UpdateItem::~UpdateItem()
{
}

CheckUpdatesPlugin::CheckUpdatesPlugin():
    QObject()
{
}


ItemBase *CheckUpdatesPlugin::createItem(const QVariantMap &staticData,
                                 QObject *parent)
{
    return new UpdateItem(staticData, parent);
}

#include "update-plugin.moc"
