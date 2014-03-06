/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
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

#ifndef PAGELIST_H
#define PAGEList_H

#include <QDir>
#include <QStringList>

class PageList : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int index READ index NOTIFY indexChanged)

public:
    explicit PageList(const QDir &dir, QObject *parent = 0);

    int index();

public Q_SLOTS:
    QString prev();
    QString next();

Q_SIGNALS:
    void indexChanged();

private:
    int setIndex(int index);

    QDir m_dir;
    int m_index;
    QStringList m_pages;
};

#endif // PAGELIST_H
