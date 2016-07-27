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

#ifndef CLICK_MANAGER_H
#define CLICK_MANAGER_H

#include <QObject>
#include <QString>

namespace UpdatePlugin
{
namespace Click
{
class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = 0) : QObject(parent) {};
    virtual ~Manager() {};

    virtual void check() = 0;
    virtual void retry(const QString &identifier, const uint &revision) = 0;
    virtual void cancel() = 0;
    virtual void launch(const QString &identifier, const uint &revision) = 0;

    virtual bool authenticated() const = 0;
    virtual bool checkingForUpdates() const = 0;

Q_SIGNALS:
    void authenticatedChanged();
    void checkingForUpdatesChanged();

    void checkCompleted();

    void networkError();
    void serverError();
    void credentialError();
};
} // Click
} // UpdatePlugin

#endif // CLICK_MANAGER_H
