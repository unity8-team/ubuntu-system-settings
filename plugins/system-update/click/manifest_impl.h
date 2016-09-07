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

#ifndef CLICK_MANIFEST_IMPL_H
#define CLICK_MANIFEST_IMPL_H

#include "click/manifest.h"

#include <QProcess>

namespace UpdatePlugin
{
namespace Click
{
class ManifestImpl : public Manifest
{
    Q_OBJECT
public:
    explicit ManifestImpl(QObject *parent = nullptr);
    ~ManifestImpl();
public Q_SLOTS:
    virtual void request() override;
private Q_SLOTS:
    void handleProcessSuccess(const int &exitCode);
    void handleProcessError(const QProcess::ProcessError &error);
private:
    QProcess m_process;
};
} // Click
} // UpdatePlugin

#endif // CLICK_MANIFEST_IMPL_H
