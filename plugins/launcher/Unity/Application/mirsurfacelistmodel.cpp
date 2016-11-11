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

#include "mirsurfacelistmodel.h"

#include "mirsurface.h"

#include <paths.h>

#include <QDebug>
#include <QDebugStateSaver>

namespace unityapp = unity::shell::application;
using namespace qtmir;

MirSurfaceListModel::MirSurfaceListModel(QObject *parent) :
    MirSurfaceListInterface(parent)
{
}

MirSurfaceListModel::~MirSurfaceListModel()
{
    Q_EMIT destroyed(this); // Early warning, while MirSurfaceListModel methods can still be accessed.
}

int MirSurfaceListModel::rowCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? m_surfaceList.size() : 0;
}

QVariant MirSurfaceListModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= m_surfaceList.size())
        return QVariant();

    if (role == SurfaceRole) {
        MirSurfaceInterface *surface = m_surfaceList.at(index.row());
        return QVariant::fromValue(static_cast<unityapp::MirSurfaceInterface*>(surface));
    } else {
        return QVariant();
    }
}

void MirSurfaceListModel::raise(MirSurfaceInterface *surface)
{
    int i = m_surfaceList.indexOf(surface);
    if (i != -1) {
        moveSurface(i, 0);
    }
}

void MirSurfaceListModel::prependSurface(MirSurfaceInterface *surface)
{
    beginInsertRows(QModelIndex(), 0/*first*/, 0/*last*/);
    m_surfaceList.prepend(surface);
    connectSurface(surface);
    endInsertRows();
    Q_EMIT countChanged(m_surfaceList.count());
    if (count() == 1) {
        Q_EMIT emptyChanged();
    }
    Q_EMIT firstChanged();
}

void MirSurfaceListModel::connectSurface(MirSurfaceInterface *surface)
{
    connect(surface, &MirSurfaceInterface::raiseRequested, this, [this, surface](){ this->raise(surface); });
    connect(surface, &QObject::destroyed, this, [this, surface](){ this->removeSurface(surface); });
}

void MirSurfaceListModel::removeSurface(MirSurfaceInterface *surface)
{
    int i = m_surfaceList.indexOf(surface);
    if (i != -1) {
        beginRemoveRows(QModelIndex(), i, i);
        m_surfaceList.removeAt(i);
        endRemoveRows();
        Q_EMIT countChanged(m_surfaceList.count());
        if (count() == 0) {
            Q_EMIT emptyChanged();
        }
        if (i == 0) {
            Q_EMIT firstChanged();
        }
    }
}

void MirSurfaceListModel::moveSurface(int from, int to)
{
    if (from == to) return;

    if (from >= 0 && from < m_surfaceList.size() && to >= 0 && to < m_surfaceList.size()) {
        QModelIndex parent;
        /* When moving an item down, the destination index needs to be incremented
           by one, as explained in the documentation:
           http://qt-project.org/doc/qt-5.0/qtcore/qabstractitemmodel.html#beginMoveRows */

        beginMoveRows(parent, from, from, parent, to + (to > from ? 1 : 0));
        m_surfaceList.move(from, to);
        endMoveRows();
    }

    if ((from == 0 || to == 0) && m_surfaceList.count() > 1) {
        Q_EMIT firstChanged();
    }
}

unityapp::MirSurfaceInterface *MirSurfaceListModel::get(int index)
{
    if (index >=0 && index < m_surfaceList.count()) {
        return m_surfaceList[index];
    } else {
        return nullptr;
    }
}

const unityapp::MirSurfaceInterface *MirSurfaceListModel::get(int index) const
{
    if (index >=0 && index < m_surfaceList.count()) {
        return m_surfaceList.at(index);
    } else {
        return nullptr;
    }
}

void MirSurfaceListModel::prependSurfaces(QList<MirSurfaceInterface*> &surfaceList, int prependFirst, int prependLast)
{
    bool wasEmpty = isEmpty();
    beginInsertRows(QModelIndex(), 0/*first*/, 0 + (prependLast - prependFirst)/*last*/);
    for (int i = prependLast; i >= prependFirst; --i) {
        auto surface = surfaceList[i];
        m_surfaceList.prepend(surface);
        connect(surface, &MirSurfaceInterface::raiseRequested, this,
                [this, surface]()
                {
                    this->raise(surface);
                });
    }
    endInsertRows();
    Q_EMIT countChanged(m_surfaceList.count());
    if (wasEmpty) {
        Q_EMIT emptyChanged();
    }
    Q_EMIT firstChanged();
}

void MirSurfaceListModel::addSurfaceList(MirSurfaceListModel *surfaceListModel)
{
    Q_ASSERT(!m_trackedModels.contains(surfaceListModel));

    if (surfaceListModel->count() > 0) {
        prependSurfaces(surfaceListModel->m_surfaceList, 0, surfaceListModel->count() - 1);
    }

    connect(surfaceListModel, &QAbstractItemModel::rowsInserted, this,
            [this, surfaceListModel](const QModelIndex & /*parent*/, int first, int last)
            {
                this->prependSurfaces(surfaceListModel->m_surfaceList, first, last);
            });

    connect(surfaceListModel, &QAbstractItemModel::rowsAboutToBeRemoved, this,
            [this, surfaceListModel](const QModelIndex & /*parent*/, int first, int last)
            {
                for (int i = first; i <= last; ++i) {
                    this->removeSurface(surfaceListModel->m_surfaceList[i]);
                }
            });

    connect(surfaceListModel, &QObject::destroyed, this,
            [this, surfaceListModel]()
            {
                this->removeSurfaceList(surfaceListModel);
            });

    m_trackedModels.append(surfaceListModel);
}

void MirSurfaceListModel::removeSurfaceList(MirSurfaceListModel *surfaceListModel)
{
    Q_ASSERT(m_trackedModels.contains(surfaceListModel));

    m_trackedModels.removeAll(surfaceListModel);

    disconnect(surfaceListModel, 0, this, 0);

    for (int i = 0; i < surfaceListModel->m_surfaceList.count(); ++i) {
        removeSurface(surfaceListModel->m_surfaceList[i]);
    }
}

bool MirSurfaceListModel::isEmpty() const
{
    return m_surfaceList.count() == 0;
}

///////////////////////////////////////////////////////////////////////////////
// ProxySurfaceListModel
//////////////////////////////////////////////////////////////////////////////

ProxySurfaceListModel::ProxySurfaceListModel(QObject *parent):
    unity::shell::application::MirSurfaceListInterface(parent)
{
}

void ProxySurfaceListModel::setSourceList(MirSurfaceListModel *sourceList)
{
    if (m_sourceList == sourceList)
        return;

    beginResetModel();

    if (m_sourceList) {
        disconnect(m_sourceList, 0, this, 0);
    }

    m_sourceList = sourceList;

    if (m_sourceList) {
        connect(m_sourceList, &QAbstractItemModel::rowsAboutToBeInserted,
                this, [this](const QModelIndex & parent, int first, int last) {this->beginInsertRows(parent, first, last);});
        connect(m_sourceList, &QAbstractItemModel::rowsInserted,
                this, [this](const QModelIndex & /*parent*/, int /*first*/, int /*last*/) {this->endInsertRows();});

        connect(m_sourceList, &QAbstractItemModel::rowsAboutToBeRemoved,
                this, [this](const QModelIndex & parent, int first, int last) {this->beginRemoveRows(parent, first, last);});
        connect(m_sourceList, &QAbstractItemModel::rowsRemoved,
                this, [this](const QModelIndex & /*parent*/, int /*first*/, int /*last*/) {this->endRemoveRows();});

        connect(m_sourceList, &QAbstractItemModel::rowsAboutToBeMoved,
                this, [this](const QModelIndex & sourceParent, int sourceStart, int sourceEnd,
                             const QModelIndex & destinationParent, int destinationRow)
                            {this->beginMoveRows(sourceParent, sourceStart, sourceEnd, destinationParent, destinationRow);});
        connect(m_sourceList, &QAbstractItemModel::rowsMoved,
                this, [this](const QModelIndex & /*parent*/, int /*start*/, int /*end*/,
                             const QModelIndex & /*destination*/, int /*row*/)
                            {this->endMoveRows();});
        connect(m_sourceList, &QObject::destroyed, this, [this]() { this->setSourceList(nullptr); });
        connect(m_sourceList, &unityapp::MirSurfaceListInterface::countChanged,
                this, &unityapp::MirSurfaceListInterface::countChanged);
        connect(m_sourceList, &unityapp::MirSurfaceListInterface::firstChanged,
                this, &unityapp::MirSurfaceListInterface::firstChanged);
    }

    endResetModel();
}

unityapp::MirSurfaceInterface *ProxySurfaceListModel::get(int index)
{
    if (!m_sourceList) {
        return nullptr;
    }

    return m_sourceList->get(index);
}

const unityapp::MirSurfaceInterface *ProxySurfaceListModel::get(int index) const
{
    if (!m_sourceList) {
        return nullptr;
    }

    return m_sourceList->get(index);
}

int ProxySurfaceListModel::rowCount(const QModelIndex &parent) const
{
    if (!m_sourceList) {
        return 0;
    }

    return m_sourceList->rowCount(parent);
}

QVariant ProxySurfaceListModel::data(const QModelIndex& index, int role) const
{
    if (!m_sourceList) {
        return QVariant();
    }

    return m_sourceList->data(index, role);
}

QDebug operator<<(QDebug dbg, const unityapp::MirSurfaceListInterface &surfaceListConst)
{
    auto surfaceList = const_cast<unityapp::MirSurfaceListInterface*>(&surfaceListConst);

    QDebugStateSaver saver(dbg);
    dbg.nospace();
    dbg << "MirSurfaceList(";
    for (int i = 0; i < surfaceList->count(); ++i) {
        if (i > 0) {
            dbg << ", ";
        }
        auto surface = surfaceList->get(i);
        dbg << (void*)surface;
    }
    dbg << ')';
    return dbg;
}
