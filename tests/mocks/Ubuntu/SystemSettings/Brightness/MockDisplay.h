#ifndef MOCK_DISPLAY_H
#define MOCK_DISPLAY_H

#include "display.h"

#include <QObject>
#include <QDebug>

struct MockDisplay : public Display
{
    Q_OBJECT
public:
    Q_INVOKABLE void setName(const QString &name) // mock only
    {
        m_name = name;
        Q_EMIT nameChanged();
    }

    Q_INVOKABLE void setType(const QString &type) // mock only
    {
        m_type = type;
        Q_EMIT typeChanged();
    }

    Q_INVOKABLE void setConnected(const bool &connected) // mock only
    {
        m_connected = connected;
        Q_EMIT connectedChanged();
    }

    Q_INVOKABLE void setUncommitedChanges(const bool uncommittedChanges) // mock only
    {
        m_uncommittedChanges = uncommittedChanges;
        Q_EMIT uncommittedChangesChanged();
    }

    // Return at what index the new mode was placed.
    Q_INVOKABLE uint addMode(const uint &horizontal, const uint &vertical,
                             const uint &refresh) // mock only
    {
        DisplayMode mode;
        mode.vertical_resolution = vertical;
        mode.horizontal_resolution = horizontal;
        mode.refresh_rate = refresh;

        m_modes.append(mode);
        Q_EMIT modesChanged();
        storeConfiguration();
        return m_modes.indexOf(mode);
    }

    Q_INVOKABLE void save()
    {
        storeConfiguration();
        setUncommitedChanges(false);
    }
Q_SIGNALS:
    void nameChanged();
    void typeChanged();
};

// Q_DECLARE_METATYPE(MockDisplay*)

#endif // MOCK_DISPLAY_H
