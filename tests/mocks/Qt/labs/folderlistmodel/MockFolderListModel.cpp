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

#include "MockFolderListModel.h"

#include <QDateTime>

MockFolderListModel::MockFolderListModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_files()
{
}

QUrl MockFolderListModel::folder() const { return m_folder; }
bool MockFolderListModel::caseSensitive() const { return m_caseSensitive; }
int MockFolderListModel::count() const { return m_count; }
QStringList MockFolderListModel::nameFilters() const { return m_nameFilters; }
QUrl MockFolderListModel::parentFolder() const { return m_parentFolder; }
QUrl MockFolderListModel::rootFolder() const { return m_rootFolder; }
bool MockFolderListModel::showDirs() const { return m_showDirs; }
bool MockFolderListModel::showDirsFirst() const { return m_showDirsFirst; }
bool MockFolderListModel::showDotAndDotDot() const { return m_showDotAndDotDot; }
bool MockFolderListModel::showFiles() const { return m_showFiles; }
bool MockFolderListModel::showHidden() const { return m_showHidden; }
bool MockFolderListModel::showOnlyReadable() const { return m_showOnlyReadable; }
MockFolderListModel::Sort MockFolderListModel::sortField() const { return m_sortField; }
bool MockFolderListModel::sortReversed() const { return m_sortReversed; }

void MockFolderListModel::setCaseSensitive(const bool caseSensitive)
{
    if (caseSensitive != m_caseSensitive) {
        m_caseSensitive = caseSensitive;
        Q_EMIT caseSensitiveChanged();
    }
}

void MockFolderListModel::setCount(const int &count)
{
    if (count != m_count) {
        m_count = count;
        Q_EMIT countChanged();
    }
}

void MockFolderListModel::setFolder(const QUrl &folder)
{
    if (folder != m_folder) {
        m_folder = folder;
        Q_EMIT folderChanged();
    }
}

void MockFolderListModel::setNameFilters(const QStringList &nameFilters)
{
    if (nameFilters != m_nameFilters) {
        m_nameFilters = nameFilters;
        Q_EMIT nameFiltersChanged();
    }
}

void MockFolderListModel::setParentFolder(const QUrl &parentFolder)
{
    if (parentFolder != m_parentFolder) {
        m_parentFolder = parentFolder;
        Q_EMIT parentFolderChanged();
    }
}

void MockFolderListModel::setRootFolder(const QUrl &rootFolder)
{
    if (rootFolder != m_rootFolder) {
        m_rootFolder = rootFolder;
        Q_EMIT rootFolderChanged();
    }
}

void MockFolderListModel::setShowDirs(const bool showDirs)
{
    if (showDirs != m_showDirs) {
        m_showDirs = showDirs;
        Q_EMIT showDirsChanged();
    }
}

void MockFolderListModel::setShowDirsFirst(const bool showDirsFirst)
{
    if (showDirsFirst != m_showDirsFirst) {
        m_showDirsFirst = showDirsFirst;
        Q_EMIT showDirsFirstChanged();
    }
}

void MockFolderListModel::setShowDotAndDotDot(const bool showDotAndDotDot)
{
    if (showDotAndDotDot != m_showDotAndDotDot) {
        m_showDotAndDotDot = showDotAndDotDot;
        Q_EMIT showDotAndDotDotChanged();
    }
}

void MockFolderListModel::setShowFiles(const bool showFiles)
{
    if (showFiles != m_showFiles) {
        m_showFiles = showFiles;
        Q_EMIT showFilesChanged();
    }
}

void MockFolderListModel::setShowHidden(const bool showHidden)
{
    if (showHidden != m_showHidden) {
        m_showHidden = showHidden;
        Q_EMIT showHiddenChanged();
    }
}

void MockFolderListModel::setShowOnlyReadable(const bool showOnlyReadable)
{
    if (showOnlyReadable != m_showOnlyReadable) {
        m_showOnlyReadable = showOnlyReadable;
        Q_EMIT showOnlyReadableChanged();
    }
}

void MockFolderListModel::setSortField(const Sort &sortField)
{
    if (sortField != m_sortField) {
        m_sortField = sortField;
        Q_EMIT sortFieldChanged();
    }
}

void MockFolderListModel::setSortReversed(const bool sortReversed)
{
    if (sortReversed != m_sortReversed) {
        m_sortReversed = sortReversed;
        Q_EMIT sortReversedChanged();
    }
}

QHash<int, QByteArray> MockFolderListModel::roleNames() const
{
    QHash<int, QByteArray> names;

    names[Roles::FileNameRole] = "fileName";
    names[Roles::FilePathRole] = "filePath";
    names[Roles::FileURLRole] = "fileURL";
    names[Roles::FileBaseNameRole] = "fileBaseName";
    names[Roles::FileSuffixRole] = "fileSuffix";
    names[Roles::FileSizeRole] = "fileSize";
    names[Roles::FileModifiedRole] = "fileModified";
    names[Roles::FileAccessedRole] = "fileAccessed";
    names[Roles::FileIsDirRole] = "fileIsDir";

    return names;
}


int MockFolderListModel::rowCount(const QModelIndex&) const
{
    return m_files.count();
}

QVariant MockFolderListModel::data(const QModelIndex &index, int role) const
{
    QVariant rv;

    if (index.row() >= m_files.size())
        return rv;

    switch (role)
    {
        case Roles::FileNameRole:
            rv = m_files.at(index.row()).fileName;
            break;
        case Roles::FilePathRole:
            rv = m_files.at(index.row()).filePath;
            break;
        case Roles::FileBaseNameRole:
            rv = m_files.at(index.row()).baseName;
            break;
        case Roles::FileSuffixRole:
            rv = m_files.at(index.row()).suffix;
            break;
        case Roles::FileSizeRole:
            rv = m_files.at(index.row()).size;
            break;
        case Roles::FileModifiedRole:
            rv = QDateTime();
            break;
        case Roles::FileAccessedRole:
            rv = QDateTime();
            break;
        case Roles::FileIsDirRole:
            rv = m_files.at(index.row()).isDir;
            break;
        case Roles::FileURLRole:
            rv = QUrl::fromLocalFile(m_files.at(index.row()).filePath);
            break;
        default:
            break;
    }
    return rv;
}

QModelIndex MockFolderListModel::index(int row, int column, const QModelIndex&) const
{
    return createIndex(row, column);
}

void MockFolderListModel::mockAddFile(const QString &fileName,
                                      const QString &filePath,
                                      const QString &baseName,
                                      const qint64 &size,
                                      const QString &suffix,
                                      const bool isDir,
                                      const bool isFile)
{
    MockFile m;
    m.fileName = fileName;
    m.filePath = filePath;
    m.baseName = baseName;
    m.size = size;
    m.suffix = suffix;
    m.isDir = isDir;
    m.isFile = isFile;
    m_files.append(m);
}
