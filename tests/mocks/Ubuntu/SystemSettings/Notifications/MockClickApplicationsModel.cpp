/*
 * Copyright 2016 Canonical Ltd.
 *
 * This file is part of system-settings.
 *
 * webbrowser-app is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * webbrowser-app is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// local
#include "MockClickApplicationsModel.h"

void MockClickApplicationsModel::addApplication(const QString& pkgName, const QString& appName)
{
    ClickApplicationEntry entry;

    entry.pkgName = pkgName;
    entry.appName = appName;
    entry.displayName = appName;

    entry.enableNotifications = true;
    entry.soundsNotify = true;
    entry.vibrationsNotify = true;
    entry.bubblesNotify = true;
    entry.listNotify = true;

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_entries << entry;
    endInsertRows();
    Q_EMIT rowCountChanged();
}

void MockClickApplicationsModel::removeApplicationByIndex(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    m_entries.removeAt(index);
    endRemoveRows();
    Q_EMIT rowCountChanged();
}

void MockClickApplicationsModel::setNotificationByIndex(int role, int idx, bool enabled)
{
    switch (role) {
    case EnableNotifications:
        m_entries[idx].enableNotifications = enabled;
        break;

    case SoundsNotify:
        m_entries[idx].soundsNotify = enabled;
        break;

    case VibrationsNotify:
        m_entries[idx].vibrationsNotify = enabled;
        break;

    case BubblesNotify:
        m_entries[idx].bubblesNotify = enabled;
        break;

    case ListNotify:
        m_entries[idx].listNotify = enabled;
        break;

    default:
        return;
    }

    QVector<int> roles;
    roles << role;

    if (role != EnableNotifications) {
        if (!m_entries[idx].soundsNotify &&
            !m_entries[idx].vibrationsNotify &&
            !m_entries[idx].bubblesNotify &&
            !m_entries[idx].listNotify ) {
            
            if (m_entries[idx].enableNotifications) {
                m_entries[idx].enableNotifications = false;
                roles << EnableNotifications;
            }
        }
    }

    Q_EMIT dataChanged(this->index(idx, 0), this->index(idx, 0), roles);
}

void MockClickApplicationsModel::cleanup()
{
    beginResetModel();
    m_entries.clear();
    endResetModel();
    Q_EMIT rowCountChanged();
}
