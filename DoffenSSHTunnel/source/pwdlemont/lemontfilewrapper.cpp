#include "lemontfilewrapper.h"
#include "tinypasswordmanager.h"
#include "../exceptions.h"
#include "../identifier.h"
#include "sha256.h"

/**
 * public static
 * @brief LemontFileWrapper::getIdentifiers
 * @param password
 * @return
 */
QList<Identifier> LemontFileWrapper::getIdentifiers(QString filename, QString password)
{
    QList<Identifier> ret;
    TinyPasswordManager *m_tpm = TinyPasswordManager::getInstance();
    unsigned char digestKey[32]; memset(digestKey, 0, 32);
    SHA256::getInstance()->digest(password.toStdString(), digestKey);
    m_tpm->setPrivateKey(digestKey);
    m_tpm->openDatabase(filename.toStdString());
    m_tpm->getIdentifiers();
    for(unsigned int i = 0; i < m_tpm->size(); ++i) {
        Identifier ident = m_tpm->at(i);
        ret.append(ident);
    }
    m_tpm->kill();
    return ret;
}
