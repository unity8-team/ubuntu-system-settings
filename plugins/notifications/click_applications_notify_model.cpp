/*
 * Copyright (C) 2016 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#include "click_applications_notify_model.h"
#include "click_applications_model.h"

ClickApplicationsNotifyModel::ClickApplicationsNotifyModel(QObject* parent)
    : QSortFilterProxyModel(parent),
    m_notifyType(-1)
{
    connect(this, SIGNAL(rowsInserted(QModelIndex, int, int)), SLOT(onModelChanged()));
    connect(this, SIGNAL(rowsRemoved(QModelIndex, int, int)), SLOT(onModelChanged()));
}

ClickApplicationsModel* ClickApplicationsNotifyModel::sourceModel() const
{
    return qobject_cast<ClickApplicationsModel*>(QSortFilterProxyModel::sourceModel());
}

void ClickApplicationsNotifyModel::setSourceModel(ClickApplicationsModel* sourceModel)
{
    if (sourceModel != this->sourceModel()) {
        QSortFilterProxyModel::setSourceModel(sourceModel);
        Q_EMIT sourceModelChanged();
        Q_EMIT countChanged();
    }
}

int ClickApplicationsNotifyModel::notifyType() const
{
    return m_notifyType;
}

void ClickApplicationsNotifyModel::setNotifyType(int type)
{
    if (m_notifyType != type) {
        m_notifyType = type;
        invalidate();
        Q_EMIT notifyTypeChanged();
        Q_EMIT countChanged();
    }
}

int ClickApplicationsNotifyModel::count() const
{
    return rowCount();
}

QVariantMap ClickApplicationsNotifyModel::get(int row) const
{
    //TODO
}

bool ClickApplicationsNotifyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

    if (m_notifyType == SoundsNotify) {
        return sourceModel()->data(index, ClickApplicationsModel::SoundsNotify).toBool();
    }
    if (m_notifyType == VibrationsNotify) {
        return sourceModel()->data(index, ClickApplicationsModel::VibrationsNotify).toBool();
    }
    if (m_notifyType == BubblesNotify) {
        return sourceModel()->data(index, ClickApplicationsModel::BubblesNotify).toBool();
    }
    if (m_notifyType == ListNotify) {
        return sourceModel()->data(index, ClickApplicationsModel::ListNotify).toBool();
    }

    return false;
}

void ClickApplicationsNotifyModel::onModelChanged()
{
    Q_EMIT countChanged();
}
