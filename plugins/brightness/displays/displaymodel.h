#ifndef DISPLAY_MODEL_H
#define DISPLAY_MODEL_H

#include "display.h"

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QModelIndex>
#include <QSharedPointer>
#include <QVariant>

class DisplayModel: public QAbstractListModel
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
      ResolutionRole,
      MirroredRole,
      RotationRole,
      LastRole = StateRole
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int,QByteArray> roleNames() const;

Q_SIGNALS:
    void countChanged();

private:
    QList<QSharedPointer<Display> > m_displays;
};

#endif // DISPLAY_MODEL_H
