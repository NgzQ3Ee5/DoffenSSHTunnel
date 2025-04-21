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


#ifndef SHA256_H
#define SHA256_H

#include <string>

#define SHA256_DIGEST_SIZE 32
#define SHA256_DATA_SIZE 64

class SHA256 {
private:
    SHA256() {};
    ~SHA256() {};

public:
    static SHA256 *getInstance();
    static void kill();

    void digest(const std::string &buf, unsigned char *digest);

private:
    void init();
    void update(const unsigned char *data, unsigned int length);
    void final();
    void digest(unsigned char *s);
    void block(const unsigned char *block);
    void transform(unsigned int *data);

    unsigned int m_state[SHA256_DIGEST_SIZE >> 2];
    unsigned int m_count_low, m_count_high;
    unsigned char m_block[SHA256_DATA_SIZE];
    unsigned int m_index;

    static SHA256 *m_instance;
};

#endif // SHA256_H
