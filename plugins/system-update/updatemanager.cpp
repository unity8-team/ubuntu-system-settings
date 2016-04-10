/*
 * Copyright (C) 2013-2016 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

namespace UpdatePlugin {

UpdateManager *UpdateManager::m_instance = 0;

UpdateManager *UpdateManager::instance()
{
    if (!m_instance)
        m_instance = new UpdateManager;

    return m_instance;
}

UpdateManager::~UpdateManager()
{
}

const bool UpdateManager::online()
{
    return m_online;
}

void UpdateManager::setOnline(const bool &online)
{
    if (online != m_online) {
        m_online = online;
        Q_EMIT onlineChanged();
    }
}

const bool UpdateManager::authenticated()
{
    return m_authenticated;
}

void UpdateManager::setAuthenticated(const bool &authenticated)
{
    if (authenticated != m_authenticated) {
        m_authenticated = authenticated;
        Q_EMIT authenticatedChanged();
    }

}

const QVariantList UpdateManager::udmDownloads()
{
    return m_udmDownloads;
}

void UpdateManager::setUdm(const QVariantList udmDownloads)
{
    if (udmDownloads != m_udmDownloads) {
        m_udmDownloads = udmDownloads;
        Q_EMIT udmDownloadsChanged();
    }
}


void UpdateManager::receivedClickMetadata(const QJsonDocument metadata)
{

}

void UpdateManager::receivedInstalledClicks(const int exitCode)
{

}

void UpdateManager::clickMetaPrimed(ClickMeta clickMeta)
{

}


} // UpdatePlugin
