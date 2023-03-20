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

#ifndef CTEXTUTILS_QUOTED_PRINTABLE
#define CTEXTUTILS_QUOTED_PRINTABLE

#include <string.h>
#include <ctype.h>

#define HEX "0123456789ABCDEF"

/**
 *
 */
int __hexval(int c) {
    if ('0' <= c && c <= '9')
        return c - '0';
    return 10 + c - 'A';
}

/**
 *
 */
void qp_decode(const char *s, size_t len, FILE *out) {

    char *p = (char *) s;
    char *n = p + len;
    while (p < n) {

        if (*p != '=')
            fprintf(out, "%c", *p++);
        else if (*(p + 1) == '\r' && *(p + 2) == '\n')
            p += 3;
        else if (*(p + 1) == '\n')
            p += 2;
        else if (!strchr(HEX, *(p + 1)))
            fprintf(out, "%c", *p++);
        else if (!strchr(HEX, *(p + 2)))
            fprintf(out, "%c", *p++);
        else {
            fprintf(out, "%c", 16 * __hexval(*(p + 1)) + __hexval(*(p + 2)));
            p += 3;
        }
    }
}

#endif //CTEXTUTILS_QUOTED_PRINTABLE