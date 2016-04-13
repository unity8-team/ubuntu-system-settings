'''download manager D-BUS mock template'''

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

BUS_NAME = 'com.canonical.applications.Downloader'
MAIN_IFACE = 'com.canonical.applications.DownloadManager'
MAIN_OBJ = '/'
SYSTEM_BUS = False


def load(mock, parameters):
    global _parameters
    _parameters = parameters

    mock.props = {
        'downloads': _parameters.get('downloads', dbus.Array([])),
    }


@dbus.service.method(MAIN_IFACE,
                     in_signature='sb', out_signature='ao')
def getAllDownloads(self, appId, uncollected):
    import syslog
    syslog.syslog("get all downloads")
    return dbus.Array(['/foo', '/bar'], signature='o')


@dbus.service.method(MAIN_IFACE,
                     in_signature='(sssa{sv}a{ss})', out_signature='o')
def createDownload(self, download_struct):
    import syslog
    syslog.syslog("create download", download_struct)
    ds = DownloadStruct(*download_struct)
    downloads = self.props["downloads"]
    ds.path = "/singledownload_%s" % len(downloads)
    downloads.append(ds)
    self.props["downloads"] = downloads
    return ds.path


class DownloadStruct():
    def __init__(self, url, hash, algorithm, metadata, headers):
        self.url = url
        self.hash = hash
        self.algorithm = algorithm
        self.metadata = metadata
        self.headers = headers
        self.path = ""

"""
'<!DOCTYPE node PUBLIC "-//freedesktop//DTD D-BUS Object Introspection 1.0//EN"
"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd">
<node>
  <interface name="com.canonical.applications.DownloadManager">
    <method name="createDownload">
      <annotation value="DownloadStruct"
        name="org.qtproject.QtDBus.QtTypeName.In0"/>
      <arg direction="in" type="(sssa{sv}a{ss})" name="download"/>
      <arg direction="out" type="o" name="downloadPath"/>
    </method>
    <method name="createMmsDownload">
      <arg direction="in" type="s" name="url"/>
      <arg direction="in" type="s" name="hostname"/>
      <arg direction="in" type="i" name="port"/>
      <arg direction="out" type="o" name="downloadPath"/>
    </method>
    <method name="createDownloadGroup">
      <annotation value="StructList"
        name="org.qtproject.QtDBus.QtTypeName.In0"/>
      <annotation value="QVariantMap"
        name="org.qtproject.QtDBus.QtTypeName.In3"/>
      <annotation value="StringMap"
        name="org.qtproject.QtDBus.QtTypeName.In4"/>
      <arg direction="in" type="a(sss)" name="downloads"/>
      <arg direction="in" type="s" name="algorithm"/>
      <arg direction="in" type="b" name="allowed3G"/>
      <arg direction="in" type="a{sv}" name="metadata"/>
      <arg direction="in" type="a{ss}" name="headers"/>
      <arg direction="out" type="o" name="download"/>
    </method>
    <method name="getAllDownloads">
      <arg direction="in" type="s" name="appId"/>
      <arg direction="in" type="b" name="uncollected"/>
      <arg direction="out" type="ao" name="downloads"/>
    </method>
    <method name="getAllDownloadsWithMetadata">
      <arg direction="in" type="s" name="name"/>
      <arg direction="in" type="s" name="value"/>
      <arg direction="out" type="ao" name="downloads"/>
    </method>
    <method name="getDownloadState">
      <annotation value="DownloadStateStruct"
        name="org.qtproject.QtDBus.QtTypeName.Out0"/>
      <arg direction="in" type="s" name="downloadId"/>
      <arg direction="out" type="(issss)" name="state"/>
    </method>
    <method name="setDefaultThrottle">
      <arg direction="in" type="t" name="speed"/>
    </method>
    <method name="defaultThrottle">
      <arg direction="out" type="t" name="speed"/>
    </method>
    <method name="allowGSMDownload">
      <arg direction="in" type="b" name="allowed"/>
    </method>
    <method name="isGSMDownloadAllowed">
      <arg direction="out" type="b" name="allowed"/>
    </method>
    <method name="exit"/>
    <signal name="downloadCreated">
      <arg direction="out" type="o" name="path"/>
    </signal>
  </interface>
  <interface name="org.freedesktop.DBus.Properties">
    <method name="Get">
      <arg name="interface_name" type="s" direction="in"/>
      <arg name="property_name" type="s" direction="in"/>
      <arg name="value" type="v" direction="out"/>
    </method>
    <method name="Set">
      <arg name="interface_name" type="s" direction="in"/>
      <arg name="property_name" type="s" direction="in"/>
      <arg name="value" type="v" direction="in"/>
    </method>
    <method name="GetAll">
      <arg name="interface_name" type="s" direction="in"/>
      <arg name="values" type="a{sv}" direction="out"/>
      <annotation
        name="org.qtproject.QtDBus.QtTypeName.Out0" value="QVariantMap"/>
    </method>
    <signal name="PropertiesChanged">
      <arg name="interface_name" type="s" direction="out"/>
      <arg name="changed_properties" type="a{sv}" direction="out"/>
      <annotation
        name="org.qtproject.QtDBus.QtTypeName.Out1" value="QVariantMap"/>
      <arg name="invalidated_properties" type="as" direction="out"/>
    </signal>
  </interface>
  <interface name="org.freedesktop.DBus.Introspectable">
    <method name="Introspect">
      <arg name="xml_data" type="s" direction="out"/>
    </method>
  </interface>
  <interface name="org.freedesktop.DBus.Peer">
    <method name="Ping"/>
    <method name="GetMachineId">
      <arg name="machine_uuid" type="s" direction="out"/>
    </method>
  </interface>
  </node>
'"""
