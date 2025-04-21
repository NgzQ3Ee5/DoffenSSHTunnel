#ifndef BOTANWRAPPER_H
#define BOTANWRAPPER_H

#include <QObject>
#include <QByteArray>
#include <QDebug>
#include <QJsonDocument>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <memory>
#include "botan.h"

using namespace std;
using namespace Botan;

class BotanWrapper : public QObject
{
    Q_OBJECT
public:
    explicit BotanWrapper(QObject *parent = 0);
    
    /*!
    * Creates a hash
    * @param Data The string to hash
    */
    QString Hash(QString Data);

    /*!
    * Creates a hash in hex format
    * @param Data The string to hash
    */
    QString HexHash(QString Data);

    /*!
    * Returns a Base64 encoded QString
    * @param Data The string to encode
    */
    QString Encode(QString Data);

    /*!
    * Returns a decoded string from a Base64 encoded string
    * @param Data The string to decode
    */
    QString Decode(QString Data);


    /*!
    * Returns renadom hex QString
    */
    QString GenerateRandomString();

    /*!
    * Returns encrypted QString
    * @param Data The string to encypt
    * @param Password The Password
    */
    QString EncryptWithPassword(QString Data, QString Password);

    /*!
    * Returns a decrypted QString
    * @param Data The string to encypt
    * @param Password The Password
    */
    QString Decrypt(QString Data, QString Password);

    /*!
    * Returns encrypted QByteArray
    * @param Data The byte array
    * @param Password The Password
    */
    QByteArray EncryptWithPassword(QByteArray Data, QString Password);

    /*!
    * Returns decrypted QByteArray
    * @param Data The encrypted byte array
    * @param Password The Password
    */
    QByteArray Decrypt(QByteArray Data, QString Password);


    /*!
    * Takes the password to hash. The resulting hash is returned as a string.
    * @param Password The password
    */
    QString BCryptGenerate(QString Password);

    /*!
    * Takes a password and a bcrypt output and returns true
    * if the password is the same as the one that was used to generate the bcrypt hash
    * @param Password The password
    * @param Hash The bcrypt output
    */
    bool BCryptCheck(QString Password, QString Hash);

signals:
    
public slots:

private:
    /*!
    * The botan libary initilizer
    */
    Botan::LibraryInitializer mInit;

};

#endif // BOTANWRAPPER_H
