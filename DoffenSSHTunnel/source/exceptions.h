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

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <string>
#include <exception>


class ExBad : public std::exception {
public:
    ExBad(const std::string &msg) {
        reason = msg;
    }

    ExBad(const ExBad &ex)
        : reason(ex.reason) {
    }

    virtual ~ExBad() throw() {}

    const char *what() const throw() {
        return reason.c_str();
    }

private:
    std::string reason;
};


class ExFileNotFound : public std::exception {
public:
    ExFileNotFound(const std::string &function, const std::string &file) {
        reason = function + " : couldn't find the following file : " + file;
    }

    ExFileNotFound(const ExFileNotFound &ex)
        : reason(ex.reason) {
    }

    virtual ~ExFileNotFound() throw() {}

    const char *what() const throw() {
        return reason.c_str();
    }

private:
    std::string reason;
};


class ExMemoryAllocationFailed : public std::exception {
public:
    ExMemoryAllocationFailed(const std::string &function) {
        reason = function + ": couldn't allocate memory";
    }

    ExMemoryAllocationFailed(const ExMemoryAllocationFailed &ex)
        : reason(ex.reason) {
    }

    virtual ~ExMemoryAllocationFailed() throw() {}

    const char *what() const throw() {
        return reason.c_str();
    }

private:
    std::string reason;
};

class ExFileErrorReading : public std::exception {
public:
    ExFileErrorReading(const std::string &function, const std::string &filename) {
        reason = function + ": couldn't read the following file : " + filename;
    }

    ExFileErrorReading(const ExFileErrorReading &ex)
        : reason(ex.reason) {
    }

    virtual ~ExFileErrorReading() throw() {};

    const char *what() const throw() {
        return reason.c_str();
    }

private:
    std::string reason;
};

class ExFileErrorWriting : public std::exception {
public:
    ExFileErrorWriting(const std::string &function, const std::string &filename) {
        reason = function + ": couldn't write into the following file : " + filename;
    }

    ExFileErrorWriting(const ExFileErrorWriting &ex)
        : reason(ex.reason) {
    }

    virtual ~ExFileErrorWriting() throw() {}

    const char *what() const throw() {
        return reason.c_str();
    }

private:
    std::string reason;
};

class ExBadFileFormat : public std::exception {
public:
    ExBadFileFormat(const std::string /*&function*/, const std::string &filename) {
        reason = /*function + ": " + */ filename + " hasn't the format expected. Wrong password ?";
    }

    ExBadFileFormat(const ExBadFileFormat &ex)
        : reason(ex.reason) {
    }

    virtual ~ExBadFileFormat() throw() {}

    const char *what() const throw() {
        return reason.c_str();
    }

private:
    std::string reason;
};

class ExBadOperatingSystem : public std::exception {
public:
    ExBadOperatingSystem(const std::string msg) {
        reason = msg;
    }

    ExBadOperatingSystem(const ExBadOperatingSystem &ex)
        : reason(ex.reason) {
    }

    virtual ~ExBadOperatingSystem() throw() {}

    const char *what() const throw() {
        return reason.c_str();
    }

private:
    std::string reason;
};

class ExIndexOutOfBounds : public std::exception {
public:
    ExIndexOutOfBounds(const std::string &function, const std::string &variable) {
        reason = function + ": index out of bounds on variable " + variable;
    }

    ExIndexOutOfBounds(const ExIndexOutOfBounds &ex)
        : reason(ex.reason) {
    }

    virtual ~ExIndexOutOfBounds() throw() {}

    const char *what() const throw() {
        return reason.c_str();
    }

private:
    std::string reason;
};

class ExNSSInternalError : public std::exception {
public:
    ExNSSInternalError(const std::string &function, const std::string &functionCalled) {
        reason = function + ": error when call function " + functionCalled;
    }

    ExNSSInternalError(const ExNSSInternalError &ex)
        : reason(ex.reason) {
    }

    virtual ~ExNSSInternalError() throw() {}

    const char *what() const throw() {
        return reason.c_str();
    }

private:
    std::string reason;
};

class ExSQLiteError : public std::exception {
public:
    ExSQLiteError(const std::string &function, const std::string &message) {
        reason = function + ": SQLite error - " + message;
    }

    ExSQLiteError(const ExSQLiteError &ex)
        : reason(ex.reason) {
    }

    virtual ~ExSQLiteError() throw() {}

    const char *what() const throw() {
        return reason.c_str();
    }

private:
    std::string reason;
};

class ExLibraryNotFound : public std::exception {
public:
    ExLibraryNotFound(const std::string &function, const std::string &library) {
        reason = function + ": library not found " + library;
    }

    ExLibraryNotFound(const ExLibraryNotFound &ex)
        : reason(ex.reason) {
    }

    virtual ~ExLibraryNotFound() throw() {}

    const char *what() const throw() {
        return reason.c_str();
    }

private:
    std::string reason;
};

#endif
