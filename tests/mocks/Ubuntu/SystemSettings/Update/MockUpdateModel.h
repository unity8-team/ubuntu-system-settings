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

#ifndef MOCK_UPDATE_MODEL_H
#define MOCK_UPDATE_MODEL_H

#include <QObject>

class MockUpdateModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(UpdateTypes filter
               READ filter
               WRITE setFilter
               NOTIFY filterChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_ENUMS(UpdateTypes)
public:
    enum class UpdateTypes
    {
        All,
        Pending,
        PendingClicksUpdates,
        PendingSystemUpdates,
        InstalledClicksUpdates,
        InstalledSystemUpdates,
        Installed
    };
    explicit MockUpdateModel(QObject *parent = 0) : QObject(parent) {}
    ~MockUpdateModel(){}

    int count() const { return m_count; }

    Q_INVOKABLE void mockCount(const int &count)
    {
        m_count = count;
        Q_EMIT (countChanged());
    }
    void setFilter(const UpdateTypes &filter)
    {
        m_filter = filter;
        Q_EMIT (filterChanged());
    }
    UpdateTypes filter() const
    {
        return m_filter;
    }

signals:
    void countChanged();
    void filterChanged();

private:
    int m_count;
    UpdateTypes m_filter;
};

#endif // MOCK_UPDATE_MODEL_H
