/*
 * Copyright (C) 2016 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#ifndef CLICKAPPLICATIONENTRY_H
#define CLICKAPPLICATIONENTRY_H

#include <QtCore/QObject>
#include <QtCore/QUrl>

class ClickApplicationEntry : public QObject
{
    Q_OBJECT

public:
    ClickApplicationEntry(QObject* parent=0);
    ~ClickApplicationEntry();

    Q_PROPERTY(QString displayName READ displayName() CONSTANT)
    Q_PROPERTY(int soundsNotify READ soundsNotify WRITE setSoundsNotify NOTIFY soundsNotifyChanged)
    Q_PROPERTY(int vibrationsNotify READ vibrationsNotify WRITE setVibrationsNotify NOTIFY vibrationsNotifyChanged)
    Q_PROPERTY(int bubblesNotify READ bubblesNotify WRITE setBubblesNotify NOTIFY bubblesNotifyChanged)
    Q_PROPERTY(int listNotify READ listNotify WRITE setListNotify NOTIFY listNotifyChanged)

    QString pkgName() const { return m_pkgName; };
    QString version() const { return m_version; };
    QString appName() const { return m_appName; };
    QString displayName() const { return m_displayName; };
    QUrl icon() const { return m_icon; };
    bool soundsNotify() const { return m_soundsNotify; };
    bool vibrationsNotify() const { return m_vibrationsNotify; };
    bool bubblesNotify() const { return m_bubblesNotify; };
    bool listNotify() const { return m_listNotify; };

    void setPkgName(QString pkgName) { m_pkgName = pkgName; };
    void setVersion(QString version) { m_version = version; };
    void setAppName(QString appName) { m_appName = appName; };
    void setDisplayName(QString displayName) { m_displayName = displayName; };
    void setIcon(QUrl icon) { m_icon = icon; };

    void setSoundsNotify(bool notify);
    void setVibrationsNotify(bool notify);
    void setBubblesNotify(bool notify);
    void setListNotify(bool notify);

Q_SIGNALS:
    void soundsNotifyChanged() const;
    void vibrationsNotifyChanged() const;
    void bubblesNotifyChanged() const;
    void listNotifyChanged() const;

private:
    QString m_pkgName;         
    QString m_version;
    QString m_appName;         
    QString m_displayName;
    QUrl m_icon;     
    bool m_soundsNotify;
    bool m_vibrationsNotify;
    bool m_bubblesNotify;
    bool m_listNotify;
};

#endif // CLICKAPPLICATIONENTRY_H
