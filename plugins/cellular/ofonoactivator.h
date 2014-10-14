/*
 * Copyright (C) 2014 Canonical, Ltd.
 *
 * Authors:
 *    Jussi Pakkanen <jussi.pakkanen@canonical.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OFONO_ACTIVATOR
#define OFONO_ACTIVATOR

#include <QObject>

/**
 * For exposing ofono controls to qml.
 */

class OfonoActivator : public QObject {
    Q_OBJECT

public:
    OfonoActivator(QObject *parent = nullptr);
    ~OfonoActivator() {};

    Q_INVOKABLE bool activate(const QString ofonoContext,  const QString imsi, const QString modemPath);

private:
};


#endif
