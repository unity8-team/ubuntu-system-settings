#ifndef CERTHANDLER_H
#define CERTHANDLER_H

#include <QtQml>
#include <QtQml/QQmlContext>
#include <QObject>
#include <QAbstractListModel>

class FileHandler : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE QByteArray getCertContent(QString filename);
    Q_INVOKABLE QString moveCertFile(QString filename);
    Q_INVOKABLE QString moveKeyFile(QString filename);
    Q_INVOKABLE QString movePacFile(QString filename);
    Q_INVOKABLE bool removeFile(QString filename);
};



class CertificateListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum CertificateListRoles {
        CNRole = Qt::UserRole + 1,
        ORole,
        expDateRole,
        //certFileNameRole,
    };

    explicit CertificateListModel(QObject *parent = 0);
    ~CertificateListModel();
    QHash<int, QByteArray> roleNames() const;
    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE QString  getfileName(const int selectedIndex) const;
    Q_INVOKABLE void dataupdate();
    QVariant data(const QModelIndex &index, int role) const;

private:
    struct Private;
    Private *p;

};

class PrivatekeyListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum PrivatekeyListRoles {
        keyName = Qt::UserRole + 1,
        keyType,
        keyAlgorithm,
        keyLength,
    };

    explicit PrivatekeyListModel(QObject *parent = 0);
    ~PrivatekeyListModel();
    QHash<int, QByteArray> roleNames() const;
    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE QString  getfileName(const int selectedIndex) const;
    Q_INVOKABLE void dataupdate();
    QVariant data(const QModelIndex &index, int role) const;
private:
    struct Private;
    Private *p;

};

class PacFileListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum PacFileListRoles {
        pacFileName = Qt::UserRole + 1,

    };

    explicit PacFileListModel(QObject *parent = 0);
    ~PacFileListModel();
    QHash<int, QByteArray> roleNames() const;
    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE QString  getfileName(const int selectedIndex) const;
    Q_INVOKABLE void dataupdate();
    QVariant data(const QModelIndex &index, int role) const;
private:
    struct Private;
    Private *p;

};


#endif // CERTHANDLER_H

