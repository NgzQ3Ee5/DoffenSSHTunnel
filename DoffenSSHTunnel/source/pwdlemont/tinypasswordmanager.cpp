/**
 *  TinyPasswordManager
 *  Copyright (C) 2008 Gross David <gdavid.devel@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "tinypasswordmanager.h"
#include "aes.h"
#include "sha256.h"
#include <fstream>
#include <cstring>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <QByteArray>

#ifdef WIN32
#define INITIALIZE_RAND() srand(GetTickCount());
//warning C4996: 'strtok': This function or variable may be unsafe. Consider using strtok_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS
//#pragma warning( disable : 4996 )
#else
extern "C" {
#include <sys/time.h>
}

#define INITIALIZE_RAND() {\
struct timeval tv;\
gettimeofday(&tv, NULL);\
srand(tv.tv_usec);\
}
#endif

TinyPasswordManager *TinyPasswordManager::m_instance = NULL;

TinyPasswordManager::TinyPasswordManager()
    : m_cipheredBuff(NULL), m_filesize(0), m_filename("") {
     memset(m_key, 0, 32);
}

TinyPasswordManager::~TinyPasswordManager() {
    Identifier *tmp = NULL;

    while(!m_idents.empty()) {
        tmp = m_idents.back();
        m_idents.pop_back();
        delete tmp;
        tmp = NULL;
    }

    if(m_cipheredBuff) {
        delete[] m_cipheredBuff;
        m_cipheredBuff = NULL;
    }
}

TinyPasswordManager *TinyPasswordManager::getInstance() {
    if(!m_instance)
        m_instance = new TinyPasswordManager();

    return m_instance;
}

void TinyPasswordManager::kill() {
    if(m_instance) {
        delete m_instance;
        m_instance = NULL;
    }
}

bool TinyPasswordManager::isBeingUsed() const {
    return !m_filename.empty();
}


void TinyPasswordManager::openDatabase(const std::string &filename) 
/*throw(ExFileNotFound, ExMemoryAllocationFailed, ExFileErrorReading)*/ {
    if(isBeingUsed() && !filename.compare(m_filename))
        return;
    else if(isBeingUsed() && filename.compare(m_filename))
        reinitialize();

    std::ifstream ifs(filename.c_str(), std::ios::in | std::ios::binary);

    if(!ifs.good())
        throw ExFileNotFound(Q_FUNC_INFO, filename);

    ifs.seekg(0, std::ios::end);
    m_filesize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    m_cipheredBuff = new unsigned char[m_filesize];

    if(!m_cipheredBuff)
        throw ExMemoryAllocationFailed(Q_FUNC_INFO);

    ifs.read(reinterpret_cast<char *>(m_cipheredBuff), m_filesize);

    if(!ifs.good())
        throw ExFileErrorReading(Q_FUNC_INFO, filename);

    m_filename = std::string(filename);
    ifs.close();
}

void TinyPasswordManager::getIdentifiers() 
/*throw(ExMemoryAllocationFailed, ExBadFileFormat)*/ {
    if(!isBeingUsed())
        return;

    AES aes;
    aes.SetParameters(256, 128);
    aes.StartDecryption(m_key);

    char *plaintext = new char[m_filesize];

    if(!plaintext)
        throw ExMemoryAllocationFailed(Q_FUNC_INFO);

    aes.Decrypt(m_cipheredBuff, reinterpret_cast<unsigned char *>(plaintext), m_filesize >> 4, AES::CBC);

    unsigned int sizePadding = plaintext[m_filesize - 1];
    if(sizePadding > 16)
        throw ExBadFileFormat(Q_FUNC_INFO, m_filename);

    plaintext[m_filesize - sizePadding] = 0;

    if(plaintext[0] != 'P' || plaintext[1] != 'M' || plaintext[2] != '1')
        throw ExBadFileFormat(Q_FUNC_INFO, m_filename);

    char* tmp = strtok_s(plaintext + 16, "\n", nullptr);

    if(!tmp)
        return;

    #define STAGE_LOGIN 0
    #define STAGE_PASSWORD 1
    #define STAGE_DESCRIPTION 2

    unsigned int stage = STAGE_LOGIN;
    Identifier *identifier = NULL;

    do {
        switch(stage % 3) {
        case STAGE_LOGIN:
            identifier = new Identifier();
            identifier->setLogin(tmp);
            break;

        case STAGE_PASSWORD:
            identifier->setPassword(tmp);
            break;

        case STAGE_DESCRIPTION:
            identifier->setDescription(tmp);
			identifier->setUuid(QUuid::createUuid());
            m_idents.push_back(identifier);
            break;
        }

        stage++;
    } while((tmp = strtok_s(nullptr, "\n", nullptr)));

    memset(plaintext, 0, m_filesize);
    delete[] plaintext;

    std::sort(m_idents.begin(), m_idents.end(), TinyPasswordManager::compareIdentifier);
}

unsigned int TinyPasswordManager::size() const {
    return static_cast<unsigned int>(m_idents.size());
}

void TinyPasswordManager::setPrivateKey(const unsigned char key[32]) {
    memcpy(m_key, key, 32);
}

bool TinyPasswordManager::comparePrivateKey(const unsigned char key[32]) {
    bool ret = true;
    QByteArray bt1;
    QByteArray bt2;
    for(int i=0;i<32;i++) {
        bt1.push_back(m_key[i]);
        bt2.push_back(key[i]);
    }
    if(bt1 != bt2) {
        ret = false;
    }
    return ret;
}

Identifier &TinyPasswordManager::at(unsigned int index)
/*throw(ExIndexOutOfBounds)*/ {
    try {
        return *(m_idents.at(index));
    }
    catch(std::out_of_range &) {
        throw ExIndexOutOfBounds(Q_FUNC_INFO, "m_idents");
    }
}

Identifier TinyPasswordManager::at(unsigned int index) const 
/*throw(ExIndexOutOfBounds)*/ {
    try {
        return *(m_idents.at(index));
    }
    catch(std::out_of_range&) {
        throw ExIndexOutOfBounds(Q_FUNC_INFO, "m_idents");
    }
}

bool TinyPasswordManager::compareIdentifier(Identifier *ident1, Identifier *ident2) {
    std::string descr1 = ident1->getDescription().toStdString();
    std::string descr2 = ident2->getDescription().toStdString();

    std::transform(descr1.begin(), descr1.end(), descr1.begin(), (int(*)(int))std::toupper);
    std::transform(descr2.begin(), descr2.end(), descr2.begin(), (int(*)(int))std::toupper);

    return (descr1.compare(descr2) > 0) ? false : true;
}

void TinyPasswordManager::addPadding(char *message, unsigned int length) {
    unsigned int i;
    unsigned int sizePadding = 16 - (length % 16);

    INITIALIZE_RAND()

    for(i = 0; i < sizePadding - 1; ++i)
        message[length + i] = static_cast<char>(rand());

    message[length + i] = static_cast<char>(sizePadding);
}

void TinyPasswordManager::reinitialize() {
    m_filename = "";
    m_filesize = 0;

    if(m_cipheredBuff) {
        delete[] m_cipheredBuff;
        m_cipheredBuff = NULL;
    }
}
