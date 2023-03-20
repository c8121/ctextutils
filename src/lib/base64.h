/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Author: christian c8121 de
 */

#ifndef CTEXTUTILS_BASE64
#define CTEXTUTILS_BASE64

#include "../submodules/cutils/src/char_util.h"

/**
 *
 */
unsigned int __base64_val(unsigned char c) {
    if ('A' <= c && c <= 'Z')
        return c - 'A';
    if ('a' <= c && c <= 'z')
        return 26 + c - 'a';
    if ('0' <= c && c <= '9')
        return 52 + c - '0';
    if (c == '+')
        return 62;
    if (c == '/')
        return 63;
    return 64;
}

/**
 *
 */
void base64_decode(unsigned char *s, size_t len, FILE *out) {
    unsigned int c, v;
    int have_bits;

    unsigned char *p = s;

    for (have_bits = 0; *p != '=' && (p - s) < len; p++) {

        if (is_whitespace(*p))
            continue;

        v = __base64_val(*p);

        switch (have_bits) {
            case 0:
                c = v;
                have_bits = 6;
                break;

            case 6:
                fprintf(out, "%c", (c << 2) | (v >> 4));
                c = v & 0x0F;
                have_bits = 4;
                break;

            case 4:
                fprintf(out, "%c", (c << 4) | (v >> 2));
                c = v & 0x03;
                have_bits = 2;
                break;

            case 2:
                fprintf(out, "%c", (c << 6) | v);
                c = 0;
                have_bits = 0;
                break;
        }
    }
}

#endif //CTEXTUTILS_BASE64