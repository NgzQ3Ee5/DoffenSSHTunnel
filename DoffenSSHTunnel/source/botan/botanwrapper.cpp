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
    Botan::AutoSeeded_RNG rng;

    // Konverter eksplisitt til bytes (UTF-8) og bruk korrekt byte-lengde
    const QByteArray dataBytes = Data.toUtf8();
    const std::string passwordStd = Password.toUtf8().toStdString();

    const std::string str = Botan::CryptoBox::encrypt(
        reinterpret_cast<const Botan::byte*>(dataBytes.constData()),
        static_cast<size_t>(dataBytes.size()),
        passwordStd,
        rng
        );

    // CryptoBox::encrypt returnerer PEM-tekst -> trygt å mappe til QString via UTF-8/Latin1
    // PEM er ASCII, så Latin1 er også ok. UTF-8 er også ok.
    return QString::fromLatin1(str.data(), static_cast<int>(str.size()));
}


QByteArray BotanWrapper::EncryptWithPassword(QByteArray Data, QString Password)
{
    Botan::AutoSeeded_RNG rng;

    const std::string passwordStd = Password.toUtf8().toStdString();

    const std::string str = Botan::CryptoBox::encrypt(
        reinterpret_cast<const Botan::byte*>(Data.constData()),
        static_cast<size_t>(Data.size()),
        passwordStd,
        rng
        );

    // Viktig: bruk lengde for å unngå NUL-trunkering (du gjorde dette riktig her)
    return QByteArray(str.data(), static_cast<int>(str.size()));
}

QString BotanWrapper::Decrypt(QString Data, QString Password)
{
    // PEM er tekst; vi må dekode tilbake til bytes før vi sender til Botan
    const QByteArray pemBytes = Data.toLatin1(); // PEM er ASCII
    const std::string passwordStd = Password.toUtf8().toStdString();

    const std::string str = Botan::CryptoBox::decrypt(
        reinterpret_cast<const Botan::byte*>(pemBytes.constData()),
        static_cast<size_t>(pemBytes.size()),
        passwordStd
        );

    // "str" er plaintext som du tidligere tolket som std::string -> QString.
    // Hvis plaintext egentlig er bytes (ikke nødvendigvis tekst), bør du ikke ha QString-varianten.
    return QString::fromUtf8(str.data(), static_cast<int>(str.size()));
}

QByteArray BotanWrapper::Decrypt(QByteArray Data, QString Password)
{
    const std::string passwordStd = Password.toUtf8().toStdString();

    const std::string str = Botan::CryptoBox::decrypt(
        reinterpret_cast<const Botan::byte*>(Data.constData()),
        static_cast<size_t>(Data.size()),
        passwordStd
        );

    return QByteArray(str.data(), static_cast<int>(str.size()));
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



