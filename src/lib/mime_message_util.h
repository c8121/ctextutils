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

#ifndef CTEXTUTILS_MIME_MESSAGE_UTIL
#define CTEXTUTILS_MIME_MESSAGE_UTIL

#include <mailutils/mime.h>

#ifndef cJSON__h

#include "../3rd/cjson/cJSON.h"
#include "../3rd/cjson/cJSON.c"

#endif

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
cJSON *json_get_addresses_(char *address) {

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



#endif //CTEXTUTILS_MIME_MESSAGE_UTIL