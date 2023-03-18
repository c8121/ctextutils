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

#include <mailutils/mime.h>

#include "lib/mime_message_parser.h"

#include "3rd/cjson/cJSON.h"
#include "3rd/cjson/cJSON.c"

#include "submodules/cutils/src/cli_args.h"
#include "submodules/cutils/src/time_util.h"

#define CHARSET "utf-8"

struct mime_header *curr_part_mime_headers = NULL;

/**
 *
 */
void usage_message(int argc, char *argv[]) {
    printf("USAGE:\n");
    printf("%s\n", argv[0]);
}

/**
 *
 */
char *decode_header_value(const char *v) {
    char *result;
    mu_rfc2047_decode("utf-8", v, &result);
    if (result == NULL)
        result = str_copy(v, strlen(v));
    return result;
}

/**
 * In a list if addresses, find the delimiting comma.
 * Respects quoted Names.
 */
char *__find_mail_address_delimiter(char *a) {

    char *e = a + strlen(a);

    int in_ignore_block = 0;
    for (char *p = a; p < e; p++) {
        if (in_ignore_block && (*p == '"' || *p == '>')) {
            in_ignore_block = 0;
            continue;
        } else if ((*p == '"' || *p == '<')) {
            in_ignore_block = 1;
            continue;
        } else if (in_ignore_block) {
            continue;
        } else if (*p == ',') {
            return p;
        }
    }

    return NULL;
}

/**
 * Create JSON Object containing names ans addresses
 */
cJSON *get_addresses(char *address) {

    cJSON *ret = cJSON_CreateArray();

    char *e = address + strlen(address);
    char *s = address;

    while (s < e) {

        cJSON *item = cJSON_CreateObject();
        cJSON_AddItemToArray(ret, item);

        char *p = __find_mail_address_delimiter(s);
        if (p == NULL)
            p = e;
        *p = '\0';

        char *adr = strchr(s, '<');
        if (adr != NULL) {
            *(adr++) = '\0';
            ltrim(adr, " \r\n\t");
            rtrim(adr, " \r\n\t>");
            cJSON_AddStringToObject(item, "address", adr);

            ltrim(s, " \r\n\t\"");
            rtrim(s, " \r\n\t\"");
            cJSON_AddStringToObject(item, "name", s);
        } else {
            cJSON_AddStringToObject(item, "address", s);
        }

        s = p + 1;
    }

    return ret;
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
        cJSON *from_addr = get_addresses(from);
        if (from_addr != NULL && cJSON_GetArraySize(from_addr) > 0)
            cJSON_AddItemReferenceToObject(json, "From", cJSON_GetArrayItem(from_addr, 0));

        char *to = decode_header_value(get_header_attribute(mime_headers, "To", NULL));
        cJSON *to_addr = get_addresses(to);
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

    read_mime_message(stdin, &__handle_message_line);
}
