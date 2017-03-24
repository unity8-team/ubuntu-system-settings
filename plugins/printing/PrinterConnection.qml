/*
 * This file is part of system-settings
 *
 * Copyright (C) 2017 Canonical Ltd.
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
 * Authored by Jonas G. Drange <jonas.drange@canonical.com>
 */

import QtQuick 2.4
import SystemSettings 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.Extras.Printers 0.1
import SystemSettings.ListItems 1.0 as SettingsListItems

Column {
    id: root
    property alias host: hostField.textFieldText
    property bool enabled: true

    function stringToType(str) {
        if (str == "lpd") return PrinterEnum.LPDType;
        if (str == "ipps") return PrinterEnum.IppSType;
        if (str == "ipp14") return PrinterEnum.Ipp14Type;
        if (str == "http") return PrinterEnum.HttpType;
        if (str == "beh") return PrinterEnum.BehType;
        if (str == "socket") return PrinterEnum.SocketType;
        if (str == "https") return PrinterEnum.HttpsType;
        if (str == "ipp") return PrinterEnum.IppType;
        if (str == "hp") return PrinterEnum.HPType;
        if (str == "usb") return PrinterEnum.USBType;
        if (str == "hpfax") return PrinterEnum.HPFaxType;
        if (str == "dnssd") return PrinterEnum.DNSSDType;
        return PrinterEnum.UnknownType;
    }

    function typeToString(type) {
        if (type == PrinterEnum.LPDType) return "lpd";
        if (type == PrinterEnum.IppSType) return "ipps";
        if (type == PrinterEnum.Ipp14Type) return "ipp14";
        if (type == PrinterEnum.HttpType) return "http";
        if (type == PrinterEnum.BehType) return "beh";
        if (type == PrinterEnum.SocketType) return "socket";
        if (type == PrinterEnum.HttpsType) return "https";
        if (type == PrinterEnum.IppType) return "ipp";
        if (type == PrinterEnum.HPType) return "hp";
        if (type == PrinterEnum.USBType) return "usb";
        if (type == PrinterEnum.HPFaxType) return "hpfax";
        if (type == PrinterEnum.DNSSDType) return "dnssd";
    }

    property var type: PrinterEnum.IppType

    TextBoxListItem {
        id: hostField
        text: i18n.tr("Host")
        enabled: root.enabled
        placeholderText: "%1://printer.mydomain/%1".arg(typeToString(type))
    }
}
