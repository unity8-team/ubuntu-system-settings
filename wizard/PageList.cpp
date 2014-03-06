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

/**
 * This class lets the list of wizard pages be dynamic.
 * - To add new ones, drop them into
 *   /usr/share/ubuntu/settings/wizard/qml/Pages with a numbered prefix, like
 *   "21-custom-page.qml".  The number determines the order in the page
 *   sequence that your page will appear.
 * - To disable an existing page, rename it so that it no longer starts with a
 *   number.
 * - To go to the next page, use pageStack.next()
 * - To go back to the previous page, use pageStack.prev()
 * - To load a page outside of the normal flow (so that it doesn't affect the
 *   back button), use pageStack.push(Qt.resolvedUrl("custom-page.qml")) in
 *   your page.
 * - See default pages for plenty of examples.
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
