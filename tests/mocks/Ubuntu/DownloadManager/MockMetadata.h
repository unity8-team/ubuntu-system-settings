/*
 * Copyright (C) 2016 Canonical, Ltd.
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

#ifndef MOCK_METADATA_H
#define MOCK_METADATA_H

#include <QObject>
#include <QVariantMap>
#include <QString>

class MockMetadata : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(bool showInIndicator READ showInIndicator WRITE setShowInIndicator NOTIFY showIndicatorChanged)

 public:
    explicit MockMetadata(QObject* parent=0);
    MockMetadata(QVariantMap map, QObject* parent=0);

    QString title() const;
    void setTitle(QString title);

    bool showInIndicator() const;
    void setShowInIndicator(bool shown);

    QVariantMap map() const;

 signals:
    void titleChanged();
    void showIndicatorChanged();
    void deflateChanged();
    void extractChanged();
};

#endif // MOCK_METADATA_H
