#ifndef DISPLAY_H
#define DISPLAY_H

#include <QObject>
#include <QString>
#include <QStringList>

class Display : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name
               READ name
               NOTIFY nameChanged)
    Q_PROPERTY(QString type
               READ type
               NOTIFY typeChanged)
    Q_PROPERTY(bool mirrored
               READ mirrored
               WRITE setMirrored
               NOTIFY mirroredChanged)
    Q_PROPERTY(bool connected
               READ connected
               NOTIFY connectedChanged)
    Q_PROPERTY(bool enabled
               READ enabled
               WRITE setEnabled
               NOTIFY enabledChanged)
    Q_PROPERTY(QString mode
               READ mode
               WRITE setMode
               NOTIFY modeChanged)
    Q_PROPERTY(QStringList availableModes
               READ availableModes
               NOTIFY availableModesChanged)
    Q_PROPERTY(Orientation orientation
               READ orientation
               WRITE setOrientation
               NOTIFY orientationChanged)
    Q_PROPERTY(double scale
               READ scale
               WRITE setScale
               NOTIFY scaleChanged)
    Q_PROPERTY(bool uncommittedChanges
               READ uncommittedChanges
               NOTIFY uncommittedChangesChanged)

Q_SIGNALS:
    void nameChanged();
    void typeChanged();
    void mirroredChanged();
    void connectedChanged();
    void enabledChanged();
    void modeChanged();
    void availableModesChanged();
    void orientationChanged();
    void scaleChanged();
    void uncommittedChangesChanged();
    void displayChanged(const Display *display);

public:
    explicit Display(QObject *parent = nullptr);
    ~Display() {};
    enum class Orientation : uint {
        Normal, PortraitMode, LandscapeInvertedMode,
        PortraitInvertedMode
    };
    enum class DisplayType : uint {
        PhysicalType, AethercastType
    };
    Q_ENUMS(Orientation DisplayType)

    QString name() const;
    QString type() const;
    bool mirrored() const;
    bool connected() const;
    bool enabled() const;
    QString mode() const;
    QStringList availableModes() const;
    Orientation orientation() const;
    double scale() const;
    bool uncommittedChanges() const;

    void setMirrored(const bool &mirrored);
    void setEnabled(const bool &enabled);
    void setMode(const QString &mode);
    void setOrientation(const Orientation &orientation);
    void setScale(const double &scale);

protected:
    void setName(const QString &name);
    void setAvailableModes(const QStringList &availableModes);
    void setConnected(const bool &connected);
    void setType(const QString &type);
    void setUncommitedChanges(const bool uncommittedChanges);
    QString m_name = QString::null;
    QString m_type = QString::null;
    bool m_mirrored = false;
    bool m_connected = false;
    bool m_enabled = false;
    QString m_mode = QString::null;
    QStringList m_availableModes = QStringList();
    Orientation m_orientation = Orientation::Normal;
    double m_scale = 1;
    bool m_uncommittedChanges = false;

private slots:
    void changedSlot();
};

Q_DECLARE_METATYPE(Display*)

#endif // DISPLAY_H
