#ifndef DISPLAY_MODEL_H
#define DISPLAY_MODEL_H

#include "display.h"

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QModelIndex>
#include <QSharedPointer>
#include <QSortFilterProxyModel>
#include <QVariant>

class DisplayModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
public:
    explicit DisplayModel(QObject *parent = 0);
    ~DisplayModel();

    enum Roles
    {
      // Qt::DisplayRole holds device name
      TypeRole = Qt::UserRole,
      MirroredRole,
      ConnectedRole,
      EnabledRole,
      ModeRole,
      ModesRole,
      OrientationRole,
      ScaleRole,
      UncommittedChangesRole,
      LastRole = UncommittedChangesRole
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole);
    QHash<int,QByteArray> roleNames() const;
    void addDisplay(const QSharedPointer<Display> &display);

Q_SIGNALS:
    void countChanged();

protected:
    void emitRowChanged(const int &row);

private:
    QList<QSharedPointer<Display> > m_displays;

private slots:
    void displayChangedSlot(const Display *display);
};

class DisplaysFilter : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    DisplaysFilter() {}
    virtual ~DisplaysFilter() {}

    void filterOnUncommittedChanges(const bool uncommitted);
    void filterOnConnected(const bool connected);

Q_SIGNALS:
    void countChanged(int count);

protected:
    virtual bool filterAcceptsRow(int, const QModelIndex&) const;
    virtual bool lessThan(const QModelIndex&, const QModelIndex&) const;

private:
    bool m_uncommittedChanges = false;
    bool m_uncommittedChangesEnabled = false;

    bool m_connected = false;
    bool m_connectedEnabled = false;

};

#endif // DISPLAY_MODEL_H
