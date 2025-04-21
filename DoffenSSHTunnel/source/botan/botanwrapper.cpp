#include "botanwrapper.h"

BotanWrapper::BotanWrapper(QObject *parent) :
    QObject(parent)
{

}

QString BotanWrapper::Hash(QString Data)
{
    try
    {
        Pipe pipe(new Hash_Filter("SHA-1"));
        pipe.process_msg(Data.toStdString());
        QString Value = QString::fromStdString(pipe.read_all_as_string(0));
        return Value;
    }
    catch(...)
    {
        return "";
    }
}

QString BotanWrapper::HexHash(QString Data)
{
    try
    {
        Pipe pipe(new Hash_Filter("SHA-1"), new Hex_Encoder);
        pipe.process_msg(Data.toStdString());
        QString Value = QString::fromStdString(pipe.read_all_as_string(0));
        return Value;
    }
    catch(...)
    {
        return "";
    }
}

QString BotanWrapper::Encode(QString Data)
{
    try
    {
        Pipe pipe(new Base64_Encoder);
        pipe.process_msg(Data.toStdString());
        QString Value = QString::fromStdString(pipe.read_all_as_string(0));
        return Value;
    }
    catch(...)
    {
        return "";
    }
}

QString BotanWrapper::Decode(QString Data)
{
    try
    {
        Pipe pipe(new Base64_Decoder);
        pipe.process_msg(Data.toStdString());
        QString Value = QString::fromStdString(pipe.read_all_as_string(0));
        return Value;
    }
    catch(...)
    {
        return "";
    }
}

QString BotanWrapper::GenerateRandomString()
{
    AutoSeeded_RNG rng;
    OctetString str(rng,32);
    return QString::fromStdString(str.as_string());
}

QString BotanWrapper::EncryptWithPassword(QString Data, QString Password)
{

    AutoSeeded_RNG rng;
    string str = CryptoBox::encrypt(
        reinterpret_cast<const Botan::byte*>(Data.toStdString().data()),
        Data.size(),
        Password.toStdString(),
        rng);
    return QString::fromStdString(str);
}


QByteArray BotanWrapper::EncryptWithPassword(QByteArray Data, QString Password)
{
    AutoSeeded_RNG rng;
    string str = CryptoBox::encrypt(
        reinterpret_cast<const Botan::byte *>(Data.constData()),
        Data.size(),
        Password.toStdString(),
        rng);
    return QByteArray(str.data());
}

QString BotanWrapper::Decrypt(QString Data, QString Password)
{
    string str = CryptoBox::decrypt(
        reinterpret_cast<const Botan::byte*>(Data.toStdString().data()),
        Data.size(),
        Password.toStdString());
    return QString::fromStdString(str);;
}

QByteArray BotanWrapper::Decrypt(QByteArray Data, QString Password)
{
    string str = CryptoBox::decrypt(
        reinterpret_cast<const Botan::byte*>(Data.toStdString().data()),
        Data.size(),
        Password.toStdString());
    return QByteArray(str.data());
}

QString BotanWrapper::BCryptGenerate(QString Password)
{
    AutoSeeded_RNG rng;
    string hash = generate_bcrypt(Password.toStdString(),rng,12);
    QString Value = QString::fromStdString(hash);
    return Value;
}

bool BotanWrapper::BCryptCheck(QString Password, QString Hash)
{
    return check_bcrypt(Password.toStdString(), Hash.toStdString());
}



