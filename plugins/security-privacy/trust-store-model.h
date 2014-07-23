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
 *
 * Authors: Alberto Mardegan <alberto.mardegan@canonical.com>
 */

#ifndef TRUST_STORE_MODEL_H
#define TRUST_STORE_MODEL_H

#include <QAbstractListModel>
#include <QQmlParserStatus>
#include <QString>

class TrustStoreModelPrivate;

class TrustStoreModel: public QAbstractListModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QString serviceName READ serviceName WRITE setServiceName
               NOTIFY serviceNameChanged)
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(int grantedCount READ grantedCount NOTIFY grantedCountChanged)

public:
    explicit TrustStoreModel(QObject *parent = 0);
    ~TrustStoreModel();

    enum Roles {
        ProfileRole = Qt::UserRole + 1,
        IconNameRole,
        GrantedRole,
    };

    void setServiceName(const QString &serviceName);
    QString serviceName() const;

    int grantedCount() const;

    Q_INVOKABLE QVariant get(int row, const QString &roleName) const;

    // reimplemented virtual methods
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

    void classBegin() Q_DECL_OVERRIDE;
    void componentComplete() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void serviceNameChanged();
    void countChanged();
    void grantedCountChanged();

private:
    TrustStoreModelPrivate *d_ptr;
    Q_DECLARE_PRIVATE(TrustStoreModel)
};

#endif // TRUST_STORE_MODEL_H
