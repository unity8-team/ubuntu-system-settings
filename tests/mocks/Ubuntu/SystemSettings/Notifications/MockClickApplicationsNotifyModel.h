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
#include "click_applications_notify_model.h"

class MockClickApplicationsModel;

class MockClickApplicationsNotifyModel : public ClickApplicationsNotifyModel {
    Q_OBJECT

    Q_PROPERTY(MockClickApplicationsModel* sourceModel READ sourceModel WRITE setSourceModel NOTIFY sourceModelChanged)

public:
    MockClickApplicationsModel* sourceModel() const;
    void setSourceModel(MockClickApplicationsModel* sourceModel);

    Q_INVOKABLE bool setNotifyEnabled(int /*row*/, bool /*enabled*/) { return true; };

Q_SIGNALS:
    void sourceModelChanged() const;
};
