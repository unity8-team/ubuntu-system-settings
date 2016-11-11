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

#ifndef QTMIR_MIRSURFACELISTMODEL_H
#define QTMIR_MIRSURFACELISTMODEL_H

// unity-api
#include <unity/shell/application/MirSurfaceListInterface.h>

#include <QAbstractListModel>
#include <QList>

namespace qtmir {

class MirSurfaceInterface;
class CombinedSurfaceListModel;

/*
   A list model of MirSurfaces

   Surfaces are ordered from most to least recently raised

   It's possible combine several list models into a new, separate, one which will track
   changes done to those original models and reflect them appropriately.
 */
class MirSurfaceListModel : public unity::shell::application::MirSurfaceListInterface
{
    Q_OBJECT
public:
    explicit MirSurfaceListModel(QObject *parent = 0);
    virtual ~MirSurfaceListModel();

    Q_INVOKABLE unity::shell::application::MirSurfaceInterface *get(int index) override;
    const unity::shell::application::MirSurfaceInterface *get(int index) const;

    // QAbstractItemModel methods
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;

    void prependSurface(MirSurfaceInterface *surface);
    void removeSurface(MirSurfaceInterface *surface);

    // Added surface list models will be tracked for later additions and removals
    void addSurfaceList(MirSurfaceListModel *surfaceList);
    void removeSurfaceList(MirSurfaceListModel *surfaceList);

    bool contains(MirSurfaceInterface *surface) const { return m_surfaceList.contains(surface); }

    bool isEmpty() const;

Q_SIGNALS:
    void emptyChanged();

private:
    void raise(MirSurfaceInterface *surface);
    void moveSurface(int from, int to);
    void connectSurface(MirSurfaceInterface *surface);
    void prependSurfaces(QList<MirSurfaceInterface*> &surfaceList, int prependFirst, int prependLast);

    QList<MirSurfaceInterface*> m_surfaceList;
    QList<MirSurfaceListModel*> m_trackedModels;
};

/*
   Acts as if it were its source list.
 */
class ProxySurfaceListModel : public unity::shell::application::MirSurfaceListInterface
{
    Q_OBJECT
public:
    ProxySurfaceListModel(QObject *parent = nullptr);
    void setSourceList(MirSurfaceListModel *sourceList);

    Q_INVOKABLE unity::shell::application::MirSurfaceInterface *get(int index) override;
    const unity::shell::application::MirSurfaceInterface *get(int index) const;
    // QAbstractItemModel methods
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
private:
    MirSurfaceListModel *m_sourceList{nullptr};
};

} // namespace qtmir

QDebug operator<<(QDebug, const unity::shell::application::MirSurfaceListInterface &);

#endif // QTMIR_MIRSURFACELISTMODEL_H
