#ifndef DISPLAY_H
#define DISPLAY_H

#include <mir_toolkit/mir_client_library.h>

#include <iostream>

#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>

struct DisplayMode {
    Q_GADGET
public:
    uint vertical_resolution = 0;
    uint horizontal_resolution = 0;
    double refresh_rate = 0.0;
    Q_INVOKABLE QString toString() const
    {
        /* TRANSLATORS: %1 refer to the amount of horizontal pixels in a
        display resolution, and %2 to the vertical pixels. E.g. 1200x720.
        %3 is the refresh rate in hz. */
        return QString("%1×%2 @ %3hz")
            .arg(horizontal_resolution)
            .arg(vertical_resolution)
            .arg(refresh_rate);
    }
    bool operator==(const DisplayMode &other) const
    {
        return (
            horizontal_resolution == other.horizontal_resolution
            && vertical_resolution == other.vertical_resolution
            && refresh_rate == other.refresh_rate
        );
    }
};

class Display : public QObject
{
    Q_OBJECT
    Q_PROPERTY(uint id READ id CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString type READ type CONSTANT)
    Q_PROPERTY(bool mirrored READ mirrored WRITE setMirrored
               NOTIFY mirroredChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled
               NOTIFY enabledChanged)
    Q_PROPERTY(uint mode READ mode WRITE setMode
               NOTIFY modeChanged)
    Q_PROPERTY(QStringList modes READ modes
               NOTIFY modesChanged) // TODO: Maybe constant?
    Q_PROPERTY(Orientation orientation READ orientation WRITE setOrientation
               NOTIFY orientationChanged)
    Q_PROPERTY(double scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(bool uncommittedChanges READ uncommittedChanges
               NOTIFY uncommittedChangesChanged)
    Q_PROPERTY(uint physicalWidthMm READ physicalWidthMm CONSTANT)
    Q_PROPERTY(uint physicalHeightMm READ physicalHeightMm CONSTANT)

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

public:
    explicit Display(QObject *parent = nullptr);
    explicit Display(MirDisplayOutput &output, QObject *parent = nullptr);
    ~Display() {};

    enum class Orientation : uint {
        NormalOrientation, PortraitModeOrientation,
        LandscapeInvertedModeOrientation,
        PortraitInvertedModeOrientation
    };
    Q_ENUMS(Orientation)

    uint id() const;
    QString name() const;
    QString type() const;
    bool mirrored() const;
    bool connected() const;
    bool enabled() const;
    uint mode() const;
    QStringList modes() const;
    Orientation orientation() const;
    double scale() const;
    bool uncommittedChanges() const;
    uint physicalWidthMm() const;
    uint physicalHeightMm() const;

    void setMirrored(const bool &mirrored);
    void setEnabled(const bool &enabled);
    void setMode(const uint &mode);
    void setOrientation(const Orientation &orientation);
    void setScale(const double &scale);

protected:
    void setUncommitedChanges(const bool uncommittedChanges);
    void setConnected(const bool &connected);

    uint m_id = 0;
    QString m_name = QString::null;
    QString m_type = QString::null;
    bool m_mirrored = false;
    bool m_connected = false;
    bool m_enabled = false;
    DisplayMode m_mode;
    QList<DisplayMode> m_modes;
    Orientation m_orientation = Orientation::NormalOrientation;
    double m_scale = 1;
    bool m_uncommittedChanges = false;
    uint m_physicalWidthMm = 0;
    uint m_physicalHeightMm = 0;

protected slots:
    void changedSlot();

private:
    void initialize();
};

Q_DECLARE_METATYPE(Display*)
//Q_DECLARE_METATYPE(DisplayMode)
Q_DECLARE_METATYPE(Display::Orientation)

#endif // DISPLAY_H
