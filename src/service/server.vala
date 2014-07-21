/*
 * Copyright (C) 2014 Canonical Ltd.
 *
 * Contact: William Hua <william.hua@canonical.com>
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
 */

[DBus (name = "com.ubuntu.Settings")]
public class Server : Object {

	private string _language;
	private string _locale;

	private DBusConnection _connection;

	private Act.UserManager? _accountsservice_manager;
	private ulong _accountsservice_manager_id;
	private Act.User? _accountsservice_user;
	private ulong _accountsservice_user_id[2];

	public Server (DBusConnection connection) {
		_connection = connection;

		load_accountsservice_manager ();
	}

	~Server () {
		if (_accountsservice_user != null) {
			if (_accountsservice_user_id[1] > 0) {
				((!) _accountsservice_user).disconnect (_accountsservice_user_id[1]);
			}

			if (_accountsservice_user_id[0] > 0) {
				((!) _accountsservice_user).disconnect (_accountsservice_user_id[0]);
			}
		}

		if (_accountsservice_manager != null && _accountsservice_manager_id > 0) {
			((!) _accountsservice_manager).disconnect (_accountsservice_manager_id);
		}
	}

	public string language {
		get { return _language; }

		private set {
			if (value != _language) {
				_language = value;

				try {
					_connection.emit_signal (null,
					                         "/com/canonical/UbuntuSystemSettings",
					                         "org.freedesktop.DBus.Properties",
					                         "PropertiesChanged",
					                         new Variant.parsed ("('com.ubuntu.Settings', { 'language' : <%s> }, @as [])", _language));
				} catch (Error error) {
					warning ("Notify signal on language not emitted: %s", error.message);
				}
			}
		}
	}

	public string locale {
		get { return _locale; }

		private set {
			if (value != _locale) {
				_locale = value;

				try {
					_connection.emit_signal (null,
					                         "/com/canonical/UbuntuSystemSettings",
					                         "org.freedesktop.DBus.Properties",
					                         "PropertiesChanged",
					                         new Variant.parsed ("('com.ubuntu.Settings', { 'locale' : <%s> }, @as [])", _locale));
				} catch (Error error) {
					warning ("Notify signal on locale not emitted: %s", error.message);
				}
			}
		}
	}

	private void load_accountsservice_manager () {
		_accountsservice_manager = Act.UserManager.get_default ();

		if (((!) _accountsservice_manager).is_loaded) {
			load_accountsservice_user ((!) _accountsservice_manager);
		} else {
			_accountsservice_manager_id = ((!) _accountsservice_manager).notify["is-loaded"].connect (() => {
			        if (((!) _accountsservice_manager).is_loaded) {
			                ((!) _accountsservice_manager).disconnect (_accountsservice_manager_id);
			                _accountsservice_manager_id = 0;
			                load_accountsservice_user ((!) _accountsservice_manager);
			        }
			});
		}
	}

	private void load_accountsservice_user (Act.UserManager manager) {
		_accountsservice_user = manager.get_user (Environment.get_user_name ());

		if (_accountsservice_user != null) {
			if (((!) _accountsservice_user).is_loaded) {
				watch_accountsservice_user ((!) _accountsservice_user);
			} else {
				_accountsservice_user_id[0] = ((!) _accountsservice_user).notify["is-loaded"].connect (() => {
				        if (((!) _accountsservice_user).is_loaded) {
				                ((!) _accountsservice_user).disconnect (_accountsservice_user_id[0]);
				                _accountsservice_user_id[0] = 0;
				                watch_accountsservice_user ((!) _accountsservice_user);
				        }
				});
			}
		}
	}

	private void watch_accountsservice_user (Act.User user) {
		_accountsservice_user_id[0] = user.notify["language"].connect (() => { language = user.language; });
		_accountsservice_user_id[1] = user.notify["formats_locale"].connect (() => { locale = user.formats_locale; });
		language = user.language;
		locale = user.formats_locale;
	}
}

private static MainLoop loop;

private static int main (string[] args) {
	Bus.own_name (BusType.SESSION,
	              "com.canonical.UbuntuSystemSettings",
	              BusNameOwnerFlags.ALLOW_REPLACEMENT | BusNameOwnerFlags.REPLACE,
	              (connection, name) => {
	                      try {
	                              connection.register_object ("/com/canonical/UbuntuSystemSettings", new Server (connection));
	                      } catch (IOError error) {
	                              warning ("Object registration failed: %s", error.message);
	                      }
	              },
	              null,
	              null);

	loop = new MainLoop ();
	Posix.sigaction_t action = { () => { loop.quit (); } };
	Posix.sigaction (Posix.SIGINT, action, null);
	Posix.sigaction (Posix.SIGTERM, action, null);
	loop.run ();

	return 0;
}
