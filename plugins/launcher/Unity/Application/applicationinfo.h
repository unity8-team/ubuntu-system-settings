/*
 * Copyright (C) 2014-2015 Canonical, Ltd.
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

#ifndef APPLICATION_INFO_H
#define APPLICATION_INFO_H

#include <QColor>
#include <QObject>
#include <QString>
#include <QUrl>

namespace qtmir
{

class ApplicationInfo : public QObject
{
    Q_OBJECT

public:
    ApplicationInfo(const ApplicationInfo&) = delete;
    virtual ~ApplicationInfo() = default;

    ApplicationInfo& operator=(const ApplicationInfo&) = delete;

    virtual QString appId() const = 0;
    virtual QString name() const = 0;
    virtual QString comment() const = 0;
    virtual QUrl icon() const = 0;
    virtual QString splashTitle() const = 0;
    virtual QUrl splashImage() const = 0;
    virtual bool splashShowHeader() const = 0;
    virtual QString splashColor() const = 0;
    virtual QString splashColorHeader() const = 0;
    virtual QString splashColorFooter() const = 0;
    virtual Qt::ScreenOrientations supportedOrientations() const = 0;
    virtual bool rotatesWindowContents() const = 0;
    virtual bool isTouchApp() const = 0;

protected:
    ApplicationInfo() = default;
};

} // namespace qtmir

#endif // APPLICATION_INFO_H
