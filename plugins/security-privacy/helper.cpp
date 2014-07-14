/*
 * Copyright (C) 2014 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "polkitlistener.h"
#include <iostream>

int main()
{
    UssPolkitListener *polkitListener = uss_polkit_listener_new();
    if (polkitListener == nullptr)
        return 3;

    // Read pid
    std::string pid;
    std::getline(std::cin, pid);
    uss_polkit_listener_set_pid(polkitListener, atoi(pid.c_str()));

    // Read password
    std::string password;
    std::getline(std::cin, password);
    uss_polkit_listener_set_password(polkitListener, password.c_str());

    if (!uss_polkit_listener_register(polkitListener))
        return 2;

    // Tell caller that we're ready to start
    std::cout << "ready" << std::endl;

    if (!uss_polkit_listener_run(polkitListener))
        return 1;

    uss_polkit_listener_free(polkitListener);
    return 0;
}
