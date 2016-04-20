'''system image D-BUS mock template'''

# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 3 of the License, or (at your option) any
# later version.  See http://www.gnu.org/copyleft/lgpl.html for the full text
# of the license.
import dbus

__author__ = 'Jonas G. Drange'
__email__ = 'jonas.drange@canonical.com'
__copyright__ = '(c) 2015 Canonical Ltd.'
__license__ = 'LGPL 3+'

BUS_NAME = 'com.canonical.SystemImage'
MAIN_IFACE = 'com.canonical.SystemImage'
MAIN_OBJ = '/Service'
SYSTEM_BUS = True


def load(mock, parameters):
    global _parameters
    _parameters = parameters

    mock.si_props = {
        'build_number': _parameters.get('build_number', 0),
        'device': _parameters.get('device', ''),
        'channel': _parameters.get('channel', ''),
        'last_update_date': _parameters.get('last_update_date', ''),
        'last_check_date': _parameters.get('last_check_date', ''),
        'target_build_number': _parameters.get('target_build_number', -1),
        'target_version_detail': _parameters.get('target_version_detail', ''),
        'version_detail': _parameters.get(
            'version_detail', dbus.Dictionary({}, signature='ss')
        ),
        'update_available': _parameters.get('update_available', False)
    }


@dbus.service.method(MAIN_IFACE,
                     in_signature='', out_signature='isssa{ss}')
def Info(self):
    return (
        self.si_props['build_number'],
        self.si_props['device'],
        self.si_props['channel'],
        self.si_props['last_update_date'],
        self.si_props['version_detail']
    )


@dbus.service.method(MAIN_IFACE,
                     in_signature='', out_signature='a{ss}')
def Information(self):

    # Build a version_details key=value string
    vd_dict = self.si_props['version_detail']
    vd_str = ''
    for i, k in enumerate(vd_dict):
        cmma = ','
        if (i == len(vd_dict) - 1):
            cmma = ''
        vd_str += '%s=%s%s' % (k, str(vd_dict[k]), cmma)

    return dbus.Dictionary({
        'target_build_number': str(self.si_props['target_build_number']),
        'device_name': self.si_props['device'],
        'last_check_date': self.si_props['last_check_date'],
        'version_detail': vd_str,
        'channel_name': self.si_props['channel'],
        'last_update_date': self.si_props['last_update_date'],
        'current_build_number': str(self.si_props['build_number'])
    }, signature='ss')


@dbus.service.method(MAIN_IFACE,
                     in_signature='', out_signature='')
def CheckForUpdate(self):
    self.EmitSignal(
        MAIN_IFACE, 'UpdateAvailableStatus', 'bbsiss',
        [self.si_props['update_available'], False, '1000', 100, 'date', ''])

    print("CheckForUpdate   ")


@dbus.service.signal(dbus_interface="%s.%s" % (
    MAIN_IFACE, "UpdateAvailableStatus"
), signature='bbsiss')
def UpdateAvailableStatus(self, is_available, downloading,
                          available_version, update_size,
                          last_update_date, error_reason):
    print("FOOOOooooooooooOOoOo")


"""
'<!DOCTYPE node PUBLIC "-//freedesktop//DTD D-BUS Object Introspection 1.0//EN"
"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd">
<node name="/Service">
  <interface name="org.freedesktop.DBus.Introspectable">
    <method name="Introspect">
      <arg direction="out" type="s" />
    </method>
  </interface>
  <interface name="com.canonical.SystemImage">
    <signal name="Rebooting">
      <arg type="b" name="status" />
    </signal>
    <signal name="UpdateAvailableStatus">
      <arg type="b" name="is_available" />
      <arg type="b" name="downloading" />
      <arg type="s" name="available_version" />
      <arg type="i" name="update_size" />
      <arg type="s" name="last_update_date" />
      <arg type="s" name="error_reason" />
    </signal>
    <method name="Information">
      <arg direction="out" type="a{ss}" />
    </method>
    <signal name="DownloadStarted">
    </signal>
    <signal name="Applied">
      <arg type="b" name="status" />
    </signal>
    <method name="Exit">
    </method>
    <method name="ApplyUpdate">
    </method>
    <signal name="UpdateDownloaded">
    </signal>
    <method name="DownloadUpdate">
    </method>
    <method name="ForceAllowGSMDownload">
    </method>
    <signal name="SettingChanged">
      <arg type="s" name="key" />
      <arg type="s" name="new_value" />
    </signal>
    <signal name="UpdatePaused">
      <arg type="i" name="percentage" />
    </signal>
    <signal name="UpdateProgress">
      <arg type="i" name="percentage" />
      <arg type="d" name="eta" />
    </signal>
    <signal name="UpdateFailed">
      <arg type="i" name="consecutive_failure_count" />
      <arg type="s" name="last_reason" />
    </signal>
    <method name="CheckForUpdate">
    </method>
    <method name="ProductionReset">
    </method>
    <method name="GetSetting">
      <arg direction="in"  type="s" name="key" />
      <arg direction="out" type="s" />
    </method>
    <method name="PauseDownload">
      <arg direction="out" type="s" />
    </method>
    <method name="CancelUpdate">
      <arg direction="out" type="s" />
    </method>
    <method name="SetSetting">
      <arg direction="in"  type="s" name="key" />
      <arg direction="in"  type="s" name="value" />
    </method>
    <method name="FactoryReset">
    </method>
  </interface>
  </node>
'
"""
