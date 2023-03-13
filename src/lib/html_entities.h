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

//TODO: Needs cleanup & revision

/*
 * Author: christian c8121 de
 */

#ifndef CTEXTUTILS_HTML_ENTITIES
#define CTEXTUTILS_HTML_ENTITIES

#include <string.h>
#include <ctype.h>

#include "html_entities_config.h"

#define BUF_SIZE 10

/**
 * Allow # 0-9 A-Z a-z
 * @return 1 if valid, 0 otherwise
 */
int is_valid_html_entity_name(char *name) {
    char *p = name;
    while (*p) {
        if (*p >= 65 && *p <= 90) //A-Z
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
const char *html_named_entity_get(char *name) {

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
 * Caller must free result
 */
char *html_entity_get(char *name) {

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
 * TODO: writes to stream, not very useful
 */
void html_entity_decode(char *s, FILE *out) {

    char *i = s;
    char *p = strchr(s, '&');
    char *e;

    char *name = NULL;

    while (p != NULL) {

        fprintf(out, "%.*s", (int) (p - i), i);

        e = strchr(p, ';');
        if (e == NULL) {
            //terminator missing
            fprintf(out, "%c", *p);
            i = p + 1;

        } else {

            if (name != NULL)
                free(name);
            name = malloc(e - p);
            snprintf(name, e - p, "%.*s%c", (int) (e - p - 1), p + 1, '\0');
            i = e + 1;

            if (!is_valid_html_entity_name(name)) {
                //invalid name, leave it as it is
                fprintf(out, "%c", *p);
                i = p + 1;
            } else {
                char *dec = html_entity_get(name);
                fprintf(out, "%s", dec);
                free(dec);
            }
        }

        p = i;
        p = strchr(p, '&');
    }

    if (*i)
        fprintf(out, "%s", i);

    if (name != NULL)
        free(name);
}

/**
 *
 */
void html_entities_decode(FILE *in, FILE *out) {

    char buf[BUF_SIZE];
    char *p = NULL;
    int c;

    while ((c = fgetc(in)) != EOF) {

        if (c == '&') {
            if (p == NULL)
                p = buf;
            *(p++) = c;
        } else if (c == ';' && p != NULL) {
            *(p++) = c;
            *p = '\0';
            html_entity_decode(buf, out);
            p = NULL;
        } else if (p == NULL) {
            fprintf(out, "%c", c);
        } else {
            *(p++) = c;
            if (p - buf >= BUF_SIZE) {
                *p = '\0';
                fprintf(out, "[\"%s\"]", buf);
                p = NULL;
            }
        }

    }
}

#endif //CTEXTUTILS_HTML_ENTITIES