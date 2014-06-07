/*
 * Copyright (C) 2014 Canonical Ltd
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
 * Authors:
 * Jonas G. Drange <jonas.drange@canonical.com>
 *
*/
#ifndef RADIOSETTINGS_H
#define RADIOSETTINGS_H


#include <QObject>
#include <QtCore>
#include <ofonoradiosettings.h>

class RadioSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString preferedTechnology READ preferedTechnology WRITE setPreferedTechnology NOTIFY preferedTechnologyChanged)
    Q_ENUMS(PreferedTechnology)

public:
    RadioSettings(QObject *parent=0);
    ~RadioSettings();

    enum TechnologyPreference {
        UnknownTechnologyPreference = 0,
        AnyTechnologyPreference,
        GsmTechnologyPreference,
        UmtsTechnologyPreference,
        LteTechnologyPreference
    };

    /* Properties */
    QString preferedTechnology() const;
    void setPreferedTechnology(QString &preferedTechnology);


Q_SIGNALS:
    void preferedTechnologyChanged(const QString &preferedTechnology);

private:
    OfonoRadioSettings *m_ofonoRadioSettings;
    QString m_preferedTechnology;

private Q_SLOTS:
    void operatorPreferedTechnologyChanged(const QString &preferedTechnology);

};


#endif // RADIOSETTINGS_H
