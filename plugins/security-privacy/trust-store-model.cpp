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

#include "trust-store-model.h"

#include <QList>
#include <QMap>
#include <QSet>
#include <QSettings>
#include <QStandardPaths>

#include <core/trust/resolve.h>
#include <core/trust/store.h>

namespace dbus = core::dbus;

class Application
{
public:
    Application() {}

    void setProfile(const QString &profile) {
        this->profile = profile;

        QString localShare =
            QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
        QSettings desktopFile(QString("%1/applications/%2.desktop").
                              arg(localShare).arg(profile),
                              QSettings::IniFormat);
        displayName = desktopFile.value("Name").toString();
        iconName = desktopFile.value("Icon").toString();
    }

    void addRequest(const core::trust::Request &request) {
        if (request.answer == core::trust::Request::Answer::granted) {
            grantedFeatures.insert(request.feature);
        }
    }

    bool hasGrants() const { return !grantedFeatures.isEmpty(); }

    QString profile;
    QString displayName;
    QString iconName;
    QSet<std::uint64_t> grantedFeatures;
};

class TrustStoreModelPrivate: public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(TrustStoreModel)

public:
    TrustStoreModelPrivate(TrustStoreModel *model);
    ~TrustStoreModelPrivate();

    void update();
    void updateGrantedCount();

private:
    QHash<int, QByteArray> roleNames;
    bool componentCompleted;
    QString serviceName;
    int grantedCount;
    std::shared_ptr<core::trust::Store> trustStore;
    QList<Application> applications;
    mutable TrustStoreModel *q_ptr;
};

TrustStoreModelPrivate::TrustStoreModelPrivate(TrustStoreModel *model):
    QObject(model),
    componentCompleted(false),
    grantedCount(0),
    q_ptr(model)
{
}

TrustStoreModelPrivate::~TrustStoreModelPrivate()
{
}

void TrustStoreModelPrivate::update()
{
    Q_Q(TrustStoreModel);

    if (!componentCompleted) return;

    q->beginResetModel();

    if (trustStore) {
        trustStore.reset();
    }

    trustStore = core::trust::resolve_store_in_session_with_name(
        serviceName.toStdString());
    auto query = trustStore->query();
    query->execute();

    QMap<QString,Application> appMap;
    while (query->status() != core::trust::Store::Query::Status::eor) {
        auto r = query->current();

        QString profile = QString::fromStdString(r.from);

        Application &app = appMap[profile];
        app.setProfile(profile);
        app.addRequest(r);

        query->next();
    }

    applications = appMap.values();
    updateGrantedCount();

    q->endResetModel();
}

void TrustStoreModelPrivate::updateGrantedCount()
{
    Q_Q(TrustStoreModel);

    int count = 0;

    Q_FOREACH(const Application &app, applications) {
        if (app.hasGrants()) count++;
    }

    if (count != grantedCount) {
        grantedCount = count;
        Q_EMIT q->grantedCountChanged();
    }
}

TrustStoreModel::TrustStoreModel(QObject *parent):
    QAbstractListModel(parent),
    d_ptr(new TrustStoreModelPrivate(this))
{
    Q_D(TrustStoreModel);
    d->roleNames[ProfileRole] = "profile";
    d->roleNames[IconNameRole] = "iconName";
    d->roleNames[GrantedRole] = "granted";

    QObject::connect(this, SIGNAL(rowsInserted(const QModelIndex &,int,int)),
                     this, SIGNAL(countChanged()));
    QObject::connect(this, SIGNAL(rowsRemoved(const QModelIndex &,int,int)),
                     this, SIGNAL(countChanged()));
    QObject::connect(this, SIGNAL(modelReset()),
                     this, SIGNAL(countChanged()));
}

TrustStoreModel::~TrustStoreModel()
{
}

void TrustStoreModel::classBegin()
{
}

void TrustStoreModel::componentComplete()
{
    Q_D(TrustStoreModel);
    d->componentCompleted = true;
    d->update();
}

void TrustStoreModel::setServiceName(const QString &serviceName)
{
    Q_D(TrustStoreModel);

    if (serviceName == d->serviceName) return;
    d->serviceName = serviceName;
    d->update();
    Q_EMIT serviceNameChanged();
}

QString TrustStoreModel::serviceName() const
{
    Q_D(const TrustStoreModel);
    return d->serviceName;
}

int TrustStoreModel::grantedCount() const
{
    Q_D(const TrustStoreModel);
    return d->grantedCount;
}

QVariant TrustStoreModel::get(int row, const QString &roleName) const
{
    int role = roleNames().key(roleName.toLatin1(), -1);
    return data(index(row), role);
}

int TrustStoreModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const TrustStoreModel);
    Q_UNUSED(parent);
    return d->applications.count();
}

QVariant TrustStoreModel::data(const QModelIndex &index, int role) const
{
    Q_D(const TrustStoreModel);

    if (index.row() >= d->applications.count()) return QVariant();

    const Application &app = d->applications.at(index.row());
    QVariant ret;

    switch (role) {
    case Qt::DisplayRole:
        ret = app.displayName;
        break;
    case IconNameRole:
        ret = app.iconName;
        break;
    case ProfileRole:
        ret = app.profile;
        break;
    case GrantedRole:
        ret = app.hasGrants();
        break;
    }

    return ret;
}

QHash<int, QByteArray> TrustStoreModel::roleNames() const
{
    Q_D(const TrustStoreModel);
    return d->roleNames;
}

#include "trust-store-model.moc"
