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
#include "click_applications_model.h"

class MockClickApplicationsModel : public ClickApplicationsModel {
    Q_OBJECT

public:
    Q_INVOKABLE void addApplication(const QString& pkgName, const QString& appName);
    Q_INVOKABLE void removeApplicationByIndex(int index);
    Q_INVOKABLE void setNotificationByIndex(int role, int idx, bool enabled);
    Q_INVOKABLE void cleanup();

protected Q_SLOTS:
    void populateModel() { /* DO NOTHING */ };
};
