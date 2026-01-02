#ifndef BOTANWRAPPER_H
#define BOTANWRAPPER_H

#include <QObject>
#include <QByteArray>
#include <QString>

/**
 * BotanWrapper
 *
 * Minimal Qt-friendly wrapper around the Botan primitives used by DoffenSSHTunnel.
 *
 * Responsibilities:
 *  - Password-based encryption/decryption of arbitrary bytes using Botan::CryptoBox
 *    (PEM-encoded output for storage compatibility).
 *  - bcrypt hash generation and verification for the in-memory master password.
 *
 * Notes:
 *  - Botan 3.x no longer requires (or provides) global library initialization.
 *  - The API surface here is intentionally small and matches the call sites in PasswordDb.
 */
class BotanWrapper : public QObject
{
    Q_OBJECT

public:
    explicit BotanWrapper(QObject* parent = nullptr);

    // Convenience overloads for textual data (UTF-8 plaintext, ASCII PEM ciphertext)
    QString EncryptWithPassword(QString Data, QString Password);
    QString Decrypt(QString Data, QString Password);

    // Binary-safe overloads (single source of truth for crypto)
    QByteArray EncryptWithPassword(QByteArray Data, QString Password);
    QByteArray Decrypt(QByteArray Data, QString Password);

    // bcrypt helpers
    QString BCryptGenerate(QString Password);
    bool BCryptCheck(QString Password, QString Hash);
};

#endif // BOTANWRAPPER_H
