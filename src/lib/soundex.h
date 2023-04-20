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

#ifndef CTEXTUTILS_SOUNDEX
#define CTEXTUTILS_SOUNDEX

#include <ctype.h>

#include "cutils/src/char_util.h"
#include "unicode_simplify.h"

const char *EN_MAPPING[] = {
        "B", "1",
        "C", "2",
        "D", "3",
        "F", "1",
        "G", "2",
        "J", "2",
        "K", "2",
        "L", "4",
        "M", "5",
        "N", "5",
        "P", "1",
        "Q", "2",
        "R", "6",
        "S", "2",
        "T", "3",
        "V", "1",
        "X", "2",
        "Z", "2",

        //List termination
        "\0", "\0"
};

const char *DE_MAPPING[] = {

        "B", "1",
        "CH", "7",
        "C", "2",
        "D", "3",
        "F", "1",
        "G", "2",
        "K", "2",
        "L", "4",
        "M", "5",
        "N", "5",
        "P", "1",
        "R", "6",
        "S", "2",
        "ß", "2",
        "T", "3",
        "V", "1",
        "W", "1",
        "X", "2",
        "Z", "2",

        //List termination
        "\0", "\0"
};

/**
 * @param name Can be EN od DE
 */
const char **soundex_mapping(const char *name) {
    if (strncasecmp(name, "DE", 2) == 0)
        return DE_MAPPING;
    else if (strncasecmp(name, "EN", 2) == 0)
        return EN_MAPPING;
    else
        return NULL;
}

/**
 * Caller must free result
 */
char *soundex(const char *in, size_t len, const char *mapping[]) {

    if (is_null_or_empty(in))
        return NULL;

    char *simplified = unicode_simplify(in, 0, len);

    char *out = malloc(5);
    snprintf(out, 5, "%c000", toupper(simplified[0]));

    int o = 1;
    for (int i = 1; i < len && o < 4;) {
        int match = 0;
        for (int c = 0; *mapping[c] != '\0'; c += 2) {
            int mlen = strlen(mapping[c]);
            if (strncasecmp(simplified + i, mapping[c], mlen) == 0) {
                out[o++] = *mapping[c + 1];
                i += mlen;
                match = 1;
                break;
            }
        }
        if (!match)
            i++;
    }

    free(simplified);

    return out;

}


#endif //CTEXTUTILS_SOUNDEX