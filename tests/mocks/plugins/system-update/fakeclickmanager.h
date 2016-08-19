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

#ifndef MOCK_CLICK_MANAGER_H
#define MOCK_CLICK_MANAGER_H

#include "click/manager.h"

#include <QList>
#include <QPair>
#include <QString>

namespace UpdatePlugin
{
class MockClickManager : public Click::Manager
{
public:
    explicit MockClickManager(QObject *parent = nullptr)
        : Click::Manager(parent) {};
    virtual ~MockClickManager() {};

    virtual void check() override
    {
        m_checkingForUpdates = true;
        Q_EMIT checkingForUpdatesChanged();
    }

    virtual void cancel() override
    {
        m_checkingForUpdates = false;
        Q_EMIT checkingForUpdatesChanged();
    }

    virtual bool launch(const QString &identifier) override
    {
        m_launched.append(identifier);
        return true;
    }

    virtual void retry(const QString &identifier, const uint &revision) override
    {
        QPair<QString, uint> p;
        p.first = identifier;
        p.second = revision;
        m_retried.append(p);
    }

    virtual bool authenticated() const override
    {
        return m_authenticated;
    }

    virtual bool checkingForUpdates() const override
    {
        return m_checkingForUpdates;
    }

    void mockChecking(const bool checking)
    {
        m_checkingForUpdates = checking;
        Q_EMIT checkingForUpdatesChanged();
    }

    void mockNetworkError()
    {
        Q_EMIT networkError();
    }

    void mockServerError()
    {
        Q_EMIT serverError();
    }

    bool m_checkingForUpdates = false;
    bool m_authenticated = false;
    QList<QString> m_launched;
    QList<QPair<QString, uint>> m_retried;
};
} // UpdatePlugin

#endif // MOCK_CLICK_MANAGER_H
