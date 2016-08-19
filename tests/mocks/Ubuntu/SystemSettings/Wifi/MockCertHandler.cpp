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

#include "MockCertHandler.h"

MockAbstractListModel::MockAbstractListModel(QObject *parent)
{
}

QHash<int, QByteArray> MockAbstractListModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[CNRole] = "CommonName";
    roles[ORole] = "Organization";
    roles[expDateRole] = "expiryDate";

    roles[keyName] = "KeyName";
    roles[keyType] = "KeyType";
    roles[keyAlgorithm] = "KeyAlgorithm";
    roles[keyLength] = "KeyLength";

    roles[pacFileName] = "pacFileName";

    return roles;
}

int MockAbstractListModel::rowCount(const QModelIndex &parent) const
{
    return m_files.size();
}

QString MockAbstractListModel::getfileName(const int selectedIndex) const
{
    return m_files[selectedIndex].fileName;
}

void MockAbstractListModel::dataupdate()
{
}

QVariant MockAbstractListModel::data(const QModelIndex &index, int role) const
{
    auto ret = QVariant();
    switch (role) {
    case CNRole:
        return m_files[index.row()].commonName;
    case ORole:
        return m_files[index.row()].organization;
    case expDateRole:
        return m_files[index.row()].expiryDate;
    case keyName:
        return m_files[index.row()].keyName;
    case keyType:
        return m_files[index.row()].keyType;
    case keyAlgorithm:
        return m_files[index.row()].keyAlgorithm;
    case keyLength:
        return m_files[index.row()].keyLength;
    case pacFileName:
        return m_files[index.row()].pacFileName;
    }
}

QByteArray MockFileHandler::getCertContent(QString filename)
{
    return QByteArray();
}

QString MockFileHandler::moveCertFile(QString filename)
{
    return QString();
}

QString MockFileHandler::moveKeyFile(QString filename)
{
    return QString();
}

QString MockFileHandler::movePacFile(QString filename)
{
    return QString();
}

bool MockFileHandler::removeFile(QString filename)
{
    return false;
}
