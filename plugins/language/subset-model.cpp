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

#define CHECKED_ROLE  (Qt::CheckStateRole)
#define ENABLED_ROLE  (Qt::UserRole + 0)
#define SUBSET_ROLE   (Qt::UserRole + 1)
#define SUPERSET_ROLE (Qt::UserRole + 2)
#define DISPLAY_ROLE  (Qt::UserRole + 3)
#define CUSTOM_ROLE   (Qt::UserRole + 4)

bool
changeLessThan(const SubsetModel::Change *change0,
               const SubsetModel::Change *change1)
{
    return change0->finish < change1->finish;
}

SubsetModel::SubsetModel(QObject *parent) :
    QAbstractListModel(parent),
    m_allowEmpty(true),
    m_checked(0),
    m_ignore(QDateTime::currentMSecsSinceEpoch())
{
}

const QStringList &
SubsetModel::customRoles() const
{
    return m_customRoles;
}

void
SubsetModel::setCustomRoles(const QStringList &customRoles)
{
    if (customRoles != m_customRoles) {
        m_customRoles = customRoles;

        Q_EMIT customRolesChanged();
    }
}

const QVariantList &
SubsetModel::superset() const
{
    return m_superset;
}

void
SubsetModel::setSuperset(const QVariantList &superset)
{
    if (superset != m_superset) {
        beginResetModel();

        for (QList<State *>::iterator i(m_state.begin()); i != m_state.end(); ++i)
            delete *i;

        m_ignore = QDateTime::currentMSecsSinceEpoch();
        m_superset = superset;
        m_subset.clear();
        m_state.clear();
        m_checked = 0;

        for (int i(0); i < m_superset.length(); i++) {
            State *state(new State);
            state->checked = false;
            state->check = m_ignore;
            state->uncheck = m_ignore;

            m_state += state;
        }

        if (!m_allowEmpty && !m_superset.isEmpty()) {
            m_subset += 0;
            m_state[0]->checked = true;
            m_checked = 1;
        }

        endResetModel();

        Q_EMIT subsetChanged();
        Q_EMIT supersetChanged();
    }
}

const QList<int> &
SubsetModel::subset() const
{
    return m_subset;
}

void
SubsetModel::setSubset(const QList<int> &subset)
{
    if (subset != m_subset) {
        beginResetModel();

        m_ignore = QDateTime::currentMSecsSinceEpoch();
        m_subset.clear();
        m_checked = 0;

        for (QList<State *>::iterator i(m_state.begin()); i != m_state.end(); ++i) {
            (*i)->checked = false;
            (*i)->check = m_ignore;
            (*i)->uncheck = m_ignore;
        }

        for (QList<int>::const_iterator i(subset.begin()); i != subset.end(); ++i) {
            if (0 <= *i && *i < m_superset.length()) {
                m_subset += *i;

                if (!m_state[*i]->checked) {
                    m_state[*i]->checked = true;
                    m_checked++;
                }
            }
        }

        if (!m_allowEmpty && m_checked == 0 && !m_superset.isEmpty()) {
            m_subset += 0;
            m_state[0]->checked = true;
            m_checked = 1;
        }

        endResetModel();

        Q_EMIT subsetChanged();
    }
}

bool
SubsetModel::allowEmpty() const
{
    return m_allowEmpty;
}

void
SubsetModel::setAllowEmpty(bool allowEmpty)
{
    if (allowEmpty != m_allowEmpty) {
        m_allowEmpty = allowEmpty;

        // Check the first element if we can't have an empty subset.
        if (!m_allowEmpty && m_state.length() > 0 && m_checked == 0) {
            m_subset += 0;
            m_state[0]->checked = true;
            m_checked = 1;
        }

        if (m_checked == 1) {
            int single(-1);

            for (int i(0); i < m_state.length(); i++) {
                if (m_state[i]->checked) {
                    single = i;
                    break;
                }
            }

            for (int i(0); i < m_subset.length(); i++) {
                if (m_subset[i] == single) {
                    QModelIndex row(index(i, 0));
                    Q_EMIT dataChanged(row, row, QVector<int>(1, ENABLED_ROLE));
                }
            }

            if (single >= 0) {
                QModelIndex row(index(m_subset.length() + single, 0));
                Q_EMIT dataChanged(row, row, QVector<int>(1, ENABLED_ROLE));
            }
        }

        Q_EMIT allowEmptyChanged();
    }
}

bool
SubsetModel::checked(int element)
{
    return m_state[element]->checked;
}

void
SubsetModel::setChecked(int  element,
                        bool checked,
                        int  timeout)
{
    qint64 time(QDateTime::currentMSecsSinceEpoch());

    if (checked)
        m_state[element]->check = time;
    else
        m_state[element]->uncheck = time;

    if (checked != m_state[element]->checked) {
        m_state[element]->checked = checked;

        if (checked)
            m_checked++;
        else
            m_checked--;

        if (!m_allowEmpty && (m_checked == 1 || (m_checked == 2 && checked))) {
            int single(-1);

            for (int i(0); i < m_state.length(); i++) {
                if (i != element && m_state[i]->checked) {
                    single = i;
                    break;
                }
            }

            for (int i(0); i < m_subset.length(); i++) {
                if (m_subset[i] == single) {
                    QModelIndex row(index(i, 0));
                    Q_EMIT dataChanged(row, row, QVector<int>(1, ENABLED_ROLE));
                }
            }

            if (single >= 0) {
                QModelIndex row(index(m_subset.length() + single, 0));
                Q_EMIT dataChanged(row, row, QVector<int>(1, ENABLED_ROLE));
            }
        }

        for (int i(0); i < m_subset.length(); i++) {
            if (m_subset[i] == element) {
                QModelIndex row(index(i, 0));
                Q_EMIT dataChanged(row, row, QVector<int>(1, CHECKED_ROLE));
            }
        }

        QModelIndex row(index(m_subset.length() + element, 0));
        Q_EMIT dataChanged(row, row, QVector<int>(1, CHECKED_ROLE));

        Change *change(new Change);
        change->element = element;
        change->checked = checked;
        change->start = time;
        change->finish = time + timeout;

        m_change.insert(qUpperBound(m_change.begin(), m_change.end(), change, changeLessThan), change);

        QTimer::singleShot(timeout, this, SLOT(timerExpired()));
    }
}

QHash<int, QByteArray>
SubsetModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;

    roleNames.insert(CHECKED_ROLE, "checked");
    roleNames.insert(ENABLED_ROLE, "enabled");
    roleNames.insert(SUBSET_ROLE, "subset");
    roleNames.insert(SUPERSET_ROLE, "superset");
    roleNames.insert(DISPLAY_ROLE, "display");

    for (int i(0); i < m_customRoles.length(); i++)
        roleNames.insert(CUSTOM_ROLE + i, m_customRoles[i].toUtf8());

    return roleNames;
}

int
SubsetModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_subset.length() + m_superset.length();
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
    case CHECKED_ROLE:
        return m_state[elementAtIndex(index)]->checked ? Qt::Checked : Qt::Unchecked;

    case ENABLED_ROLE:
        return m_allowEmpty || m_checked != 1 || !m_state[elementAtIndex(index)]->checked;

    case SUBSET_ROLE:
    case SUPERSET_ROLE:
        return (role == SUBSET_ROLE) == (index.row() < m_subset.length());

    case DISPLAY_ROLE:
        role = CUSTOM_ROLE;
        break;
    }

    int column(role - CUSTOM_ROLE);
    int element(elementAtIndex(index));
    QVariantList list(m_superset[element].toList());

    if (0 <= column && column < list.length())
        return list[column];

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
    Change *change(m_change.first());

    m_change.removeFirst();

    if (change->start > m_ignore) {
        if (change->checked) {
            if (change->start > m_state[change->element]->uncheck) {
                if (!m_subset.contains(change->element)) {
                    beginInsertRows(QModelIndex(), m_subset.length(), m_subset.length());
                    m_subset += change->element;
                    endInsertRows();

                    Q_EMIT subsetChanged();
                }
            }
        } else {
            if (change->start > m_state[change->element]->check) {
                for (int i(0); i < m_subset.length(); i++) {
                    while (i < m_subset.length() && m_subset[i] == change->element) {
                        beginRemoveRows(QModelIndex(), i, i);
                        m_subset.removeAt(i);
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
    return row < m_subset.length() ? m_subset[row] : row - m_subset.length();
}

int
SubsetModel::elementAtIndex(const QModelIndex &index) const
{
    return elementAtRow(index.row());
}
