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

#ifndef CLICKAPPLICATIONSNOTIFYMODEL_H
#define CLICKAPPLICATIONSNOTIFYMODEL_H

// Qt
#include <QtCore/QSortFilterProxyModel>

class ClickApplicationsModel;
class ClickApplicationEntry;

class ClickApplicationsNotifyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(ClickApplicationsModel* sourceModel READ sourceModel WRITE setSourceModel NOTIFY sourceModelChanged)
    Q_PROPERTY(int notifyType READ notifyType WRITE setNotifyType NOTIFY notifyTypeChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

    Q_ENUMS(NotifyTypes)

public:
    ClickApplicationsNotifyModel(QObject* parent=0);

    enum NotifyTypes {
        SoundsNotify,
        VibrationsNotify,
        BubblesNotify,
        ListNotify
    };

    ClickApplicationsModel* sourceModel() const;
    void setSourceModel(ClickApplicationsModel* sourceModel);

    int notifyType() const;
    void setNotifyType(int type);

    int count() const;
    Q_INVOKABLE ClickApplicationEntry* get(int row) const;

Q_SIGNALS:
    void sourceModelChanged() const;
    void notifyTypeChanged() const;
    void countChanged() const;

protected:
    // reimplemented from QSortFilterProxyModel
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;

private Q_SLOTS:
    void onModelChanged();

private:
    int m_notifyType;
};

#endif // CLICKAPPLICATIONSNOTIFYMODEL_H
