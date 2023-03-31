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

#ifndef cJSON__h

#include "cJSON/cJSON.h"
#include "cJSON/cJSON.c"

#endif

#include "lib/mime_message_parser.h"
#include "lib/mime_message_util.h"

#include "cutils/src/cli_args.h"
#include "cutils/src/time_util.h"

#define CHARSET "utf-8"

struct mime_header *curr_part_mime_headers = NULL;

/**
 *
 */
void usage_message(int argc, char *argv[]) {
    printf("USAGE:\n");
    printf("%s <file> \n", argv[0]);
}


/**
 *
 */
int __handle_message_line(struct mime_header *mime_headers, int read_state, const char *line) {

    if (curr_part_mime_headers != NULL)
        return 0; //Only read main header

    if (read_state == MIME_MESSAGE_READ_BODY) { //Header complete
        curr_part_mime_headers = mime_headers;

        cJSON *json = cJSON_CreateObject();

        cJSON_AddStringToObject(json, "ID", get_header_attribute(mime_headers, "Message-ID", NULL));

        char *subject = decode_header_value(get_header_attribute(mime_headers, "Subject", NULL));
        cJSON_AddStringToObject(json, "Subject", subject);
        freenn(subject);

        char *date = convert_valid_time_string(get_header_attribute(mime_headers, "Date", NULL));
        cJSON_AddStringToObject(json, "Date", date);
        freenn(date);

        char *from = decode_header_value(get_header_attribute(mime_headers, "From", NULL));
        cJSON *from_addr = json_get_addresses(from);
        if (from_addr != NULL && cJSON_GetArraySize(from_addr) > 0)
            cJSON_AddItemReferenceToObject(json, "From", cJSON_GetArrayItem(from_addr, 0));

        char *to = decode_header_value(get_header_attribute(mime_headers, "To", NULL));
        cJSON *to_addr = json_get_addresses(to);
        cJSON_AddItemReferenceToObject(json, "To", to_addr);

        char *s = cJSON_Print(json);
        printf("%s\n", s);
        free(s);
        cJSON_Delete(json);

        return 0;
    }

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

    if (cli_get_arg(1, argc, argv) == NULL) {
        usage_message(argc, argv);
        return EX_USAGE;
    } else {
        char *file_name = cli_get_arg(1, argc, argv);
        if (!file_exists(file_name))
            fail(EX_IOERR, "File not found");

        FILE *fp = fopen(file_name, "r");
        if (fp == NULL) {
            fprintf(stderr, "Failed to open file: %s\n", file_name);
            return 0;
        }

        read_mime_message(fp, &__handle_message_line);

        fclose(fp);
    }
}
