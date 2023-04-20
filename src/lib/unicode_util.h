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

/*
 *
 * Nice explanation from https://www.cprogramming.com/tutorial/unicode.html:
 *
 * UTF-8 is a specific scheme for mapping a sequence of 1-4 bytes to a number from 0x000000 to 0x10FFFF:
 *
 * 00000000 -- 0000007F: 	0xxxxxxx
 * 00000080 -- 000007FF: 	110xxxxx 10xxxxxx
 * 00000800 -- 0000FFFF: 	1110xxxx 10xxxxxx 10xxxxxx
 * 00010000 -- 001FFFFF: 	11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 *
 * The x's are bits to be extracted from the sequence and glued together to form the final number.
 *
 */

#ifndef CTEXTUTILS_UNICODE_UTIL
#define CTEXTUTILS_UNICODE_UTIL


void __test_handle_char(unsigned long int codepoint, char *out) {
    printf("(%li)", codepoint);
}

/**
 * Read each char, calculate code point and call *handle_char
 */
char *unicode_iterate(const char *s, size_t offset, size_t len,
                      void (*handle_char)(unsigned long int codepoint, char *out)) {

    char *out = malloc(len + 1);
    memset(out, 0, len + 1);
    char *out_p = out;

    char *p = (char *) s;
    char *end = p + len;

    unsigned long int p0, p1, p2, p3;

    while (p < end) {

        if ((*p & 0xF0) == 0xF0) {
            p0 = *p & 0x1F;
            p1 = *(p + 1) & 0x3F;
            p2 = *(p + 2) & 0x3F;
            p3 = *(p + 3) & 0x3F;
            (*handle_char)((p1 * 262144) + (p1 * 4096) + (p2 * 64) + p3, out_p);
            p += 4;
        } else if ((*p & 0xE0) == 0xE0) {
            p0 = *p & 0x1F;
            p1 = *(p + 1) & 0x3F;
            p2 = *(p + 2) & 0x3F;
            (*handle_char)((p0 * 4096) + (p1 * 64) + p2, out_p);
            p += 3;
        } else if ((*p & 0xC0) == 0xC0) {
            p0 = *p & 0x1F;
            p1 = *(p + 1) & 0x3F;
            (*handle_char)((p0 * 64) + p1, out_p);
            p += 2;
        } else {
            (*handle_char)(*p, out_p);
            p++;
        }

        out_p++;
    }

    return out;
}

#endif //CTEXTUTILS_UNICODE_UTIL