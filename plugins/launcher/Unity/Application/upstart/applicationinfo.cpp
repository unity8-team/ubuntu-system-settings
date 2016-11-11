/*
 * Copyright (C) 2014-2016 Canonical, Ltd.
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

#include "applicationinfo.h"

namespace qtmir
{
namespace upstart
{

ApplicationInfo::ApplicationInfo(const QString &appId, std::shared_ptr<ubuntu::app_launch::Application::Info> info)
    : qtmir::ApplicationInfo(),
      m_appId(appId),
      m_info(info)
{
}

QString ApplicationInfo::appId() const
{
    return m_appId;
}

QString ApplicationInfo::name() const
{
    return QString::fromStdString(m_info->name().value());
}

QString ApplicationInfo::comment() const
{
    return QString::fromStdString(m_info->description().value());
}

QUrl ApplicationInfo::icon() const
{
    return QUrl::fromLocalFile(QString::fromStdString(m_info->iconPath().value()));
}

QString ApplicationInfo::splashTitle() const
{
    return QString::fromStdString(m_info->splash().title.value());
}

QUrl ApplicationInfo::splashImage() const
{
    return QUrl::fromLocalFile(QString::fromStdString(m_info->splash().image.value()));
}

bool ApplicationInfo::splashShowHeader() const
{
    return m_info->splash().showHeader.value();
}

QString ApplicationInfo::splashColor() const
{
    return QString::fromStdString(m_info->splash().backgroundColor.value());
}

QString ApplicationInfo::splashColorHeader() const
{
    return QString::fromStdString(m_info->splash().headerColor.value());
}

QString ApplicationInfo::splashColorFooter() const
{
    return QString::fromStdString(m_info->splash().footerColor.value());
}

Qt::ScreenOrientations ApplicationInfo::supportedOrientations() const
{
    Qt::ScreenOrientations response = 0;
    auto orientations = m_info->supportedOrientations();
    if (orientations.portrait)
        response |= Qt::PortraitOrientation;
    if (orientations.landscape)
        response |= Qt::LandscapeOrientation;
    if (orientations.invertedPortrait)
        response |= Qt::InvertedPortraitOrientation;
    if (orientations.invertedLandscape)
        response |= Qt::InvertedLandscapeOrientation;
    return response;
}

bool ApplicationInfo::rotatesWindowContents() const
{
    return m_info->rotatesWindowContents().value();
}


bool ApplicationInfo::isTouchApp() const
{
    return m_info->supportsUbuntuLifecycle().value();
}

} // namespace upstart
} // namespace qtmir
