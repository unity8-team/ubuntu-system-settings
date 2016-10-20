#include "displaymodel.h"

DisplayModel::DisplayModel(QObject *parent)
{
}

DisplayModel::~DisplayModel()
{
}

int DisplayModel::rowCount(const QModelIndex &parent) const
{
    return m_displays.count();
}

QVariant DisplayModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;

    if ((0<=index.row()) && (index.row() < m_displays.size())) {

        auto device = m_displays[index.row()];
        QString displayName;

        switch (role) {
        case Qt::DisplayRole:
            ret = device->name();
            break;
        case TypeRole:
            ret = device->type();
            break;
        case ResolutionRole:
            ret = device->resolution();
            break;
        case MirroredRole:
            ret = device->mirrored();
            break;
        case RotationRole:
            ret = device->rotation();
            break;
        case StateRole:
            ret = device->state();
            break;
        }
    }

    return ret;
}

QHash<int,QByteArray> DisplayModel::roleNames() const
{
    static QHash<int,QByteArray> names;
    if (Q_UNLIKELY(names.empty())) {
        names[Qt::DisplayRole] = "displayName";
        names[TypeRole] = "type";
        names[ResolutionRole] = "resolution";
        names[MirroredRole] = "mirrored";
        names[RotationRole] = "rotation";
        names[StateRole] = "state";
    }
    return names;
}
