#include "botanwrapper.h"

#include <span>
#include <string>

#ifdef Q_OS_WIN
    #include "win/botan_all.h"
#endif

#ifdef Q_OS_MACOS
    #include "mac/botan_all.h"
#endif

#ifdef Q_OS_LINUX
    #include "linux/botan_all.h"
#endif

BotanWrapper::BotanWrapper(QObject* parent)
    : QObject(parent)
{
}

QString BotanWrapper::EncryptWithPassword(QString Data, QString Password)
{
    const QByteArray encrypted = EncryptWithPassword(Data.toUtf8(), Password);
    // CryptoBox returns PEM (ASCII) -> Latin1 is correct and explicit
    return QString::fromLatin1(encrypted.constData(), encrypted.size());
}

QByteArray BotanWrapper::EncryptWithPassword(QByteArray Data, QString Password)
{
    Botan::AutoSeeded_RNG rng;

    const std::string passwordStd = Password.toUtf8().toStdString();

    const std::string pem = Botan::CryptoBox::encrypt(
        reinterpret_cast<const Botan::byte*>(Data.constData()),
        static_cast<size_t>(Data.size()),
        passwordStd,
        rng);

    return QByteArray(pem.data(), static_cast<int>(pem.size()));
}

QString BotanWrapper::Decrypt(QString Data, QString Password)
{
    const QByteArray decrypted = Decrypt(Data.toLatin1(), Password); // PEM is ASCII
    return QString::fromUtf8(decrypted.constData(), decrypted.size());
}

QByteArray BotanWrapper::Decrypt(QByteArray Data, QString Password)
{
    const std::string passwordStd = Password.toUtf8().toStdString();

    const std::string pt = Botan::CryptoBox::decrypt(
        reinterpret_cast<const Botan::byte*>(Data.constData()),
        static_cast<size_t>(Data.size()),
        passwordStd);

    return QByteArray(pt.data(), static_cast<int>(pt.size()));
}

QString BotanWrapper::BCryptGenerate(QString Password)
{
    Botan::AutoSeeded_RNG rng;

    // bcrypt input should be bytes; UTF-8 is the most predictable cross-platform choice
    const std::string pass = Password.toUtf8().toStdString();

    const std::string hash = Botan::generate_bcrypt(pass, rng, 12 /*work factor*/);
    return QString::fromStdString(hash);
}

bool BotanWrapper::BCryptCheck(QString Password, QString Hash)
{
    const std::string pass = Password.toUtf8().toStdString();
    const std::string hash = Hash.toStdString();

    return Botan::check_bcrypt(pass, hash);
}
