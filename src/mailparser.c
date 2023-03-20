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
#include "lib/quoted_printable.h"
#include "lib/base64.h"

#include "submodules/cutils/src/cli_args.h"

#define MAX_OUTFILE_NAME_LENGTH 4096

struct mime_header *curr_part_mime_headers = NULL;

char *out_dir;
char *base_file_name = "message-content";

int file_num = 0;
FILE *curr_file = NULL;

void (*decode_print)(const char *line) = NULL;

/**
 *
 */
void usage_message(int argc, char *argv[]) {
    printf("USAGE:\n");
    printf("%s <output directory> [-n <base file name>]\n", argv[0]);
    printf("Reads message from stdin.\n");
}

/**
 *
 */
void decode_base64_print(const char *line) {
    if (curr_file != NULL)
        base64_decode((unsigned char *) line, strlen(line), curr_file);
}

/**
 *
 */
void decode_qp_print(const char *line) {
    if (curr_file != NULL)
        qp_decode(line, strlen(line), curr_file);
}

/**
 *
 */
void decode_7bit_print(const char *line) {
    if (curr_file != NULL)
        fprintf(curr_file, "%s", line);
}

/**
 *
 */
void decode_8bit_print(const char *line) {
    if (curr_file != NULL)
        fprintf(curr_file, "%s", line);
}

/**
 *
 */
int __handle_message_line(struct mime_header *mime_headers, int read_state, const char *line) {

    if (read_state == MIME_MESSAGE_READ_BOUNDARY) {
        decode_print = NULL;
    } else if (read_state == MIME_MESSAGE_READ_BODY && curr_part_mime_headers != mime_headers) {

        if (curr_file != NULL) {
            fclose(curr_file);
            curr_file = NULL;
        }

        curr_part_mime_headers = mime_headers;

        char *content_type = get_header_attribute(mime_headers, "content-type", NULL);
        fprintf(stderr, "PART, content-type='%s'\n", content_type);

        char *encoding = get_header_attribute(mime_headers, "Content-Transfer-Encoding", NULL);
        fprintf(stderr, "      encoding='%s'\n", encoding);

        if (content_type != NULL && strcasestr(content_type, "multipart/") == NULL) {
            if (encoding != NULL) {
                if (strcasestr(encoding, "base64") != NULL)
                    decode_print = &decode_base64_print;
                else if (strcasestr(encoding, "quoted-printable") != NULL)
                    decode_print = &decode_qp_print;
                else if (strcasestr(encoding, "7bit") != NULL)
                    decode_print = &decode_7bit_print;
            } else {
                decode_print = &decode_8bit_print;
            }

            char *orig_filename = get_attachment_filename(mime_headers);

            char filename[MAX_OUTFILE_NAME_LENGTH];
            char *ext = file_ext(orig_filename, 10, "");

            do {
                snprintf(filename, MAX_OUTFILE_NAME_LENGTH, "%s/%s%i.%s", out_dir, base_file_name, ++file_num, ext);
                if (file_num > 1000)
                    fail(EX_IOERR, "Counted to %i, still no free filename... giving up");
            } while (file_exists(filename));


            printf("Create file: %s\n", filename);
            curr_file = fopen(filename, "w");
            freenn(ext);
        }

    }

    if (decode_print != NULL)
        (*decode_print)(line);

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

    if (argc < 2) {
        usage_message(argc, argv);
        return EX_USAGE;
    }

    out_dir = argv[1];
    if (is_null_or_empty(out_dir)) {
        usage_message(argc, argv);
        return EX_USAGE;
    }

    if (!dir_exists(out_dir))
        fail(EX_IOERR, "Output directory does not exist");

    int i = cli_get_opt_idx("-n", argc, argv);
    if (i > 0)
        base_file_name = argv[i];

    read_mime_message(stdin, &__handle_message_line);

    if (curr_file != NULL)
        fclose(curr_file);
}
