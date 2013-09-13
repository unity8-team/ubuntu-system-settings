/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: William Hua <william.hua@canonical.com>
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

#ifndef KEYBOARD_LAYOUT_H
#define KEYBOARD_LAYOUT_H

#include <QtCore>

class KeyboardLayout : public QObject
{
private:

    Q_OBJECT

    Q_PROPERTY(QString name
               READ name
               CONSTANT)

    Q_PROPERTY(QString language
               READ language
               CONSTANT)

    Q_PROPERTY(QString displayName
               READ displayName
               CONSTANT)

public:

    explicit KeyboardLayout(const QString &name        = QString(),
                            const QString &language    = QString(),
                            const QString &displayName = QString(),
                            QObject       *parent      = NULL);

    explicit KeyboardLayout(const QFileInfo &fileInfo,
                            QObject         *parent   = NULL);

    const QString &name() const;
    const QString &language() const;
    const QString &displayName() const;

private:

    QString _name;
    QString _language;
    QString _displayName;
};

#endif // KEYBOARD_LAYOUT_H
