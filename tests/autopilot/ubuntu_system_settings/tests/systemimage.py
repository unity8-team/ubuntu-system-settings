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


def information(self):
    return dbus.Dictionary({
        'target_build_number': str(self.si_props['target_build_number']),
        'device_name': self.si_props['device'],
        'last_check_date': self.si_props['last_check_date'],
        'version_detail': self.si_props['version_detail'],
        'channel_name': self.si_props['channel'],
        'last_update_date': self.si_props['last_update_date'],
        'current_build_number': str(self.si_props['build_number'])
    }, signature='ss')


def pausedownload(self):
    return self.si_props['reply_on_pause']


def cancelupdate(self):
    return self.si_props['reply_on_cancel']


def getsetting(self, key):
    return str(self.si_props[key])


def setsetting(self, key, value):
    self.si_props[key] = value
    self.EmitSignal(
        MAIN_IFACE, 'SettingChanged', 'ss', [key, value]
    )


def load(mock, parameters):
    global _parameters
    _parameters = parameters

    mock.si_props = {
        'build_number': _parameters.get('build_number', 0),
        'device': _parameters.get('device', ''),
        'auto_download': _parameters.get('auto_download', -1),
        'channel': _parameters.get('channel', ''),
        'last_update_date': _parameters.get('last_update_date', ''),
        'last_check_date': _parameters.get('last_check_date', ''),
        'target_build_number': _parameters.get('target_build_number', -1),
        'target_version_detail': _parameters.get('target_version_detail', ''),
        'version_detail': _parameters.get('version_detail', ''),
        'update_available': _parameters.get('update_available', False),
        'reply_on_pause': _parameters.get('reply_on_pause', ''),
        'reply_on_cancel': _parameters.get('reply_on_cancel', ''),
        'failures_before_warning': _parameters.get('failures_before_warning',
                                                   None),
    }

    mock.information = information
    mock.pausedownload = pausedownload
    mock.cancelupdate = cancelupdate
    mock.getsetting = getsetting
    mock.setsetting = setsetting

    mock.AddMethods(MAIN_IFACE, [
        ('Information', '', 'a{ss}', 'ret = self.information(self)'),
        ('Exit', '', '', ''),
        ('ApplyUpdate', '', '', ''),
        ('CheckForUpdate', '', '', ''),
        ('FactoryReset', '', '', ''),
        ('ProductionReset', '', '', ''),
        ('UpdateDownloaded', '', '', ''),
        ('DownloadUpdate', '', '', ''),
        ('ForceAllowGSMDownload', '', '', ''),
        ('GetSetting', 's', 's', 'ret = self.getsetting(self, args[0])'),
        ('SetSetting', 'ss', '',
         'ret = self.setsetting(self, args[0], args[1])'),
        ('PauseDownload', '', 's', 'ret = self.pausedownload(self)'),
        ('CancelUpdate', '', 's', 'ret = self.cancelupdate(self)'),
    ])
