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

#ifndef TINY_PASSWORD_MANAGER_H
#define TINY_PASSWORD_MANAGER_H

#ifdef WIN32
#undef UNICODE
#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1
//#define _WIN32_WINNT 0x0502
#endif

#include "../identifier.h"
#include "../exceptions.h"

#include <string>
#include <vector>

#ifdef WIN32
#include <windows.h>
#endif

/*!
 * \class TinyPasswordManager
 * \brief This class manages the database of identifiers.
 *
 * The aim of this class is to create, load or alter a database. Thus, the 
 * TinyPasswordManager class uses SHA256 and AES classes.
 *
 * \author Gross David <gdavid.devel@gmail.com>
 * \date 06/20/2009
 */

class TinyPasswordManager {
private:
    //! Create a TinyPasswordManager object.
    /*!
     *  \sa getInstance()
     */ 
    TinyPasswordManager();
    
    //! Destroy a TinyPasswordManager object. 
    /*!
     *  \sa kill()
     */ 
    ~TinyPasswordManager();

public:
    //! Returns the single instance of TinyPasswordManager class.
    /*!
     *  Instanciate a TinyPasswordManager object during the first call 
     *  of this method, then returns it.
     *
     *  \return the single TinyPasswordManager instance.
     */
    static TinyPasswordManager *getInstance();
    
    //! Destroys the single instance of TinyPasswordManager class.
    static void kill();

    //! If the manager is being worked on a database.
    /*!
     *  \return true if the manager is being used, false in the other case.
     */
    bool isBeingUsed() const;
        
    //! Open an existing database.
    /*!
     *  \param filename the path of the existing database.
     */
    void openDatabase(const std::string &filename = std::string("pwd.db"));
    //throw(ExFileNotFound, ExMemoryAllocationFailed, ExFileErrorReading);
    
    //! Get the identifiers of the database.
    /*!
     *  Before calling this method, you must set the private AES Key with 
     *  the SetPrivateKey method().
     *
     *  \param filename the path of the existing database.
     *  \sa setPrivateKey()
     */
    void getIdentifiers();
    //throw(ExMemoryAllocationFailed, ExBadFileFormat);

    //! Get the number of identifiers in the current database.
    /*!
     *  \return the number of identifiers.
     */
    unsigned int size() const;

    //! Get an identifier of the database.
    /*!
     *  \param index the index of the identifier.
     *  \return the identifier.
     */
    Identifier &at(unsigned int index);
    //throw(ExIndexOutOfBounds);

    //! Get an identifier of the database.
    /*!
     *  \param index the index of the identifier.
     *  \return the identifier.
     */
    Identifier at(unsigned int index) const;
    //throw(ExIndexOutOfBounds);

    //! Set the private AES Key.
    /*!
     *  This method must be called before a call to GetIdentifiers() or AddIdentifier().
     *
     *  \param key the private AES Key.
     *  \sa getIdentifiers(), addIdentifier()
     */
    void setPrivateKey(const unsigned char key[32]);
	bool comparePrivateKey(const unsigned char key[32]);

private:
    //! Compare two Identifier objects.
    /*!
     *  \param ident1 the first Identifier object.
     *  \param ident2 the second Identifier object.
     *  \return true if they are identical, false in the other case.
     */
    static bool compareIdentifier(Identifier *ident1, Identifier *ident2);
    
    //! Add padding to a message.
    /*!
     *  \param message the message.
     *  \param length the length of the padding.
     */
    static void addPadding(char *message, unsigned int length);

    //! Reinitialize the manager
    void reinitialize();

    //! Vector of pointers of Identifier object.
    std::vector<Identifier *> m_idents;

    //! The ciphered buffer.
    unsigned char *m_cipheredBuff;

    //! The size of the current database.
    unsigned int m_filesize;

    //! The path of the current database.
    std::string m_filename;

    //! The private AES Key of the current database.
    unsigned char m_key[32];
    
    //! The single instance of TinyPasswordManager class.
    static TinyPasswordManager *m_instance;
};


#endif
