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

#include "cJSON/cJSON.h"
#include "cJSON/cJSON.c"

#endif

/**
 *
 */
char *decode_header_value(const char *v) {
    if (v == NULL)
        return NULL;
    char *result;
    mu_rfc2047_decode("utf-8", v, &result);
    if (result == NULL)
        result = str_copy(v, strlen(v));
    return result;
}


/**
 * In a list if addresses, find the delimiter.
 * Respects quoted Names.
 */
char *__find_mail_address_delimiter(char *a, const char *delimiters) {

    char *e = a + strlen(a);

    char ignore_block_delimiter = 0;
    for (char *p = a; p < e; p++) {

        if (ignore_block_delimiter != 0 && *p == ignore_block_delimiter) {
            //End of delimited block
            ignore_block_delimiter = 0;
            continue;
        } else {
            char *delimiter = strchr(delimiters, *p);
            if (*p == '"' && delimiter == NULL) {
                ignore_block_delimiter = '"';
                continue;
            } else if (*p == '<' && delimiter == NULL) {
                ignore_block_delimiter = '>';
                continue;
            } else if (ignore_block_delimiter != 0) {
                //In delimited block
                continue;
            } else if (delimiter != NULL) {
                return p;
            }
        }
    }

    return NULL;
}

/**
 * Create JSON Object containing names and addresses.
 * Tries to handle several types of header formats, for example:
 *    To: Name, Name2 <email@address>
 *    To: Name, Name2 <email@address>; ...
 *    To: Name; Name; Name;
 */
cJSON *json_get_addresses(char *address) {

    if (address == NULL)
        return NULL;

    const char *list_delimiters = ",\0"; //RFC defined limiter for multiple addresses
    if (strchr(address, '@') == NULL)
        list_delimiters = ",;"; //Only names found (no addresses), accept additional delimiter;


    cJSON *ret = cJSON_CreateArray();

    char *e = address + strlen(address);
    char *s = address;

    while (s < e) {

        cJSON *item = cJSON_CreateObject();
        cJSON_AddItemToArray(ret, item);

        char *p = __find_mail_address_delimiter(s, list_delimiters);
        if (p == NULL)
            p = e;
        *p = '\0';

        char *adr = __find_mail_address_delimiter(s, "<("); //Split 'Name <address>'
        if (adr != NULL) {
            *(adr++) = '\0';
            char *max = __find_mail_address_delimiter(adr, ">)");
            if (max != NULL)
                *max = '\0';
            ltrim(adr, " \r\n\t<(");
            rtrim(adr, " \r\n\t>)");
            cJSON_AddStringToObject(item, "address", adr);

            ltrim(s, " \r\n\t\"'");
            rtrim(s, " \r\n\t\"'");
            cJSON_AddStringToObject(item, "name", s);
        } else {
            ltrim(s, " \r\n\t\"'");
            rtrim(s, " \r\n\t\"'");
            if (strchr(address, '@') != NULL)
                cJSON_AddStringToObject(item, "address", s);
            else
                cJSON_AddStringToObject(item, "name", s);
        }

        s = p + 1;
    }

    return ret;
}


#endif //CTEXTUTILS_MIME_MESSAGE_UTIL