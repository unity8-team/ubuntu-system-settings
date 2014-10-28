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
#include <QTimer>
#include <SystemSettings/ItemBase>

#include "../update_manager.h"

using namespace SystemSettings;
using namespace UpdatePlugin;

class UpdateItem: public ItemBase
{
    Q_OBJECT

public:
    explicit UpdateItem(const QVariantMap &staticData, QObject *parent = 0);
    Q_INVOKABLE void setVisibility(bool visible);
    ~UpdateItem();

private Q_SLOTS:
    void onUpdateAvailableFound(bool);
    void onModelChanged();
    void shouldShow();

private:
    UpdateManager *m_updateManager;
};

UpdateItem::UpdateItem(const QVariantMap &staticData, QObject *parent):
    ItemBase(staticData, parent)
{
    setVisibility(false);
    m_updateManager = UpdateManager::instance();    
    QObject::connect(m_updateManager, SIGNAL(updateAvailableFound(bool)),
                  this, SLOT(onUpdateAvailableFound(bool)));
    QObject::connect(m_updateManager, SIGNAL(modelChanged()),
                  this, SLOT(onModelChanged()));
    QTimer::singleShot(100, this, SLOT(shouldShow()));
}

void UpdateItem::onUpdateAvailableFound(bool)
{
    if (m_updateManager->model().count() > 0)
        setVisibility(true);
}

void UpdateItem::setVisibility(bool visible)
{
    setVisible(visible);
}

void UpdateItem::onModelChanged()
{
    if (m_updateManager->model().count() > 0)
        setVisibility(true);
    else 
        setVisibility(false);
}

void UpdateItem::shouldShow()
{
    if (m_updateManager->checkTarget())
        setVisibility(true);
}

UpdateItem::~UpdateItem()
{
}

ItemBase *CheckUpdatesPlugin::createItem(const QVariantMap &staticData,
                                 QObject *parent)
{
    return new UpdateItem(staticData, parent);
}

#include "update-plugin.moc"
