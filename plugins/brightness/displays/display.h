/*
 * This file is part of system-settings
 *
 * Copyright (C) 2017 Canonical Ltd.
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

#ifndef DISPLAY_H
#define DISPLAY_H

#include "enums.h"
#include "output/output.h"

#include <mir_toolkit/mir_client_library.h>

#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QSharedPointer>
#include <QVariantMap>

namespace DisplayPlugin
{
class Q_DECL_EXPORT Display : public QObject
{
    Q_OBJECT

public:
    explicit Display(QObject *parent = Q_NULLPTR);
    explicit Display(QSharedPointer<Output> output, QObject *parent = Q_NULLPTR);
    explicit Display(const uint &id);
    ~Display() {};

    int id() const;
    QString name() const;
    QString type() const;
    bool mirrored() const;
    bool connected() const;
    bool enabled() const;
    QSharedPointer<OutputMode> mode() const;
    QList<QSharedPointer<OutputMode>> availableModes() const;
    Enums::Orientation orientation() const;
    float scale() const;
    bool uncommittedChanges() const;
    uint physicalWidthMm() const;
    uint physicalHeightMm() const;
    Enums::PowerMode powerMode() const;

    void setMirrored(const bool &mirrored);
    void setEnabled(const bool &enabled);
    void setMode(const QSharedPointer<OutputMode> &mode);
    void setOrientation(const Enums::Orientation &orientation);
    void setScale(const float &scale);

Q_SIGNALS:
    void mirroredChanged();
    void connectedChanged();
    void enabledChanged();
    void modeChanged();
    void modesChanged();
    void orientationChanged();
    void scaleChanged();
    void uncommittedChangesChanged();
    void displayChanged(const Display *display);

protected:
    void setUncommitedChanges(const bool uncommittedChanges);
    void storeConfiguration();

    QString m_name = QString::null;
    bool m_mirrored = false;
    bool m_uncommittedChanges = false;
    QSharedPointer<Output> m_output = QSharedPointer<Output>(Q_NULLPTR);

protected slots:
    void changedSlot();

private:
    void initialize();
    bool hasChanged() const;

    QVariantMap m_storedConfig;
};

} // DisplayPlugin

#endif // DISPLAY_H
