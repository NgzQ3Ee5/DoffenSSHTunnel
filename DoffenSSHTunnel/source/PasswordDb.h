#ifndef PASSWORDDB_H
#define PASSWORDDB_H

#include "pch.h"
#include "identifier.h"
#include "botan/botanwrapper.h"
#include <QJsonObject>

class PasswordDb : public QObject
{
    Q_OBJECT

public:
    void createEmptyFile(QString filename, QString password);
    void openFile(QString filename, QString password);
    void saveFile(QString password);
    void clear();
    unsigned int size() const;
    Identifier at(unsigned int index) const;
    void removeIdentifier(const QUuid &uuid);
    void addIdentifier(const Identifier &identifier);
    bool comparePassword(QString password);

    static PasswordDb *getInstance();
    static void kill();

signals:

public slots:

protected:
    void readLemontFile(QString password);
    void readBotanCryptoBoxFile(QString password);
    bool compareIdentifier(Identifier& ident1, Identifier& ident2);

protected:
    void setPassword(QString password);
    QString m_password;
    QString m_filename;
    QList<Identifier> m_idents;
    BotanWrapper m_bw;


private:
    PasswordDb(QObject *parent = 0);
    ~PasswordDb();
    //! The single instance of PasswordDb class.
    static PasswordDb *m_instance;

};

#endif // PASSWORDDB_H
