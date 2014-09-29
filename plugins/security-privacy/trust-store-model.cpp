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

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QIcon>
#include <QList>
#include <QMap>
#include <QSet>
#include <QStandardPaths>

#include <core/trust/resolve.h>
#include <core/trust/store.h>

#include <glib.h>

class Application
{
public:
    Application() {}

    void setId(const QString &id) {
        this->id = id;
        GKeyFile *desktopInfo = g_key_file_new();
        QString desktopFilename = resolveDesktopFilename(id);

        gboolean loaded = g_key_file_load_from_file(desktopInfo,
                                                    desktopFilename.toUtf8().data(),
                                                    G_KEY_FILE_NONE,
                                                    nullptr);

        if (!loaded) {
            g_warning("Couldn't parse the desktop: %s", desktopFilename.toUtf8().data());
            g_key_file_free(desktopInfo);
            return;
        }

        gchar *name = g_key_file_get_locale_string(desktopInfo,
                                                   G_KEY_FILE_DESKTOP_GROUP,
                                                   G_KEY_FILE_DESKTOP_KEY_NAME,
                                                   nullptr,
                                                   nullptr);
        displayName = QString::fromUtf8(name);

        gchar *icon = g_key_file_get_string(desktopInfo,
                                            G_KEY_FILE_DESKTOP_GROUP,
                                            G_KEY_FILE_DESKTOP_KEY_ICON,
                                            nullptr);
        gchar *path = g_key_file_get_string(desktopInfo,
                                            G_KEY_FILE_DESKTOP_GROUP,
                                            G_KEY_FILE_DESKTOP_KEY_PATH,
                                            nullptr);
        iconName = resolveIcon(QString::fromUtf8(icon),
                               QString::fromUtf8(path));
        g_free(name);
        g_free(icon);
        g_free(path);
        g_key_file_free(desktopInfo);
    }

    QString resolveDesktopFilename(const QString &id) {
        QString localShare =
            QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
        QString desktopFilename(QString("%1/applications/%2.desktop").
                                arg(localShare).arg(id));
        if (QFile(desktopFilename).exists())
            return desktopFilename;

        /* search the directory for a matching filename */
        QDir dir(QString("%1/applications").arg(localShare));
        dir.setFilter(QDir::Files);
        QStringList fileList = dir.entryList();
        QString pattern = QString("%1*.desktop").arg(id);
        for (int i = 0; i < fileList.count(); i++) {
            /* stop at the first match */
            if (QDir::match(pattern, fileList[i])) {
                return QString("%1/applications/%2").arg(localShare).arg(fileList[i]);
            }
        } 
        
        qWarning() << "No desktop file found for app id: " << id;
        return QString();
    }

    QString resolveIcon(const QString &iconName, const QString &basePath) {
        /* If iconName points to a valid file, use it */
        if (QFile::exists(iconName)) {
            return iconName;
        }

        /* See if the iconName resolves to a file installed by the click
         * package (which is extracted in basePath). */
        QDir baseDir(basePath);
        QString iconFilePath =
            baseDir.absoluteFilePath(QDir::cleanPath(iconName));
        if (QFile::exists(iconFilePath)) {
            return iconFilePath;
        }

        /* Is iconName a valid theme icon? */
        if (QIcon::hasThemeIcon(iconName)) {
            return "image://theme/" + iconName;
        }

        return QString();
    }

    void addRequest(const core::trust::Request &request) {
        if (request.answer == core::trust::Request::Answer::granted) {
            grantedFeatures.insert(request.feature.value);
        } else {
            grantedFeatures.remove(request.feature.value);
        }
    }

    bool hasGrants() const { return !grantedFeatures.isEmpty(); }

    QString id;
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
    void updateRow(int row);
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

    /* Test is the trustStore is valid; ideally, there should be an API on the
     * trust-store to check this. See
     * https://bugs.launchpad.net/bugs/1348215 */
    try {
        auto query = trustStore->query();
    } catch (std::exception &e) {
        qWarning() << "Exception " << e.what();
        trustStore.reset();
    }

    QMap<QString,Application> appMap;

    if (trustStore) {
        auto query = trustStore->query();
        query->execute();

        while (query->status() != core::trust::Store::Query::Status::eor) {
            auto r = query->current();

            QString applicationId = QString::fromStdString(r.from);

            /* filter out unconfined apps, they can access everything anyway */
            if (applicationId == "unconfined") {
                query->next();
                continue;
            }

            Application &app = appMap[applicationId];
            app.setId(applicationId);
            app.addRequest(r);

            query->next();
        }
    }

    applications = appMap.values();
    updateGrantedCount();

    q->endResetModel();
}

void TrustStoreModelPrivate::updateRow(int row)
{
    Q_Q(TrustStoreModel);

    Q_ASSERT(trustStore);
    Q_ASSERT(row >= 0 && row < applications.count());

    Application &app = applications[row];
    app.grantedFeatures.clear();

    auto query = trustStore->query();
    query->for_application_id(app.id.toStdString());
    query->execute();

    while (query->status() != core::trust::Store::Query::Status::eor) {
        auto r = query->current();

        app.addRequest(r);

        query->next();
    }

    updateGrantedCount();

    /* Let the model emit the change notification */
    QModelIndex index = q->index(row);
    q->dataChanged(index, index);
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
    d->roleNames[Qt::DisplayRole] = "applicationName";
    d->roleNames[ApplicationIdRole] = "applicationId";
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

void TrustStoreModel::setEnabled(int row, bool enabled)
{
    Q_D(TrustStoreModel);

    if (Q_UNLIKELY(!d->trustStore)) {
        qWarning() << "Trust store is NULL on setEnabled call";
        return;
    }

    if (Q_UNLIKELY(row >= d->applications.count())) return;

    const Application &app = d->applications.at(row);

    core::trust::Request r;
    r.from = app.id.toStdString();
    r.feature = core::trust::Feature(core::trust::Request::default_feature);
    r.answer = enabled ?
        core::trust::Request::Answer::granted : core::trust::Request::Answer::denied;
    r.when = std::chrono::system_clock::now();

    d->trustStore->add(r);

    /* When disabling, we must disable all the features */
    if (!enabled) {
        Q_FOREACH(std::int64_t feature, app.grantedFeatures) {
            /* Skip the default feature, we already disabled it */
            if (feature == core::trust::Request::default_feature) continue;

            r.feature = core::trust::Feature(feature);
            d->trustStore->add(r);
        }
    }

    /* Reload the application from the trust store */
    d->updateRow(row);
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
    case ApplicationIdRole:
        ret = app.id;
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
