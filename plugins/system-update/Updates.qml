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
 *
 * This file represents the UI of the System Updates panel.
 */

Column {

    /*!
        \qmlproperty bool userAuthenticated

        Indicates whether or not the user is authenticated and can receive
        updates which require authentication.
    */
    property bool userAuthenticated: false

    /*!
        \qmlproperty bool havePower

        Indicates whether or not there is sufficient power to perform
        updates.
    */
    property bool havePower: false


}
