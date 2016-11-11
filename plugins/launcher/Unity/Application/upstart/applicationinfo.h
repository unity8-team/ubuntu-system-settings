/*
 * Copyright (C) 2013-2016 Canonical, Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3, as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranties of MERCHANTABILITY,
 * SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef UPSTART_APPLICATION_INFO_H
#define UPSTART_APPLICATION_INFO_H

#include "../applicationinfo.h"

#include <ubuntu-app-launch/application.h>

namespace qtmir
{
namespace upstart
{

class ApplicationInfo : public qtmir::ApplicationInfo
{
public:
    ApplicationInfo(const QString &appId, std::shared_ptr<ubuntu::app_launch::Application::Info> info);

    QString appId() const override;
    QString name() const override;
    QString comment() const override;
    QUrl icon() const override;
    QString splashTitle() const override;
    QUrl splashImage() const override;
    bool splashShowHeader() const override;
    QString splashColor() const override;
    QString splashColorHeader() const override;
    QString splashColorFooter() const override;
    Qt::ScreenOrientations supportedOrientations() const override;
    bool rotatesWindowContents() const override;
    bool isTouchApp() const override;

private:
    QString m_appId;
    std::shared_ptr<ubuntu::app_launch::Application::Info> m_info;
};

} // namespace upstart
} // namespace qtmir

#endif // UPSTART_APPLICATION_INFO_H
