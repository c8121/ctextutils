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
#include <sysexits.h>

#include "lib/mime_message_parser.h"
#include "lib/mime_message_util.h"
#include "lib/quoted_printable.h"
#include "lib/base64.h"

#include "cutils/src/cli_args.h"
#include "cutils/src/file_util.h"

#define MAX_OUTFILE_NAME_LENGTH 4096

struct mime_header *curr_part_mime_headers = NULL;

char *out_dir;
char *base_file_name = "message-content";

int file_num = 0;
FILE *curr_file = NULL;

int num_alternative = 0;

char **ignore_contentype;
int ignore_contentype_first = -1;
int ignore_contentype_last = -1;

void (*decode_print)(const char *line) = NULL;

/**
 *
 */
void usage_message(int argc, char *argv[]) {
    printf("USAGE:\n");
    printf("%s <output directory> [-n <base file name>] [-ignore <contentype (part of name)> <...>]\n", argv[0]);
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
 * @return 1 if to be ignored, 0 otherwise
 */
int __ignore_content_type(const char *content_type) {

    if (content_type == NULL)
        return 1;

    for (int i = ignore_contentype_first; i <= ignore_contentype_last; i++) {
        if (strcasestr(content_type, ignore_contentype[i]) != NULL)
            return 1;
    }

    return 0;
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

        if (curr_part_mime_headers == NULL) {
            //Main headers
            char *subject = decode_header_value(get_header_value(mime_headers, "Subject"));
            if (subject != NULL)
                printf("%s\n\n", subject);
            freenn(subject);
        }

        curr_part_mime_headers = mime_headers;

        char *content_type = get_header_attribute(mime_headers, "content-type", NULL);
        //fprintf(stderr, "PART, content-type='%s'\n", content_type);

        if (!__ignore_content_type(content_type)) {
            if (mime_headers->parent != NULL) {
                char *parent_content_type = get_header_attribute(mime_headers->parent, "content-type", NULL);
                if (strcasestr(parent_content_type, "multipart/alternative") != NULL)
                    num_alternative++;
                else
                    num_alternative = 0;
            } else {
                num_alternative = 0;
            }

            char *encoding = get_header_attribute(mime_headers, "Content-Transfer-Encoding", NULL);
            //fprintf(stderr, "      encoding='%s'\n", encoding);

            if (num_alternative < 2 && content_type != NULL && strcasestr(content_type, "multipart/") == NULL) {
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

                char *orig_filename = get_attachment_filename(mime_headers, "unnamed");

                char filename[MAX_OUTFILE_NAME_LENGTH];
                char *ext = file_ext(orig_filename, 10, "");

                do {
                    snprintf(filename, MAX_OUTFILE_NAME_LENGTH, "%s/%s%i.%s", out_dir, base_file_name, ++file_num, ext);
                    if (file_num > 1000)
                        fail(EX_IOERR, "Counted to %i, still no free filename... giving up");
                } while (file_exists(filename));


                fprintf(stderr, "Create file: %s (%s, %s, '%s')\n",
                        filename, content_type,
                        encoding != NULL ? encoding : "none",
                        orig_filename != NULL ? orig_filename : "none"
                );
                curr_file = fopen(filename, "w");

                freenn(ext);
                freenn(orig_filename);
            }

            freenn(encoding);
            freenn(content_type);
        } else {
            fprintf(stderr, "Ignoring part of content-type '%s'\n", content_type);
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

    ignore_contentype = argv;
    ignore_contentype_first = cli_get_opt_idx("-ignore", argc, argv);
    if (ignore_contentype_first > 0) {
        ignore_contentype_last = argc - 1;
        for (i = ignore_contentype_first; i < argc; i++) {
            if (argv[i][0] == '-') {
                ignore_contentype_last = i - 1;
                break;
            }
        }
    }

    read_mime_message(stdin, &__handle_message_line);

    if (curr_file != NULL)
        fclose(curr_file);
}
