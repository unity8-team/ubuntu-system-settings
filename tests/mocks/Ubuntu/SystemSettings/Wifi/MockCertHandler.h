/*
 * This file is part of system-settings
 *
 * Copyright (C) 2016 Canonical Ltd.
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

#ifndef MOCK_CERTHANDLER_H
#define MOCK_CERTHANDLER_H

#include <QAbstractListModel>
#include <QObject>
#include <QList>

struct MockFile {
    QString fileName;
    QString commonName;
    QString organization;
    QString expiryDate; // dd.mm.yyyy

    QString keyName;
    QString keyType;
    QString keyAlgorithm;
    uint keyLength;

    QString pacFileName;
};

class MockAbstractListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        CNRole = Qt::UserRole + 1,
        ORole,
        expDateRole,

        keyName,
        keyType,
        keyAlgorithm,
        keyLength,

        pacFileName
    };
    explicit MockAbstractListModel(QObject *parent = 0);
    ~MockAbstractListModel() {};
    QHash<int, QByteArray> roleNames() const;
    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE QString  getfileName(const int selectedIndex) const;
    Q_INVOKABLE void dataupdate();
    QVariant data(const QModelIndex &index, int role) const;
    QList<MockFile> m_files = QList<MockFile>();
};

class MockFileHandler : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE QByteArray getCertContent(QString filename);
    Q_INVOKABLE QString moveCertFile(QString filename);
    Q_INVOKABLE QString moveKeyFile(QString filename);
    Q_INVOKABLE QString movePacFile(QString filename);
    Q_INVOKABLE bool removeFile(QString filename);
};

class MockCertificateListModel : public MockAbstractListModel
{
    Q_OBJECT
};

class MockPrivatekeyListModel : public MockAbstractListModel
{
    Q_OBJECT
};

class MockPacFileListModel : public MockAbstractListModel
{
    Q_OBJECT
};

#endif // MOCK_CERTHANDLER_H
