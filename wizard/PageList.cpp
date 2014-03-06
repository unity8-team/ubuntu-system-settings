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

#include "PageList.h"

PageList::PageList(const QDir &dir, QObject *parent)
    : QObject(parent),
      m_dir(dir),
      m_index(-1),
      m_pages()
{
    QStringList filters("[0-9]*");
    m_pages = dir.entryList(filters, QDir::Files | QDir::Readable, QDir::Name);
}

int PageList::index()
{
    return m_index;
}

QString PageList::prev()
{
    if (m_index > 0)
        return m_dir.absoluteFilePath(m_pages[setIndex(m_index - 1)]);
    else
        return QString();
}

QString PageList::next()
{
    if (m_index < m_pages.length() - 1)
        return m_dir.absoluteFilePath(m_pages[setIndex(m_index + 1)]);
    else
        return QString();
}

int PageList::setIndex(int index)
{
    m_index = index;
    Q_EMIT indexChanged();
    return m_index;
}
