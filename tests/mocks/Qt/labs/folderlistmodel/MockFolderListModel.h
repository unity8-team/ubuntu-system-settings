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

#ifndef MOCK_FOLDERLISTMODEL_H
#define MOCK_FOLDERLISTMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QString>
#include <QUrl>
#include <QStringList>

struct MockFile {
    QString fileName;
    QString filePath;
    QString baseName;
    qint64 size;
    QString suffix;
    bool isDir;
    bool isFile;
};

class MockFolderListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(Sort)
    Q_PROPERTY(QUrl folder READ folder WRITE setFolder NOTIFY folderChanged)
    Q_PROPERTY(bool caseSensitive READ caseSensitive
               WRITE setCaseSensitive NOTIFY caseSensitiveChanged)
    Q_PROPERTY(int count READ count
               WRITE setCount NOTIFY countChanged)
    Q_PROPERTY(QStringList nameFilters READ nameFilters
               WRITE setNameFilters NOTIFY nameFiltersChanged)
    Q_PROPERTY(QUrl parentFolder READ parentFolder
               WRITE setParentFolder NOTIFY parentFolderChanged)
    Q_PROPERTY(QUrl rootFolder READ rootFolder
               WRITE setRootFolder NOTIFY rootFolderChanged)
    Q_PROPERTY(bool showDirs READ showDirs
               WRITE setShowDirs NOTIFY showDirsChanged)
    Q_PROPERTY(bool showDirsFirst READ showDirsFirst
               WRITE setShowDirsFirst NOTIFY showDirsFirstChanged)
    Q_PROPERTY(bool showDotAndDotDot READ showDotAndDotDot
               WRITE setShowDotAndDotDot NOTIFY showDotAndDotDotChanged)
    Q_PROPERTY(bool showFiles READ showFiles
               WRITE setShowFiles NOTIFY showFilesChanged)
    Q_PROPERTY(bool showHidden READ showHidden
               WRITE setShowHidden NOTIFY showHiddenChanged)
    Q_PROPERTY(bool showOnlyReadable READ showOnlyReadable
               WRITE setShowOnlyReadable NOTIFY showOnlyReadableChanged)
    Q_PROPERTY(Sort sortField READ sortField
               WRITE setSortField NOTIFY sortFieldChanged)
    Q_PROPERTY(bool sortReversed READ sortReversed
               WRITE setSortReversed NOTIFY sortReversedChanged)

public:
    explicit MockFolderListModel(QObject *parent = 0);

    enum Roles {
        FileNameRole = Qt::DisplayRole + 1,
        FilePathRole,
        FileURLRole,
        FileBaseNameRole,
        FileSuffixRole,
        FileSizeRole,
        FileModifiedRole,
        FileAccessedRole,
        FileIsDirRole
    };

    enum class Sort : uint
    {
        Unsorted = 0,
        Name,
        Time,
        Size,
        Type
    };

    QUrl folder() const;
    bool caseSensitive() const;
    int count() const;
    QStringList nameFilters() const;
    QUrl parentFolder() const;
    QUrl rootFolder() const;
    bool showDirs() const;
    bool showDirsFirst() const;
    bool showDotAndDotDot() const;
    bool showFiles() const;
    bool showHidden() const;
    bool showOnlyReadable() const;
    Sort sortField() const;
    bool sortReversed() const;

    void setCaseSensitive(const QUrl &folder);
    void setCaseSensitive(const bool caseSensitive);
    void setCount(const int &count);
    void setFolder(const QUrl &folder);
    void setNameFilters(const QStringList &nameFilters);
    void setParentFolder(const QUrl &parentFolder);
    void setRootFolder(const QUrl &rootFolder);
    void setShowDirs(const bool showDirs);
    void setShowDirsFirst(const bool showDirsFirst);
    void setShowDotAndDotDot(const bool showDotAndDotDot);
    void setShowFiles(const bool showFiles);
    void setShowHidden(const bool showHidden);
    void setShowOnlyReadable(const bool showOnlyReadable);
    void setSortField(const Sort &sortField);
    void setSortReversed(const bool sortReversed);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void mockAddFile(const QString &fileName,
                                 const QString &filePath,
                                 const QString &baseName,
                                 const qint64 &size,
                                 const QString &suffix,
                                 const bool isDir,
                                 const bool isFile);

Q_SIGNALS:
    void caseSensitiveChanged();
    void countChanged();
    void folderChanged();
    void nameFiltersChanged();
    void parentFolderChanged();
    void rootFolderChanged();
    void showDirsChanged();
    void showDirsFirstChanged();
    void showDotAndDotDotChanged();
    void showFilesChanged();
    void showHiddenChanged();
    void showOnlyReadableChanged();
    void sortFieldChanged();
    void sortReversedChanged();

private:
    QUrl m_folder = QUrl();
    bool m_caseSensitive = false;
    int m_count = false;
    QStringList m_nameFilters = QStringList();
    QUrl m_parentFolder = QUrl();
    QUrl m_rootFolder = QUrl();
    bool m_showDirs = false;
    bool m_showDirsFirst = false;
    bool m_showDotAndDotDot = false;
    bool m_showFiles = false;
    bool m_showHidden = false;
    bool m_showOnlyReadable = false;
    Sort m_sortField = Sort::Unsorted;
    bool m_sortReversed = false;

    QList<MockFile> m_files;
};

#endif // MOCK_FOLDERLISTMODEL_H
