/*
 * Copyright 2013 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of version 3 of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "fakeprocess.h"
#include <QFile>
#include <QTextStream>
#include <QDir>

namespace UpdatePlugin {

FakeProcess::FakeProcess(QObject *parent) :
    QObject(parent)
{
}

void FakeProcess::start(QString command, QStringList args)
{
    Q_UNUSED(args);
    if(command == "click") {
        QString path = QDir::currentPath();
        path.append("/click.result");

        this->m_content.clear();
        QFile file(path);
        file.open(QIODevice::ReadOnly);
        QTextStream in(&file);
        while(!in.atEnd()) {
            QString line = in.readLine();
            this->m_content.append(line);
        }

        file.close();
        emit this->finished(0);
    }
}

QString FakeProcess::readAllStandardOutput()
{
    return this->m_content;
}

void FakeProcess::startDetached(QString command, QStringList args)
{
    Q_UNUSED(command);
    Q_UNUSED(args);
}

}
