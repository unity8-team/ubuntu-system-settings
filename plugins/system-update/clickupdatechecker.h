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
#ifndef CLICKAPI_H
#define CLICKAPI_H

namespace UpdatePlugin {

class ClickUpdateChecker : public QObject
{
    Q_OBJECT
public:
    explicit ClickUpdateChecker(QObject *parent = 0);
    ~ClickUpdateChecker() {}

    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)

    void check();
    void abort();

    const QString errorString();

public slots:
    void receivedClickUpdateMetadata(const QJsonDocument &metadata);

private slots:
    void requestSucceded();
    void requestFailed();
    void requestSslFailed();
    void processedInstalledClicks(const int exitCode);

signals:
    void serverError();
    void errorStringChanged();
    void foundClickUpdate(QMap<QString, QVariant> clickUpdateMetadata);

private:
    QProcess m_process;
    UbuntuOne::Token m_token;
};

}

#endif // CLICKAPI_H
