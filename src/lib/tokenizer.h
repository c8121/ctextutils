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

#ifndef CTEXTUTILS_TOKENIZER
#define CTEXTUTILS_TOKENIZER

#define TOKENIZER_DEFAULT_DELIMITERS " \r\n\t\f"

#include <stdio.h>
#include <sysexits.h>

#include "../submodules/cutils/src/util.h"

/**
 *
 */
int is_delimiter(int c, const char *delimiters) {
    char *d = (char *) delimiters;
    while (*d) {
        if (c == *d)
            return 1;
        d++;
    }
    return 0;
}

/**
 *
 */
void tokenize(FILE *input, const char *delimiters, size_t max_token_length,
              void (*next_token_function)(const char *token)) {

    if (max_token_length < 1)
        fail(EX_USAGE, "Token length < 1");


    char token[max_token_length];
    char *p = token;

    int c;
    while ((c = fgetc(input)) != EOF) {
        if (is_delimiter(c, delimiters)) {
            if (p - token > 0) {
                *p = '\0';
                next_token_function(token);
            }
            p = token;
        } else {
            *(p++) = c;
        }

        if (p - token >= max_token_length) {
            *p = '\0';
            next_token_function(token);
            p = token;
        }
    }

    if (p - token > 0) {
        *p = '\0';
        next_token_function(token);
    }
}

#endif //CTEXTUTILS_TOKENIZER