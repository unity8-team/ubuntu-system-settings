#include "displaymodel.h"

#include <QDebug>

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

    if ((0 <= index.row()) && (index.row() < m_displays.size())) {

        auto display = m_displays[index.row()];

        switch (role) {
        case Qt::DisplayRole:
            ret = display->name();
            break;
        case TypeRole:
            ret = display->type();
            break;
        case MirroredRole:
            ret = display->mirrored();
            break;
        case ConnectedRole:
            ret = display->connected();
            break;
        case EnabledRole:
            ret = display->enabled();
            break;
        case ModeRole:
            ret = display->mode();
            break;
        case ModesRole:
            ret = display->modes();
            break;
        case OrientationRole:
            ret = (uint) display->orientation();
            break;
        case ScaleRole:
            ret = display->scale();
            break;
        case UncommittedChangesRole:
            ret = display->uncommittedChanges();
            break;
        }
    }

    return ret;
}

bool DisplayModel::setData(const QModelIndex &index, const QVariant &value,
                           int role)
{
    if ((0 <= index.row()) && (index.row() < m_displays.size())) {
        auto display = m_displays[index.row()];

        switch (role) {
        case MirroredRole:
            display->setMirrored(value.toBool());
            break;
        case EnabledRole:
            display->setEnabled(value.toBool());
            break;
        case ModeRole:
            display->setMode(value.toUInt());
            break;
        case OrientationRole:
            display->setOrientation((Display::Orientation) value.toUInt());
            break;
        case ScaleRole:
            display->setScale(value.toInt());
            break;
        case Qt::DisplayRole:
        case TypeRole:
        case ConnectedRole:
        case ModesRole:
        case UncommittedChangesRole:
        default:
            return false;
        }
        return true;
    } else {
        return false;
    }
}


QHash<int,QByteArray> DisplayModel::roleNames() const
{
    static QHash<int,QByteArray> names;
    if (Q_UNLIKELY(names.empty())) {
        names[Qt::DisplayRole] = "displayName";
        names[MirroredRole] = "mirrored";
        names[ConnectedRole] = "connected";
        names[EnabledRole] = "enabled";
        names[ModeRole] = "mode";
        names[ModesRole] = "modes";
        names[OrientationRole] = "orientation";
        names[ScaleRole] = "scale";
        names[UncommittedChangesRole] = "uncommittedChanges";
    }
    return names;
}

void DisplayModel::addDisplay(const QSharedPointer<Display> &display)
{
    int row = findRowFromId(display->id());

    if (row >= 0) { // update existing display
        m_displays[row] = display;
        emitRowChanged(row);
    } else { // add new display
        row = m_displays.size();
        beginInsertRows(QModelIndex(), row, row);
        m_displays.append(display);
        endInsertRows();
    }

    if (display) {
        QObject::connect(display.data(), SIGNAL(displayChanged(const Display*)),
                         this, SLOT(displayChangedSlot(const Display*)));
    }

    Q_EMIT countChanged();
}

void DisplayModel::emitRowChanged(const int &row)
{
    if (0 <= row && row < m_displays.size()) {
        QModelIndex qmi = index(row, 0);
        Q_EMIT dataChanged(qmi, qmi);
    }
}

void DisplayModel::displayChangedSlot(const Display *display)
{
    // find the row that goes with this display
    int row = -1;
    if (display != nullptr)
        for (int i = 0, n = m_displays.size(); row == -1 && i < n; i++)
            if (m_displays[i].data() == display)
                row = i;

    if (row != -1)
        emitRowChanged(row);
}

QSharedPointer<Display> DisplayModel::getById(const uint &id)
{
    Q_FOREACH(auto display, m_displays) {
        if (display->id() == id)
            return display;
    }
    return QSharedPointer<Display>(nullptr);
}

int DisplayModel::findRowFromId(const uint &id)
{
    for (int i = 0; i < m_displays.size(); i++) {
        if (m_displays[i]->id() == id)
            return i;
    }
    return -1;
}

DisplaysFilter::DisplaysFilter()
{
    connect(this, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
            this, SLOT(rowsChanged(const QModelIndex&, int, int)));
    connect(this, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
            this, SLOT(rowsChanged(const QModelIndex&, int, int)));
}

bool DisplaysFilter::lessThan(const QModelIndex &left,
                              const QModelIndex &right) const
{
    const QString a = sourceModel()->data(left, Qt::DisplayRole).value<QString>();
    const QString b = sourceModel()->data(right, Qt::DisplayRole).value<QString>();
    return a < b;
}

void DisplaysFilter::filterOnUncommittedChanges(const bool uncommitted)
{
    m_uncommittedChanges = uncommitted;
    m_uncommittedChangesEnabled = true;
    invalidateFilter();
}

void DisplaysFilter::filterOnConnected(const bool connected)
{
    m_connected = connected;
    m_connectedEnabled = true;
    invalidateFilter();
}

bool DisplaysFilter::filterAcceptsRow(int sourceRow,
                                      const QModelIndex &sourceParent) const
{
    bool accepts = true;
    QModelIndex childIndex = sourceModel()->index(sourceRow, 0, sourceParent);

    if (accepts && m_uncommittedChangesEnabled) {
        const bool uncommittedChanges = childIndex.model()->data(
            childIndex, DisplayModel::UncommittedChangesRole
        ).value<bool>();
        accepts = (m_uncommittedChanges == uncommittedChanges);
    }

    if (accepts && m_connectedEnabled) {
        const bool connected = childIndex.model()->data(
            childIndex, DisplayModel::ConnectedRole
        ).value<bool>();
        accepts = (m_connected == connected);
    }

    return accepts;
}

void DisplaysFilter::rowsChanged(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)
    Q_UNUSED(first)
    Q_UNUSED(last)
    Q_EMIT countChanged();
}
