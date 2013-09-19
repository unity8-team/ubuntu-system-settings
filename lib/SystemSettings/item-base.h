/*
 * This file is part of system-settings
 *
 * Copyright (C) 2013 Canonical Ltd.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@canonical.com>
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

#ifndef SYSTEM_SETTINGS_ITEM_BASE_H
#define SYSTEM_SETTINGS_ITEM_BASE_H

#include <QObject>
#include <QQmlComponent>
#include <QVariantMap>

namespace SystemSettings {

extern const QLatin1String keyName;
extern const QLatin1String keyIcon;
extern const QLatin1String keyCategory;
extern const QLatin1String keyPriority;
extern const QLatin1String keyTranslations;
extern const QLatin1String keyFormFactors;
extern const QLatin1String keyKeywords;
extern const QLatin1String keyPlugin;
extern const QLatin1String keyEntryComponent;
extern const QLatin1String keyPageComponent;
extern const QLatin1String keyHasDynamicKeywords;
extern const QLatin1String keyHasDynamicVisibility;
extern const QLatin1String keyHideByDefault;

class ItemBasePrivate;
class ItemBase: public QObject
{
    Q_OBJECT

public:
    ItemBase(const QVariantMap &staticData, QObject *parent = 0);
    ~ItemBase();

    QUrl icon() const;
    QStringList keywords() const;
    bool isVisible() const;
    virtual QQmlComponent *entryComponent(QQmlEngine *engine,
                                          QObject *parent = 0);
    virtual QQmlComponent *pageComponent(QQmlEngine *engine,
                                         QObject *parent = 0);

protected:
    void setIcon(const QUrl &icon);
    void setKeywords(const QStringList &keywords);
    void setVisible(bool visible);
    const QVariantMap &staticData() const;

Q_SIGNALS:
    void iconChanged();
    void keywordsChanged();
    void visibilityChanged();

private:
    ItemBasePrivate *d_ptr;
    Q_DECLARE_PRIVATE(ItemBase)
};

} // namespace

#endif // SYSTEM_SETTINGS_ITEM_BASE_H
