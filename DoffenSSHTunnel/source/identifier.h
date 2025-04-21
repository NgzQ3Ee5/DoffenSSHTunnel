#ifndef IDENTIFIER_H
#define IDENTIFIER_H

#include <QString>
#include <QUuid>

class Identifier 
{
public:
    explicit Identifier();
    QString getLogin() const;
    QString getPassword() const;
    QString getDescription() const;
    void setLogin(const QString &login);
    void setPassword(const QString &password);
    void setDescription(const QString &description);
	void setUuid(QUuid uuid) { m_uuid = uuid; }
	QUuid getUuid() { return m_uuid; }
    bool operator==(const Identifier &identifier);
    bool operator!=(const Identifier &identifier);

private:

    QString m_login;
    QString m_password;
    QString m_description;
	QUuid m_uuid;
};

#endif
