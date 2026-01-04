#include "PasswordDb.h"
#include "pwdlemont/lemontfilewrapper.h"
#include "exceptions.h"
#include "identifier.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QtCore/QFile>
#include <QtAlgorithms>

//private static
PasswordDb *PasswordDb::m_instance = NULL;

//private
PasswordDb::PasswordDb(QObject *parent) :
    QObject(parent)
{

}

//private
PasswordDb::~PasswordDb()
{

}

//static
PasswordDb *PasswordDb::getInstance()
{
    if(!m_instance)
        m_instance = new PasswordDb();

    return m_instance;
}

//static
void PasswordDb::kill()
{
    if(m_instance) {
        delete m_instance;
        m_instance = NULL;
    }
}


//protected
void PasswordDb::setPassword(QString password)
{
    m_password = m_bw.BCryptGenerate(password);
}

void PasswordDb::createEmptyFile(QString filename, QString password)
{
    m_idents.clear();
    m_filename = filename;
    saveFile(password);
}

void PasswordDb::openFile(QString filename, QString password)
{
    m_idents.clear();
    m_filename = filename;

    readBotanFile(password);
    setPassword(password);
}

//protected
void PasswordDb::readBotanFile(QString password)
{
    m_idents.clear();

    QFile loadFile(m_filename);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        throw ExFileNotFound(Q_FUNC_INFO, m_filename.toStdString());
    }

    QByteArray data = loadFile.readAll();
    loadFile.close();

    BotanWrapper::DecryptFormat fmt;
    QJsonDocument doc(QJsonDocument::fromJson(m_bw.Decrypt(data, password, &fmt)));
    QJsonObject json = doc.object();

    QJsonArray pwdArray = json["identities"].toArray();
    for(int i=0;i<pwdArray.size();i++) {
        QJsonObject pwdObject = pwdArray.at(i).toObject();
        Identifier ident;
        ident.setLogin(pwdObject["login"].toString());
        ident.setPassword(pwdObject["password"].toString());
        ident.setDescription(pwdObject["varname"].toString());
        QString sUuid = pwdObject["uuid"].toString();
        if(!sUuid.isEmpty()) {
            QUuid uuid = QUuid(sUuid);
            if(uuid.isNull()) {
                //invalid uuid format in .pwd file
                uuid = QUuid::createUuid();
            }
            ident.setUuid(uuid);
        }
        else {
            ident.setUuid(QUuid::createUuid());
        }
        m_idents.append(ident);
    }

    if(fmt == BotanWrapper::DecryptFormat::CryptoBoxLegacy)
    {
        // 2026-01-04: I just decrypted DoffenSSHTunnel.pwd with the old CryptoBox format.
        // Save it with the new DTENC1 format now (converting to my new format)

        const QString backupPath = m_filename + ".old_cryptobox";

        // Remove existing backup (QFile::copy() will fail if destination exists)
        if (QFile::exists(backupPath)) {
            QFile existingBackup(backupPath);
            if (!existingBackup.remove()) {
                throw ExBad(
                    QString("Failed to remove existing backup '%1': %2")
                        .arg(backupPath, existingBackup.errorString())
                        .toStdString()
                    );
            }
        }

        // Copy source -> backup
        QFile source(m_filename);
        if (!source.copy(backupPath)) {
            throw ExBad(
                QString("Failed to copy '%1' -> '%2': %3")
                    .arg(m_filename, backupPath, source.errorString())
                    .toStdString()
                );
        }

        saveFile(password);
    }

}

void PasswordDb::saveFile(QString password)
{
    QFile saveFile(m_filename);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        throw ExFileErrorWriting(Q_FUNC_INFO, m_filename.toStdString());
    }

    QJsonArray pwdArray;
    for(int i=0;i<m_idents.size();i++) {
        Identifier ident = m_idents.at(i);
        QJsonObject pwdObject;
        pwdObject["login"] = ident.getLogin();
        pwdObject["password"] = ident.getPassword();
        pwdObject["varname"] = ident.getDescription();
        pwdObject["uuid"] = ident.getUuid().toString();
        pwdArray.append(pwdObject);
    }

    QJsonObject pwdFileObject;
    pwdFileObject["identities"] = pwdArray;

    QJsonDocument saveDoc(pwdFileObject);
    //qDebug() << saveDoc.toJson();
    saveFile.write(m_bw.EncryptWithPassword(saveDoc.toJson(), password));
    saveFile.close();

    setPassword(password);
}

void PasswordDb::clear()
{
    m_idents.clear();
}

unsigned int PasswordDb::size() const
{
    return static_cast<unsigned int>(m_idents.size());
}

Identifier PasswordDb::at(unsigned int index) const
{
    try {
        Identifier ident = m_idents.at(index);
        Identifier ret;
        ret.setDescription(ident.getDescription());
        ret.setLogin(ident.getLogin());
        ret.setPassword(ident.getPassword());
        ret.setUuid(ident.getUuid());
        return ret;
    }
    catch(std::out_of_range&) {
        throw ExIndexOutOfBounds(Q_FUNC_INFO, "m_idents");
    }
}

void PasswordDb::removeIdentifier(const QUuid &uuid)
{
    int index = -1;
    for(int i=0; i<m_idents.size(); i++) {
        Identifier ident = m_idents.at(i);
        if(ident.getUuid() == uuid) {
            index = i;
            break;
        }
    }
    if(index != -1) {
        m_idents.removeAt(index);
    }
}

void PasswordDb::addIdentifier(const Identifier &identifier)
{
    m_idents.append(Identifier(identifier));
   // qSort(m_idents.begin(), m_idents.end(), compareIdentifier);
}

bool PasswordDb::comparePassword(QString password)
{
    bool ret = false;
    if(m_bw.BCryptCheck(password, m_password)) {
        ret = true;
    }
    return ret;
}

bool PasswordDb::compareIdentifier(Identifier& ident1, Identifier& ident2) {
    QString descr1 = ident1.getDescription();
    QString descr2 = ident2.getDescription();
    return (descr1.compare(descr2, Qt::CaseInsensitive) > 0) ? false : true;
}



