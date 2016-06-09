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

#ifndef MOCK_CLICK_UPDATE_MODEL_H
#define MOCK_CLICK_UPDATE_MODEL_H

#include <QObject>

class MockClickUpdateModel : public QObject
{
    Q_OBJECT
public:
    MockClickUpdateModel(QObject *parent) {}

    ~MockClickUpdateModel(){}

    Q_PROPERTY(int count READ count NOTIFY countChanged)

    int count() const { return m_count; }

    Q_INVOKABLE void mockCount(const int &count)
    {
        m_count = count;
        Q_EMIT (countChanged());
    }

signals:
    void countChanged();

private:
    int m_count;

};

#endif // MOCK_CLICK_UPDATE_MODEL_H
