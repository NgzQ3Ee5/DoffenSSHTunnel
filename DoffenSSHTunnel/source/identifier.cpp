#include "identifier.h"

Identifier::Identifier() {
	m_login = "";
	m_password = "";
	m_description = "";
}

QString Identifier::getLogin() const {
	return m_login;
}

QString Identifier::getPassword() const {
	return m_password;
}

QString Identifier::getDescription() const {
	return m_description;
}

void Identifier::setLogin(const QString &login) {
	m_login = login;
}

void Identifier::setPassword(const QString &password) {
	m_password = password;
}

void Identifier::setDescription(const QString &description) {
	m_description = description;
}

bool Identifier::operator==(const Identifier &identifier) {
    return m_login == identifier.m_login && m_password == identifier.m_password
           && m_description == identifier.m_description;
}

bool Identifier::operator!=(const Identifier &identifier) {
    return !(*this == identifier);
}

