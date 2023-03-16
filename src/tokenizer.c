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

#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <ctype.h>
#include <regex.h>

#include "lib/tokenizer.h"

#include "submodules/cutils/src/cli_args.h"

#define DEFAULT_MAX_TOKEN_LENGTH 128
#define DEFAULT_TOKEN_SEPARATOR " ";

char *__tokenizer_separator = DEFAULT_TOKEN_SEPARATOR;
int __tokenizer_token_count = 0;

size_t __max_token_length = DEFAULT_MAX_TOKEN_LENGTH;
size_t __min_token_length = 1;
int __lcase = 0;
regex_t *__match_regex = NULL;

/**
 *
 */
void usage_message(int argc, char *argv[]) {
    printf("USAGE:\n");
    printf("%s [-help] [-d <delimiters>] [-m <max token length>] [-n min token length] \\\n", argv[0]);
    printf("        [-lcase] \\\n");
    printf("        [-match <regex>] \\\n");
    printf("        [-s <separator>]\n");
}

/**
 *
 */
void print_token(const char *token) {

    if (strnlen(token, __max_token_length) < __min_token_length)
        return;

    if (__match_regex != NULL) {
        int ret = regexec(__match_regex, token, 0, NULL, 0);
        if (ret == REG_NOMATCH)
            return;
    }

    if (__tokenizer_token_count++ > 0)
        printf("%s", __tokenizer_separator);

    if (__lcase) {
        for (const char *p = token; *p; ++p)
            printf("%c", tolower(p[0]));
    } else {
        printf("%s", token);
    }
}

/**
 * 
 */
int main(int argc, char *argv[]) {

    if (cli_has_opt("-help", argc, argv)) {
        usage_message(argc, argv);
        return EX_OK;
    }

    char *delimiters = TOKENIZER_DEFAULT_DELIMITERS;
    int opt = cli_get_opt_idx("-d", argc, argv);
    if (opt > 0) {
        delimiters = argv[opt];
    }

    opt = cli_get_opt_idx("-s", argc, argv);
    if (opt > 0) {
        __tokenizer_separator = argv[opt];
    }

    opt = cli_get_opt_idx("-m", argc, argv);
    if (opt > 0) {
        __max_token_length = atoll(argv[opt]);
        if (__max_token_length < 1)
            fail(EX_USAGE, "Max token length < 1");
    }

    opt = cli_get_opt_idx("-n", argc, argv);
    if (opt > 0) {
        __min_token_length = atoll(argv[opt]);
        if (__min_token_length > __max_token_length)
            fail(EX_USAGE, "Min token length > max token length");
    }

    if (cli_has_opt("-lcase", argc, argv)) {
        __lcase = 1;
    }

    opt = cli_get_opt_idx("-match", argc, argv);
    if (opt > 0) {
        __match_regex = malloc(sizeof(regex_t));
        int ret = regcomp(__match_regex, argv[opt], 0);
        if (ret != 0)
            fail(EX_USAGE, "Invalid regex");
    }

    tokenize(stdin, delimiters, __max_token_length, &print_token);
    printf("\n");
}
