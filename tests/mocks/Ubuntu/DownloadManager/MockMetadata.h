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
 */

#ifndef MOCK_METADATA_H
#define MOCK_METADATA_H

#include <QObject>
#include <QVariant>
#include <QVariantMap>
#include <QString>

class MockMetadata : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(bool showInIndicator READ showInIndicator WRITE setShowInIndicator NOTIFY showIndicatorChanged)
    Q_PROPERTY(QStringList command READ command WRITE setCommand NOTIFY commandChanged)
    Q_PROPERTY(QVariantMap custom READ custom WRITE setCustom NOTIFY customChanged)

public:
    explicit MockMetadata(QObject* parent=0);

    QString title() const;
    void setTitle(QString title);

    bool showInIndicator() const;
    void setShowInIndicator(bool shown);

    QVariantMap map() const;

    QStringList command() const;
    void setCommand(const QStringList &command);

    QVariantMap custom() const;
    void setCustom(const QVariantMap &custom);


signals:
    void titleChanged();
    void showIndicatorChanged();
    void commandChanged();
    void customChanged();

private:
    QVariantMap m_map;
    QVariantMap m_custom;

};

#endif // MOCK_METADATA_H
