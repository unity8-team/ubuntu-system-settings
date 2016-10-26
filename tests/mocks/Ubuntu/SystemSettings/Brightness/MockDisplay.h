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

    Q_INVOKABLE void setAvailableModes(const QStringList &availableModes) // mock only
    {
        m_availableModes = availableModes;
        Q_EMIT availableModesChanged();
    }

    Q_INVOKABLE void setUncommitedChanges(const bool uncommittedChanges) // mock only
    {
        m_uncommittedChanges = uncommittedChanges;
        Q_EMIT uncommittedChangesChanged();
    }

    Q_INVOKABLE void addMode(const QString &mode) // mock only
    {
        m_availableModes.append(mode);
        Q_EMIT availableModesChanged();
    }
};

Q_DECLARE_METATYPE(MockDisplay*)

#endif // MOCK_DISPLAY_H
