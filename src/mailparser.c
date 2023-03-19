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

//to enable strcasestr, strptime, must be before including string.h
#define _GNU_SOURCE

#include <stdio.h>
#include <sysexits.h>

#include "lib/mime_message_parser.h"

#include "submodules/cutils/src/cli_args.h"

struct mime_header *curr_part_mime_headers = NULL;

/**
 *
 */
void usage_message(int argc, char *argv[]) {
    printf("USAGE:\n");
    printf("%s\n", argv[0]);
    printf("Reads message from stdin.\n");
}


/**
 *
 */
int __handle_message_line(struct mime_header *mime_headers, int read_state, const char *line) {

    if (read_state == MIME_MESSAGE_READ_BODY && curr_part_mime_headers != mime_headers) {
        curr_part_mime_headers = mime_headers;
        fprintf(stderr, "PART, content-type='%s'\n", get_header_attribute(mime_headers, "content-type", NULL));
        fprintf(stderr, "      name='%s'\n", get_header_attribute(mime_headers, "content-type", "name"));
        fprintf(stderr, "      filename='%s'\n", get_header_attribute(mime_headers, "content-disposition", "filename"));
        fprintf(stderr, "      encoding='%s'\n", get_header_attribute(mime_headers, "Content-Transfer-Encoding", NULL));
    }

    printf("%s", line);

    return 1;
}


/**
 * 
 */
int main(int argc, char *argv[]) {

    if (cli_has_opt("-help", argc, argv)) {
        usage_message(argc, argv);
        return EX_OK;
    }

    read_mime_message(stdin, &__handle_message_line);
}
