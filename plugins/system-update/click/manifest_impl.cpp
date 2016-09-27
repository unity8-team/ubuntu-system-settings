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

#include "click/manifest_impl.h"
#include "helpers.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

namespace UpdatePlugin
{
namespace Click
{
ManifestImpl::ManifestImpl(QObject *parent)
    : Manifest(parent)
    , m_process()
{
    connect(&m_process, SIGNAL(finished(const int&)),
            this, SLOT(handleProcessSuccess(const int&)));
}

ManifestImpl::~ManifestImpl()
{
    if (m_process.state() != QProcess::NotRunning) {
        m_process.kill();
        m_process.waitForFinished(1);
    }
}

void ManifestImpl::request()
{
    QStringList args("list");
    args << "--manifest";
    QString command = Helpers::whichClick();
    m_process.start(command, args);
    if (!m_process.waitForStarted()) {
        handleProcessError(m_process.error());
    }
}

void ManifestImpl::handleProcessSuccess(const int &exitCode)
{
    Q_UNUSED(exitCode);

    QString output(m_process.readAllStandardOutput());
    auto document = QJsonDocument::fromJson(output.toUtf8());
    if (document.isArray()) {
        Q_EMIT requestSucceeded(document.array());
    } else {
        Q_EMIT requestFailed();
    }
}

void ManifestImpl::handleProcessError(const QProcess::ProcessError &error)
{
    QString err;
    switch (error) {
    case QProcess::FailedToStart:
        err = "Failed to start";
        break;
    case QProcess::Crashed:
        err = "Crashed";
        break;
    case QProcess::Timedout:
        err = "Timed out";
        break;
    case QProcess::WriteError:
        err = "Write error";
        break;
    case QProcess::ReadError:
        err = "Read error";
        break;
    case QProcess::UnknownError:
        err = "Unknown error";
        break;
    }
    qCritical() << Q_FUNC_INFO << "Manifest failed to execute process:" << err;
    Q_EMIT requestFailed();
}
} // Click
} // UpdatePlugin
