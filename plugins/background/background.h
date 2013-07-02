#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <QDBusInterface>
#include <QObject>
#include <QProcess>

class Background : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString backgroundFile
                READ backgroundFile
                NOTIFY backgroundFileChanged )
    
public:
    explicit Background(QObject *parent = 0);
    ~Background();

public Q_SLOTS:
    void slotChanged();

Q_SIGNALS:
    void backgroundFileChanged();

protected:
    QString backgroundFile();

    QString background_file;

private:
    QDBusConnection system_bus_connection;
    QString object_path;
    QDBusInterface accountsservice_iface;
    QString get_background_file();
};

#endif // BACKGROUND_H
