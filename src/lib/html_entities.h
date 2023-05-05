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

#ifndef CTEXTUTILS_HTML_ENTITIES
#define CTEXTUTILS_HTML_ENTITIES

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "html_entities_config.h"

#define DECODE_BUF_SIZE 10

/**
 * Allow # 0-9 A-Z a-z
 * @return 1 if valid, 0 otherwise
 */
int is_valid_html_entity_name(const char *name) {
    char *p = (char *) name;
    while (*p) {
        if (*p == 35)
            p++;
        else if (*p >= 65 && *p <= 90) //A-Z
            p++;
        else if (*p >= 97 && *p <= 122) //a-z
            p++;
        else if (*p >= 48 && *p <= 57) //0-9
            p++;
        else
            return 0;
    }
    return 1;
}

/**
 * Take a entity name and return decoded UTF-8.
 * Parameter name can be a named entity
 */
const char *html_named_entity_get(const char *name) {

    for (int i = 0; *html_named_entities[i] != '\0'; i += 2) {
        if (strcmp(name, html_named_entities[i]) == 0)
            return html_named_entities[i + 1];
    }

    fprintf(stderr, "Entity not found: %s\n", name);
    return NULL;

}

/**
 * Take a entity name and return decoded UTF-8.
 * Parameter name can be a named entity or a number like #NNN #xNNN
 *
 * @return decoded entity, NULL on fail.
 * Caller must free result
 */
char *html_entity_get(const char *name) {

    if (!is_valid_html_entity_name(name))
        return NULL;

    char *dec = malloc(5);
    *dec = '\0';

    int i;

    if (*name == '#') {

        if (tolower(*(name + 1)) == 'x') //Hex number
            i = strtol(name + 2, 0, 16);
        else
            i = atoi(name + 1);


    } else {
        const char *e = html_named_entity_get(name);
        if (e != NULL)
            i = atoi(e);
        else
            i = 216; //O slash
    }

    if (i < 0x80) {
        *dec = i;
        *(dec + 1) = '\0';
    } else if (i < 0x800) {
        *dec = 192 + i / 64;
        *(dec + 1) = 128 + i % 64;
        *(dec + 2) = '\0';
    } else if (i < 0x10000) {
        *dec = 224 + i / 4096;
        *(dec + 1) = 128 + i / 64 % 64;
        *(dec + 2) = 128 + i % 64;
        *(dec + 3) = '\0';
    } else if (i < 0x110000) {
        *dec = 240 + i / 262144;
        *(dec + 1) = 128 + i / 4096 % 64;
        *(dec + 2) = 128 + i / 64 % 64;
        *(dec + 3) = 128 + i % 64;
        *(dec + 4) = '\0';
    }

    return dec;
}

/**
 *
 */
void html_entities_decode(FILE *in, FILE *out) {

    char buf[DECODE_BUF_SIZE];
    char *p = NULL;
    int c;

    char *dec = NULL;

    while ((c = fgetc(in)) != EOF) {

        if (c == '&') {
            if (p == NULL)
                p = buf;
        } else if (c == ';' && p != NULL) {
            *p = '\0';
            dec = html_entity_get(buf);
            if (dec != NULL) {
                fprintf(out, "%s", dec);
                free(dec);
            } else {
                fprintf(out, "%c%s%c", '&', buf, ';');
            }
            p = NULL;
        } else if (p == NULL) {
            fprintf(out, "%c", c);
        } else {
            *(p++) = c;
            if (p - buf >= DECODE_BUF_SIZE) {
                *p = '\0';
                fprintf(out, "%c%s", '&', buf);
                p = NULL;
            }
        }
    }
}

#endif //CTEXTUTILS_HTML_ENTITIES