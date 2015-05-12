#include "certhandler.h"

#include <QtQml>
#include <QtQml/QQmlContext>
#include <QtDebug>
#include <QObject>
#include <QSslCertificate>
#include <QAbstractListModel>
#include <QDir>

#define CERTS_PATH "/home/phablet/.local/share/ubuntu-system-settings/Documents/"  //<----to be changed #jkb
#define KEYS_PATH  "/home/phablet/.local/share/ubuntu-system-settings/Documents/"
/*
CERTS_PATH points to the certificates directory. 
Later on should/could be .../ubuntu-system-settings/wifi/ssl/certs
CA and client to be stored here.
and KEYS_PATH to private keys
Cold be .../ubuntu-system-settings/wifi/ssl/keys

Same for pac files?
*/

QByteArray CertificateHandler::getCertContent(QString filename){
    QFile file(filename);
      if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Could not resolve Cert-File (" << filename << "): File does not exist or is empty." ;
            return QByteArray();
      }
      else {
            return file.readAll();
      }
}

QString CertificateHandler::moveCertFile(QString filename){
    QDir certPath(CERTS_PATH);
    if (!certPath.exists(CERTS_PATH)){
        certPath.mkpath(CERTS_PATH);
    }
    QFile file(filename);
    QByteArray certificate = getCertContent(filename);
    QList<QSslCertificate> SslCertificate = QSslCertificate::fromData(certificate, QSsl::Pem);
    QStringList subject = SslCertificate[0].subjectInfo(QSslCertificate::CommonName);
    QString modFileName = CERTS_PATH+subject[0]+".pem";
    if(file.rename(modFileName.replace(" ", "_"))){
        return file.fileName();
    } else {
        return "Error storing certificate." ;
    }
}

bool CertificateHandler::removeFile(QString filename){
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
    files.insert(0, "None");
    files.append("Choose file…");
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
        files.insert(0, "None");
        files.append("Choose file…");
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
            //case certFileNameRole : return "";
        }
    } else if (index.row() == p->data.size()-1){
        const QString &rowend = p->data[index.row()];

        switch(role) {
            case CNRole : return rowend;
            case ORole : return "";
            case expDateRole : return "";
            //case certFileNameRole : return "";
        }
    }

    const QString &row = CERTS_PATH+p->data[index.row()];
    QList<QSslCertificate> certificate = QSslCertificate::fromPath(row, QSsl::Pem, QRegExp::Wildcard);

    switch(role) {

    case CNRole : return certificate[0].subjectInfo(QSslCertificate::CommonName)[0];
    case ORole : return certificate[0].subjectInfo(QSslCertificate::Organization)[0];
    case expDateRole : return certificate[0].expiryDate().toString("dd.MM.yyyy");
    //case certFileNameRole : return row;

    default : return QVariant();

    }
}
