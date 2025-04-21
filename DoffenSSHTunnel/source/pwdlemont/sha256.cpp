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

#include "sha256.h"

#include <sstream>
#include <iostream>
#include <cstring>

#define SHA256_DATA_LENGTH    16

#define ror(x,n) ( ((x) >> (n)) | ((x) << (32-(n))) )
#define rol(x,n) ( ((x) << (n)) | ((x) >> (32-(n))) )

#define STRING2INT(s) ((((((EXTRACT_UCHAR(s) << 8) | EXTRACT_UCHAR(s+1)) << 8) | EXTRACT_UCHAR(s+2)) << 8) | EXTRACT_UCHAR(s+3))

#define ROTR(n,x) (ror(x,n))
#define SHR(n,x) ((x)>>(n))
#define EXTRACT_UCHAR(p) (*(unsigned char *)(p))

#define Choice(x,y,z) ((z) ^ ((x) & ((y) ^ (z))))
#define Majority(x,y,z) (((x) & (y)) ^ ((z) & ((x) ^ (y))))

#define S0(x) (ROTR(2,(x)) ^ ROTR(13,(x)) ^ ROTR(22,(x)))
#define S1(x) (ROTR(6,(x)) ^ ROTR(11,(x)) ^ ROTR(25,(x)))

#define s0(x) (ROTR(7,(x)) ^ ROTR(18,(x)) ^ SHR(3,(x)))
#define s1(x) (ROTR(17,(x)) ^ ROTR(19,(x)) ^ SHR(10,(x)))

#define EXPAND(W,i) (W[(i) & 15] += (s1(W[((i)-2) & 15]) + W[((i)-7) & 15] + s0(W[((i)-15) & 15])))

#define ROUND(a, b, c, d, e, f, g, h, k, data) do { \
  unsigned int T1 = h + S1(e) + Choice(e,f,g) + k + data; \
  unsigned int T2 = S0(a) + Majority(a,b,c); \
  d += T1; \
  h = T1 + T2; \
} while (0)

static const unsigned int K[64] = {
    0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL,
    0x3956c25bUL, 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL,
    0xd807aa98UL, 0x12835b01UL, 0x243185beUL, 0x550c7dc3UL,
    0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL, 0xc19bf174UL,
    0xe49b69c1UL, 0xefbe4786UL, 0xfc19dc6UL, 0x240ca1ccUL,
    0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL,
    0x983e5152UL, 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL,
    0xc6e00bf3UL, 0xd5a79147UL, 0x6ca6351UL, 0x14292967UL,
    0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL, 0x53380d13UL,
    0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
    0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL,
    0xd192e819UL, 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL,
    0x19a4c116UL, 0x1e376c08UL, 0x2748774cUL, 0x34b0bcb5UL,
    0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL, 0x682e6ff3UL,
    0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
    0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL,
};

SHA256 *SHA256::m_instance = NULL;

SHA256 *SHA256::getInstance() {
    if(!m_instance)
        m_instance = new SHA256();

    return m_instance;
}

void SHA256::kill() {
    if(!m_instance) {
        delete m_instance;
        m_instance = NULL;
    }
}

void SHA256::init() {
    m_state[0] = 0x6a09e667;
    m_state[1] = 0xbb67ae85;
    m_state[2] = 0x3c6ef372;
    m_state[3] = 0xa54ff53a;
    m_state[4] = 0x510e527f;
    m_state[5] = 0x9b05688c;
    m_state[6] = 0x1f83d9ab;
    m_state[7] = 0x5be0cd19;

    m_count_low = m_count_high = 0;
    m_index = 0;
}

void SHA256::transform(unsigned int *data) {
    unsigned int a = m_state[0];
    unsigned int b = m_state[1];
    unsigned int c = m_state[2];
    unsigned int d = m_state[3];
    unsigned int e = m_state[4];
    unsigned int f = m_state[5];
    unsigned int g = m_state[6];
    unsigned int h = m_state[7];

    ROUND(a, b, c, d, e, f, g, h, K[0], data[0]);
    ROUND(h, a, b, c, d, e, f, g, K[1], data[1]);
    ROUND(g, h, a, b, c, d, e, f, K[2], data[2]);
    ROUND(f, g, h, a, b, c, d, e, K[3], data[3]);
    ROUND(e, f, g, h, a, b, c, d, K[4], data[4]);
    ROUND(d, e, f, g, h, a, b, c, K[5], data[5]);
    ROUND(c, d, e, f, g, h, a, b, K[6], data[6]);
    ROUND(b, c, d, e, f, g, h, a, K[7], data[7]);

    ROUND(a, b, c, d, e, f, g, h, K[8], data[8]);
    ROUND(h, a, b, c, d, e, f, g, K[9], data[9]);
    ROUND(g, h, a, b, c, d, e, f, K[10], data[10]);
    ROUND(f, g, h, a, b, c, d, e, K[11], data[11]);
    ROUND(e, f, g, h, a, b, c, d, K[12], data[12]);
    ROUND(d, e, f, g, h, a, b, c, K[13], data[13]);
    ROUND(c, d, e, f, g, h, a, b, K[14], data[14]);
    ROUND(b, c, d, e, f, g, h, a, K[15], data[15]);

    ROUND(a, b, c, d, e, f, g, h, K[16], EXPAND(data, 0));
    ROUND(h, a, b, c, d, e, f, g, K[17], EXPAND(data, 1));
    ROUND(g, h, a, b, c, d, e, f, K[18], EXPAND(data, 2));
    ROUND(f, g, h, a, b, c, d, e, K[19], EXPAND(data, 3));
    ROUND(e, f, g, h, a, b, c, d, K[20], EXPAND(data, 4));
    ROUND(d, e, f, g, h, a, b, c, K[21], EXPAND(data, 5));
    ROUND(c, d, e, f, g, h, a, b, K[22], EXPAND(data, 6));
    ROUND(b, c, d, e, f, g, h, a, K[23], EXPAND(data, 7));
    ROUND(a, b, c, d, e, f, g, h, K[24], EXPAND(data, 8));
    ROUND(h, a, b, c, d, e, f, g, K[25], EXPAND(data, 9));
    ROUND(g, h, a, b, c, d, e, f, K[26], EXPAND(data, 10));
    ROUND(f, g, h, a, b, c, d, e, K[27], EXPAND(data, 11));
    ROUND(e, f, g, h, a, b, c, d, K[28], EXPAND(data, 12));
    ROUND(d, e, f, g, h, a, b, c, K[29], EXPAND(data, 13));
    ROUND(c, d, e, f, g, h, a, b, K[30], EXPAND(data, 14));
    ROUND(b, c, d, e, f, g, h, a, K[31], EXPAND(data, 15));

    ROUND(a, b, c, d, e, f, g, h, K[32], EXPAND(data, 0));
    ROUND(h, a, b, c, d, e, f, g, K[33], EXPAND(data, 1));
    ROUND(g, h, a, b, c, d, e, f, K[34], EXPAND(data, 2));
    ROUND(f, g, h, a, b, c, d, e, K[35], EXPAND(data, 3));
    ROUND(e, f, g, h, a, b, c, d, K[36], EXPAND(data, 4));
    ROUND(d, e, f, g, h, a, b, c, K[37], EXPAND(data, 5));
    ROUND(c, d, e, f, g, h, a, b, K[38], EXPAND(data, 6));
    ROUND(b, c, d, e, f, g, h, a, K[39], EXPAND(data, 7));
    ROUND(a, b, c, d, e, f, g, h, K[40], EXPAND(data, 8));
    ROUND(h, a, b, c, d, e, f, g, K[41], EXPAND(data, 9));
    ROUND(g, h, a, b, c, d, e, f, K[42], EXPAND(data, 10));
    ROUND(f, g, h, a, b, c, d, e, K[43], EXPAND(data, 11));
    ROUND(e, f, g, h, a, b, c, d, K[44], EXPAND(data, 12));
    ROUND(d, e, f, g, h, a, b, c, K[45], EXPAND(data, 13));
    ROUND(c, d, e, f, g, h, a, b, K[46], EXPAND(data, 14));
    ROUND(b, c, d, e, f, g, h, a, K[47], EXPAND(data, 15));

    ROUND(a, b, c, d, e, f, g, h, K[48], EXPAND(data, 0));
    ROUND(h, a, b, c, d, e, f, g, K[49], EXPAND(data, 1));
    ROUND(g, h, a, b, c, d, e, f, K[50], EXPAND(data, 2));
    ROUND(f, g, h, a, b, c, d, e, K[51], EXPAND(data, 3));
    ROUND(e, f, g, h, a, b, c, d, K[52], EXPAND(data, 4));
    ROUND(d, e, f, g, h, a, b, c, K[53], EXPAND(data, 5));
    ROUND(c, d, e, f, g, h, a, b, K[54], EXPAND(data, 6));
    ROUND(b, c, d, e, f, g, h, a, K[55], EXPAND(data, 7));
    ROUND(a, b, c, d, e, f, g, h, K[56], EXPAND(data, 8));
    ROUND(h, a, b, c, d, e, f, g, K[57], EXPAND(data, 9));
    ROUND(g, h, a, b, c, d, e, f, K[58], EXPAND(data, 10));
    ROUND(f, g, h, a, b, c, d, e, K[59], EXPAND(data, 11));
    ROUND(e, f, g, h, a, b, c, d, K[60], EXPAND(data, 12));
    ROUND(d, e, f, g, h, a, b, c, K[61], EXPAND(data, 13));
    ROUND(c, d, e, f, g, h, a, b, K[62], EXPAND(data, 14));
    ROUND(b, c, d, e, f, g, h, a, K[63], EXPAND(data, 15));

    m_state[0] += a;
    m_state[1] += b;
    m_state[2] += c;
    m_state[3] += d;
    m_state[4] += e;
    m_state[5] += f;
    m_state[6] += g;
    m_state[7] += h;
}

void SHA256::block(const unsigned char *block) {
    unsigned int data[SHA256_DATA_LENGTH];

    if (!++m_count_low)
        ++m_count_high;

    for (int i = 0; i < SHA256_DATA_LENGTH; i++, block += 4)
        data[i] = STRING2INT(block);

    transform(data);
}

void SHA256::update(const unsigned char *buffer, unsigned int length) {
    if (m_index) {
        unsigned left = SHA256_DATA_SIZE - m_index;
        if (length < left) {
            memcpy(m_block + m_index, buffer, length);
            m_index += length;
            return;
        } else {
            memcpy(m_block + m_index, buffer, left);
            block(m_block);
            buffer += left;
            length -= left;
        }
    }

    while (length >= SHA256_DATA_SIZE) {
        block(buffer);
        buffer += SHA256_DATA_SIZE;
        length -= SHA256_DATA_SIZE;
    }

    memcpy(m_block, buffer, length);
    m_index = length;
}

void SHA256::final() {
    unsigned int data[SHA256_DATA_LENGTH];
    int i = m_index;

    m_block[i++] = 0x80;

    for (; i & 3; i++)
        m_block[i] = 0;

    int words = i >> 2;

    for (i = 0; i < words; i++)
        data[i] = STRING2INT(m_block + 4 * i);

    if (words > (SHA256_DATA_LENGTH - 2)) {
        for (i = words; i < SHA256_DATA_LENGTH; i++)
            data[i] = 0;

        transform(data);

        for (i = 0; i < (SHA256_DATA_LENGTH - 2); i++)
            data[i] = 0;
    }
    else
        for (i = words; i < SHA256_DATA_LENGTH - 2; i++)
            data[i] = 0;

    data[SHA256_DATA_LENGTH - 2] = (m_count_high << 9) | (m_count_low >> 23);
    data[SHA256_DATA_LENGTH - 1] = (m_count_low << 9) | (m_index << 3);

    transform(data);
}

void SHA256::digest(unsigned char *s) {
    for (unsigned int i = 0; i < SHA256_DIGEST_SIZE / 4; i++) {
        *s++ = m_state[i] >> 24;
        *s++ = 0xff & (m_state[i] >> 16);
        *s++ = 0xff & (m_state[i] >> 8);
        *s++ = 0xff & m_state[i];
    }
}

void SHA256::digest(const std::string &buf, unsigned char *dig) {
    init();
    update(reinterpret_cast<const unsigned char *>(buf.c_str()), static_cast<unsigned int>(buf.length()));
    final();
    digest(dig);
}


