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
 *
 * Soundex implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

#include "cutils/src/cli_args.h"
#include "cutils/src/util.h"

#include "lib/soundex.h"

/**
 *
 */
void usage_message(int argc, char *argv[]) {
    printf("USAGE:\n");
    printf("%s [-help] <word> [Mapping: EN|DE]\n", argv[0]);
}

/**
 * 
 */
int main(int argc, char *argv[]) {

    if (cli_has_opt("-help", argc, argv)) {
        usage_message(argc, argv);
        return EX_OK;
    }

    if (argc < 2) {
        usage_message(argc, argv);
        return EX_USAGE;
    }

    void *mapping = EN_MAPPING;
    if (argc > 2) {
        mapping = soundex_mapping(argv[2]);
        if (mapping == NULL)
            fail(EX_USAGE, "Unknown mapping");
    }

    char *out = soundex(argv[1], strlen(argv[1]), (const char **) mapping);
    if (out != NULL)
        printf("%s\n", out);
    freenn(out);
}
