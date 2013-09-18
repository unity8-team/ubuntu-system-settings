/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: William Hua <william.hua@canonical.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "subset-model.h"

#define SUBSET_ROLE   (Qt::UserRole + 0)
#define SUPERSET_ROLE (Qt::UserRole + 1)
#define ENABLED_ROLE  (Qt::UserRole + 2)
#define DISPLAY_ROLE  (Qt::DisplayRole)
#define CHECKED_ROLE  (Qt::CheckStateRole)

bool
changeLessThan(const SubsetModel::Change *change0,
               const SubsetModel::Change *change1)
{
    return change0->finish < change1->finish;
}

SubsetModel::SubsetModel(QObject *parent) :
    QAbstractListModel(parent),
    _allowEmpty(true),
    _checked(0),
    _ignore(QDateTime::currentMSecsSinceEpoch())
{
}

const QStringList &
SubsetModel::superset() const
{
    return _superset;
}

void
SubsetModel::setSuperset(const QStringList &superset)
{
    if (superset != _superset) {
        beginResetModel();

        for (QList<State *>::iterator i(_state.begin()); i != _state.end(); ++i)
            delete *i;

        _ignore = QDateTime::currentMSecsSinceEpoch();
        _superset = superset;
        _subset.clear();
        _state.clear();
        _checked = 0;

        for (int i(0); i < _superset.length(); i++) {
            State *state(new State);
            state->checked = false;
            state->check = _ignore;
            state->uncheck = _ignore;

            _state += state;
        }

        if (!_allowEmpty && !_superset.isEmpty()) {
            _subset += 0;
            _state[0]->checked = true;
            _checked = 1;
        }

        endResetModel();

        Q_EMIT subsetChanged();
        Q_EMIT supersetChanged();
    }
}

const QList<int> &
SubsetModel::subset() const
{
    return _subset;
}

void
SubsetModel::setSubset(const QList<int> &subset)
{
    if (subset != _subset) {
        beginResetModel();

        _ignore = QDateTime::currentMSecsSinceEpoch();
        _subset.clear();
        _checked = 0;

        for (QList<State *>::iterator i(_state.begin()); i != _state.end(); ++i) {
            (*i)->checked = false;
            (*i)->check = _ignore;
            (*i)->uncheck = _ignore;
        }

        for (QList<int>::const_iterator i(subset.begin()); i != subset.end(); ++i) {
            if (0 <= *i && *i < _superset.length()) {
                _subset += *i;

                if (!_state[*i]->checked) {
                    _state[*i]->checked = true;
                    _checked++;
                }
            }
        }

        if (!_allowEmpty && _checked == 0 && !_superset.isEmpty()) {
            _subset += 0;
            _state[0]->checked = true;
            _checked = 1;
        }

        endResetModel();

        Q_EMIT subsetChanged();
    }
}

bool
SubsetModel::allowEmpty() const
{
    return _allowEmpty;
}

void
SubsetModel::setAllowEmpty(bool allowEmpty)
{
    if (allowEmpty != _allowEmpty) {
        _allowEmpty = allowEmpty;

        if (!_allowEmpty && _checked == 0) {
            _subset += 0;
            _state[0]->checked = true;
            _checked = 1;
        }

        if (_checked == 1) {
            int single(-1);

            for (int i(0); i < _state.length(); i++) {
                if (_state[i]->checked) {
                    single = i;
                    break;
                }
            }

            for (int i(0); i < _subset.length(); i++) {
                if (_subset[i] == single) {
                    QModelIndex row(index(i, 0));
                    Q_EMIT dataChanged(row, row, QVector<int>(1, ENABLED_ROLE));
                }
            }

            if (single >= 0) {
                QModelIndex row(index(_subset.length() + single, 0));
                Q_EMIT dataChanged(row, row, QVector<int>(1, ENABLED_ROLE));
            }
        }

        Q_EMIT allowEmptyChanged();
    }
}

bool
SubsetModel::checked(int element)
{
    return _state[element]->checked;
}

void
SubsetModel::setChecked(int  element,
                        bool checked,
                        int  timeout)
{
    qint64 time(QDateTime::currentMSecsSinceEpoch());

    if (checked)
        _state[element]->check = time;
    else
        _state[element]->uncheck = time;

    if (checked != _state[element]->checked) {
        _state[element]->checked = checked;

        if (checked)
            _checked++;
        else
            _checked--;

        if (!_allowEmpty && (_checked == 1 || (_checked == 2 && checked))) {
            int single(-1);

            for (int i(0); i < _state.length(); i++) {
                if (i != element && _state[i]->checked) {
                    single = i;
                    break;
                }
            }

            for (int i(0); i < _subset.length(); i++) {
                if (_subset[i] == single) {
                    QModelIndex row(index(i, 0));
                    Q_EMIT dataChanged(row, row, QVector<int>(1, ENABLED_ROLE));
                }
            }

            if (single >= 0) {
                QModelIndex row(index(_subset.length() + single, 0));
                Q_EMIT dataChanged(row, row, QVector<int>(1, ENABLED_ROLE));
            }
        }

        for (int i(0); i < _subset.length(); i++) {
            if (_subset[i] == element) {
                QModelIndex row(index(i, 0));
                Q_EMIT dataChanged(row, row, QVector<int>(1, CHECKED_ROLE));
            }
        }

        QModelIndex row(index(_subset.length() + element, 0));
        Q_EMIT dataChanged(row, row, QVector<int>(1, CHECKED_ROLE));

        Change *change(new Change);
        change->element = element;
        change->checked = checked;
        change->start = time;
        change->finish = time + timeout;

        _change.insert(qUpperBound(_change.begin(), _change.end(), change, changeLessThan), change);

        QTimer::singleShot(timeout, this, SLOT(timerExpired()));
    }
}

QHash<int, QByteArray>
SubsetModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;

    roleNames.insert(SUBSET_ROLE, "subset");
    roleNames.insert(SUPERSET_ROLE, "superset");
    roleNames.insert(ENABLED_ROLE, "enabled");
    roleNames.insert(DISPLAY_ROLE, "display");
    roleNames.insert(CHECKED_ROLE, "checked");

    return roleNames;
}

int
SubsetModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return _subset.length() + _superset.length();
}

Qt::ItemFlags
SubsetModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);

    return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
}

QVariant
SubsetModel::data(const QModelIndex &index,
                  int                role) const
{
    switch (role) {
    case SUBSET_ROLE:
    case SUPERSET_ROLE:
        return (role == SUBSET_ROLE) == (index.row() < _subset.length());

    case ENABLED_ROLE:
        return _allowEmpty || _checked != 1 || !_state[elementAtIndex(index)]->checked;

    case DISPLAY_ROLE:
        return _superset[elementAtIndex(index)];

    case CHECKED_ROLE:
        return _state[elementAtIndex(index)]->checked ? Qt::Checked : Qt::Unchecked;
    }

    return QVariant();
}

bool
SubsetModel::setData(const QModelIndex &index,
                     const QVariant    &value,
                     int                role)
{
    switch (role) {
    case CHECKED_ROLE:
        switch (static_cast<QMetaType::Type>(value.type())) {
        case QMetaType::Bool:
        case QMetaType::QChar:
        case QMetaType::Int:
        case QMetaType::UInt:
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
            setChecked(elementAtIndex(index), value.toBool(), 0);

            return true;

        default:
            break;
        }

        break;
    }

    return false;
}

void
SubsetModel::timerExpired()
{
    Change *change(_change.first());

    _change.removeFirst();

    if (change->start > _ignore) {
        if (change->checked) {
            if (change->start > _state[change->element]->uncheck) {
                if (!_subset.contains(change->element)) {
                    beginInsertRows(QModelIndex(), _subset.length(), _subset.length());
                    _subset += change->element;
                    endInsertRows();

                    Q_EMIT subsetChanged();
                }
            }
        }
        else {
            if (change->start > _state[change->element]->check) {
                for (int i(0); i < _subset.length(); i++) {
                    while (i < _subset.length() && _subset[i] == change->element) {
                        beginRemoveRows(QModelIndex(), i, i);
                        _subset.removeAt(i);
                        endRemoveRows();
                    }
                }

                Q_EMIT subsetChanged();
            }
        }
    }

    delete change;
}

int
SubsetModel::elementAtRow(int row) const
{
    return row < _subset.length() ? _subset[row] : row - _subset.length();
}

int
SubsetModel::elementAtIndex(const QModelIndex &index) const
{
    return elementAtRow(index.row());
}
