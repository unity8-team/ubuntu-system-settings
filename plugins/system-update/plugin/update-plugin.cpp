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

#include "../system_update.h"
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
    void changeVisibility(const QString&, Update*);
    void onUpdateAvailableFound(bool);
    void shouldShow();

private:
    SystemUpdate m_systemUpdate;
    UpdateManager *m_updateManager;
};

UpdateItem::UpdateItem(const QVariantMap &staticData, QObject *parent):
    ItemBase(staticData, parent),
    m_systemUpdate(this)
{
    m_updateManager = UpdateManager::instance();
    QObject::connect(&m_systemUpdate, SIGNAL(updateAvailable(const QString&, Update*)),
                  this, SLOT(changeVisibility(const QString&, Update*)));
    QObject::connect(m_updateManager, SIGNAL(updateAvailableFound(bool)),
                  this, SLOT(onUpdateAvailableFound(bool)));
    QTimer::singleShot(100, this, SLOT(shouldShow()));
}

void UpdateItem::onUpdateAvailableFound(bool)
{
    qWarning() << Q_FUNC_INFO << m_updateManager->model().count();
    if (m_updateManager->model().count() > 0)
        setVisibility(true);
}

void UpdateItem::changeVisibility(const QString&, Update*)
{
    setVisibility(true);
}

void UpdateItem::setVisibility(bool visible)
{
    qWarning() << Q_FUNC_INFO << visible;
    setVisible(visible);
    Q_EMIT visibilityChanged();
}

void UpdateItem::shouldShow()
{
    if (m_systemUpdate.checkTarget())
        setVisibility(true);
    qWarning() << Q_FUNC_INFO << isVisible();
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
