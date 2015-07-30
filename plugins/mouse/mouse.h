/*
 * Copyright (C) 2015 Canonical Ltd
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
 * Authors:
 * Ken VanDine <ken.vandine@canonical.com>
 *
 */

#ifndef MOUSE_H
#define MOUSE_H

#include <QDBusInterface>
#include <QObject>

class Mouse : public QObject
{
    Q_OBJECT
    Q_ENUMS(Button)
    Q_PROPERTY( QString path
                READ path
                WRITE setPath
                NOTIFY pathChanged )
    Q_PROPERTY( bool enabled
                READ enabled
                WRITE setEnabled
                NOTIFY enabledChanged )
    Q_PROPERTY( Button primaryButton
                READ primaryButton
                WRITE setPrimaryButton
                NOTIFY primaryButtonChanged )
    Q_PROPERTY( float cursorSpeed
                READ cursorSpeed
                WRITE setCursorSpeed
                NOTIFY cursorSpeedChanged )
    Q_PROPERTY( float clickSpeed
                READ clickSpeed
                WRITE setClickSpeed
                NOTIFY clickSpeedChanged )
    Q_PROPERTY( float scrollSpeed
                READ scrollSpeed
                WRITE setScrollSpeed
                NOTIFY scrollSpeedChanged )
    Q_PROPERTY( bool connected
                READ connected
                NOTIFY connectedChanged )

public:
    enum Button {
        Left,
        Middle,
        Right
    };

    explicit Mouse(QObject *parent = 0);
    ~Mouse();

    QString path() const;
    void setPath(const QString &path);

    bool enabled() const;
    void setEnabled(const bool &enabled);

    Button primaryButton() const;
    void setPrimaryButton(const Mouse::Button &button);

    float cursorSpeed() const;
    void setCursorSpeed(const float &speed);

    float clickSpeed() const;
    void setClickSpeed(const float &speed);

    float scrollSpeed() const;
    void setScrollSpeed(const float &speed);

    bool connected() const;

Q_SIGNALS:
    void pathChanged(const QString &path);
    void enabledChanged(const bool enabled);
    void primaryButtonChanged(const Mouse::Button &button);
    void cursorSpeedChanged(float speed);
    void clickSpeedChanged(float speed);
    void scrollSpeedChanged(float speed);
    void connectedChanged(bool connected);

private:
    QDBusConnection m_systemBusConnection;
    QDBusInterface m_unityInterface;
    QString m_path;
    bool m_enabled;
    Button m_primaryButton = Mouse::Button::Left;
    float m_cursorSpeed = 0.5;
    float m_clickSpeed = 500;
    float m_scrollSpeed = 0.5;
    bool m_connected;

};

Q_DECLARE_METATYPE (Mouse::Button)

#endif // MOUSE_H
