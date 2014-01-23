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
.pragma library

function convert_bytes_to_size(bytes) {
    var SIZE_IN_GIB = 1024.0 * 1024.0 * 1024.0;
    var SIZE_IN_MIB = 1024.0 * 1024.0;
    var SIZE_IN_KIB = 1024.0;

    var result = "";
    var size = 0;
    if (bytes < SIZE_IN_KIB) {
        result = bytes + " bytes";
    } else if (bytes < SIZE_IN_MIB) {
        size = (bytes / SIZE_IN_KIB).toFixed(2);
        result = bytes + " KiB";
    } else if (bytes < SIZE_IN_GIB) {
        size = (bytes / SIZE_IN_MIB).toFixed(2);
        result = size + " MiB";
    } else {
        size = (bytes / SIZE_IN_GIB).toFixed(2);
        result = size + " GiB";
    }

    return result;
}
