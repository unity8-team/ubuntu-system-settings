#include "certhandler.h"

#include <QtQml>
#include <QtQml/QQmlContext>
#include <QtDebug>
#include <QObject>
#include <QSslCertificate>
#include <QSslKey>
#include <QAbstractListModel>
#include <QDir>
#include <QFile>

#define CERTS_PATH "/home/phablet/.local/share/ubuntu-system-settings/wifi/ssl/certs/"
#define KEYS_PATH  "/home/phablet/.local/share/ubuntu-system-settings/wifi/ssl/private/"
#define PACS_PATH  "/home/phablet/.local/share/ubuntu-system-settings/wifi/ssl/pac/"

#include <libintl.h>
QString _(const char *text){
    return QString::fromUtf8(dgettext(0, text));
}

QByteArray FileHandler::getCertContent(QString filename){
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not resolve File (" << filename << "): File does not exist or is empty." ;
        return QByteArray();
    }
    else {
        return file.readAll();
    }
}

QString FileHandler::moveCertFile(QString filename){
    QDir certPath(CERTS_PATH);
    if (!certPath.exists(CERTS_PATH)){
        certPath.mkpath(CERTS_PATH);
    }
    QFile file(filename);
    QByteArray certificate = getCertContent(filename);
    QList<QSslCertificate> SslCertificateList = QSslCertificate::fromData(certificate, QSsl::Pem);
    if ( !SslCertificateList.isEmpty() ){
        QStringList subject = SslCertificateList[0].subjectInfo(QSslCertificate::CommonName);
        QString modFileName = CERTS_PATH+subject[0]+".pem";
        if(file.rename(modFileName.replace(" ", "_"))){
            return file.fileName();
        } else {
            return "";
        }
    }
    return "";
}

QString FileHandler::moveKeyFile(QString filename){
    QDir keyPath(KEYS_PATH);
    if (!keyPath.exists(KEYS_PATH)){
        keyPath.mkpath(KEYS_PATH);
    }
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QSslKey checkKey(file.readAll(),  QSsl::Rsa);
    file.close();
    if ( !checkKey.isNull() ){
        QFileInfo fileInfo(file);
        QString modFileName = KEYS_PATH + fileInfo.fileName().replace(" ", "_");
        if(file.rename(modFileName)){
            return file.fileName();
        } else {
            return "" ;
        }
    }
    return "";
}

QString FileHandler::movePacFile(QString filename){
    QDir keyPath(PACS_PATH);
    if (!keyPath.exists(PACS_PATH)){
        keyPath.mkpath(PACS_PATH);
    }
    QFile file(filename);
    QFileInfo fileInfo(file);
    QString modFileName = PACS_PATH + fileInfo.baseName().replace(" ", "_") + ".pac";
    if(file.rename(modFileName)){
        return file.fileName();
    }
    return "" ;
}

bool FileHandler::removeFile(QString filename){
    QFile file(filename);
    return file.remove();
}

struct CertificateListModel::Private {
    QStringList data;
};

CertificateListModel::CertificateListModel(QObject *parent) : QAbstractListModel(parent) {
    p = new CertificateListModel::Private();
    QStringList nameFilter("*.pem");
    QDir directory(CERTS_PATH);
    QStringList files = directory.entryList(nameFilter);
    files.sort(Qt::CaseInsensitive);
    files.insert(0, _("None") );
    files.append( _("Choose…") );
    p->data = files;
}

CertificateListModel::~CertificateListModel() {
    delete p;
}

QHash<int, QByteArray> CertificateListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[CNRole] = "CommonName";
    roles[ORole] = "Organization";
    roles[expDateRole] = "expiryDate";

    //roles[certFileNameRole] = "certFileName";
    //...more if needed see QSslCertificate::SubjectInfo
    return roles;
}

int CertificateListModel::rowCount(const QModelIndex &/*parent*/) const {
    return p->data.size();
}

QString CertificateListModel::getfileName(const int selectedIndex) const {
    return CERTS_PATH + p->data[selectedIndex];
}

void CertificateListModel::dataupdate(){
    beginResetModel();
    p->data.clear();
    QStringList nameFilter("*.pem");
    QDir directory(CERTS_PATH);
    QStringList files = directory.entryList(nameFilter);
    files.sort(Qt::CaseInsensitive);
    files.insert(0, _("None") );
    files.append( _("Choose…") );
    p->data = files;
    endResetModel();
}

QVariant CertificateListModel::data(const QModelIndex &index, int role) const {
    if(!index.isValid() || index.row() >= ( p->data.size()) ) {
        return QVariant();
    } else if (index.row() == 0){
        const QString &row0 = p->data[index.row()];

        switch(role) {
        case CNRole : return row0;
        case ORole : return "";
        case expDateRole : return "";

        }
    } else if (index.row() == p->data.size()-1){
        const QString &rowend = p->data[index.row()];

        switch(role) {
        case CNRole : return rowend;
        case ORole : return "";
        case expDateRole : return "";
        }
    }

    const QString &row = CERTS_PATH+p->data[index.row()];
    QList<QSslCertificate> certificate = QSslCertificate::fromPath(row, QSsl::Pem, QRegExp::Wildcard);

    switch(role) {

    case CNRole : return certificate[0].subjectInfo(QSslCertificate::CommonName)[0];
    case ORole : return certificate[0].subjectInfo(QSslCertificate::Organization)[0];
    case expDateRole : return certificate[0].expiryDate().toString("dd.MM.yyyy");

    default : return QVariant();
    }
}

/***************************************/

struct PrivatekeyListModel::Private {
    QStringList data;
};

PrivatekeyListModel::PrivatekeyListModel(QObject *parent) : QAbstractListModel(parent) {
    p = new PrivatekeyListModel::Private();
    QDir directory(KEYS_PATH);
    QStringList files = directory.entryList(QDir::Files, QDir::Name);
    files.sort(Qt::CaseInsensitive);
    files.insert(0, _("None") );
    files.append( _("Choose…") );
    p->data = files;
}

PrivatekeyListModel::~PrivatekeyListModel() {
    delete p;
}

QHash<int, QByteArray> PrivatekeyListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[keyName] = "KeyName";
    roles[keyType] = "KeyType";
    roles[keyAlgorithm] = "KeyAlgorithm";
    roles[keyLength] = "KeyLength";
    return roles;
}

int PrivatekeyListModel::rowCount(const QModelIndex &/*parent*/) const {
    return p->data.size();
}

QString PrivatekeyListModel::getfileName(const int selectedIndex) const {
    return  KEYS_PATH + p->data[selectedIndex];
}

void PrivatekeyListModel::dataupdate(){
    beginResetModel();
    p->data.clear();
    QDir directory(KEYS_PATH);
    QStringList files = directory.entryList(QDir::Files, QDir::Name);
    files.sort(Qt::CaseInsensitive);
    files.insert(0, _("None") );
    files.append( _("Choose…") );
    p->data = files;
    endResetModel();
}

QVariant PrivatekeyListModel::data(const QModelIndex &index, int role) const {
    if(!index.isValid() || index.row() >= ( p->data.size()) ) {
        return QVariant();
    } else if (index.row() == 0){
        const QString &row0 = p->data[index.row()];

        switch(role) {
        case keyName : return row0; // returns "None"
        case keyType : return "";
        case keyAlgorithm : return "";
        case keyLength : return "";
        }
    } else if (index.row() == p->data.size()-1){
        const QString &rowend = p->data[index.row()];

        switch(role) {
        case keyName : return rowend; // returns "Choose file...
        case keyType : return "";
        case keyAlgorithm : return "";
        case keyLength : return "";
        }
    }

    const QString &row = KEYS_PATH+p->data[index.row()];
    QFile keyFile(row);
    keyFile.open(QIODevice::ReadOnly);
    QSslKey privateKey( keyFile.readAll(),  QSsl::Rsa );
    QString type;
    if (privateKey.type() == 0){ type = _("Private key");}
    else { type = _("Public key"); }

    QString algorithm;
    if (privateKey.algorithm() == 1) { algorithm = "RSA";}
    else if (privateKey.algorithm() == 2){ algorithm = "DSA";}
    else { algorithm = _("Opaque");}

    QFileInfo keyFileInfo(keyFile);
    switch(role) {

    case keyName : return keyFileInfo.fileName();
    case keyType : return type;
    case keyAlgorithm : return algorithm;
    case keyLength : return privateKey.length();

    default : return QVariant();
    }
}

/***************************************/

struct PacFileListModel::Private {
    QStringList data;
};

PacFileListModel::PacFileListModel(QObject *parent) : QAbstractListModel(parent) {
    p = new PacFileListModel::Private();
    QDir directory(PACS_PATH);
    QStringList files = directory.entryList(QDir::Files, QDir::Name);
    files.sort(Qt::CaseInsensitive);
    files.insert(0, _("None") );
    files.append( _("Choose…") );
    p->data = files;
}

PacFileListModel::~PacFileListModel() {
    delete p;
}

QHash<int, QByteArray> PacFileListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[pacFileName] = "pacFileName";
    return roles;
}

int PacFileListModel::rowCount(const QModelIndex &/*parent*/) const {
    return p->data.size();
}

QString PacFileListModel::getfileName(const int selectedIndex) const {
    return  PACS_PATH + p->data[selectedIndex];
}

void PacFileListModel::dataupdate(){
    beginResetModel();
    p->data.clear();
    QDir directory(PACS_PATH);
    QStringList files = directory.entryList(QDir::Files, QDir::Name);
    files.sort(Qt::CaseInsensitive);
    files.insert(0, _("None") );
    files.append( _("Choose…") );
    p->data = files;
    endResetModel();
}

QVariant PacFileListModel::data(const QModelIndex &index, int role) const {
    if(!index.isValid() || index.row() >= ( p->data.size()) ) {
        return QVariant();
    } else if (index.row() == 0){
        const QString &row0 = p->data[index.row()];

        switch(role) {
        case pacFileName : return row0; // returns "None"
        }
    } else if (index.row() == p->data.size()-1){
        const QString &rowend = p->data[index.row()];

        switch(role) {
        case pacFileName : return rowend; // returns "Choose file...
        }
    }

    const QString &name = p->data[index.row()];
    switch(role) {

    case pacFileName : return name;

    default : return QVariant();
    }
}
