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

#include "lib/tokenizer.h"

#include "submodules/cutils/src/cli_args.h"

#define DEFAULT_MAX_TOKEN_LENGTH 128
#define DEFAULT_TOKEN_SEPARATOR " ";

char *__tokenizer_separator = DEFAULT_TOKEN_SEPARATOR;
int __tokenizer_token_count = 0;

/**
 *
 */
void usage_message(int argc, char *argv[]) {
    printf("USAGE:\n");
    printf("%s [-help] [-d <delimiters>] [-m <max token length>] [-s <separator>]\n", argv[0]);
}

/**
 *
 */
void print_token(const char *token) {
    if (__tokenizer_token_count++ > 0)
        printf("%s", __tokenizer_separator);
    printf("%s", token);
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

    int max_token_length = DEFAULT_MAX_TOKEN_LENGTH;
    opt = cli_get_opt_idx("-m", argc, argv);
    if (opt > 0) {
        max_token_length = atoi(argv[opt]);
    }

    opt = cli_get_opt_idx("-s", argc, argv);
    if (opt > 0) {
        __tokenizer_separator = argv[opt];
    }

    tokenize(stdin, delimiters, max_token_length, &print_token);
    printf("\n");
}
