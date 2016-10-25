#ifndef MOCK_DISPLAY_MODEL_H
#define MOCK_DISPLAY_MODEL_H

#include "displaymodel.h"
#include "MockDisplay.h"

#include <QObject>
#include <QQmlEngine>

class MockDisplayModel : public DisplayModel
{
    Q_OBJECT
public:
    MockDisplayModel(QObject *parent = 0) : DisplayModel(parent) {
    };
    ~MockDisplayModel() {};
    Q_INVOKABLE MockDisplay* mockAddDisplay() // mock only
    {
        QSharedPointer<MockDisplay> d = QSharedPointer<MockDisplay>(new MockDisplay);
        addDisplay(d);
        QQmlEngine::setObjectOwnership(d.data(), QQmlEngine::CppOwnership);
        return d.data();
    }
};

#endif // MOCK_DISPLAY_MODEL_H
