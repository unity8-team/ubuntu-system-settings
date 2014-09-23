/*
 * Copyright (C) 2013 Canonical Ltd
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
 * Authors: Iain Lane <iain.lane@canonical.com>
 *
*/

#ifndef CLICK_H
#define CLICK_H

#include <QAbstractTableModel>
#include <QDir>
#include <QObject>
#include <QProcess>
#include <QSortFilterProxyModel>

class ClickModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ClickModel(QObject *parent = 0);
    ~ClickModel();

    Q_ENUMS(Roles)

    enum Roles {
        DisplayNameRole = Qt::DisplayRole,
        InstalledSizeRole = Qt::UserRole + 1,
        IconRole
    };

    struct Click {
        QString name;
        QString displayName;
        QString icon;
        uint installSize;
    };

    // implemented virtual methods from QAbstractTableModel
    int rowCount (const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data (const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;
    quint64 getClickSize() const;

private:
    void populateFromDesktopOrIniFile(Click *newClick,
                                 QVariantMap hooks,
                                 QDir directory);
    Click buildClick(QVariantMap manifest);
    QList<Click> buildClickList();

    QList<Click> m_clickPackages;
    int m_totalClickSize;

};

Q_DECLARE_METATYPE (ClickModel::Roles)

class ClickFilterProxy: public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit ClickFilterProxy(ClickModel *parent = 0);

};

#endif // CLICK_H
