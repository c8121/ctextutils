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

#ifndef CTEXTUTILS_HTML_STRIP_TAGS
#define CTEXTUTILS_HTML_STRIP_TAGS

#include <ctype.h>

#include "cutils/src/char_util.h"

#define HTML_STRIP_TAGS_INGORE_CONTENT_OF "!-- script style"


/**
 * 
 */
int ignore_content(char *tag_name) {

    const char *p = HTML_STRIP_TAGS_INGORE_CONTENT_OF;
    int len = strlen(p);

    while (p - HTML_STRIP_TAGS_INGORE_CONTENT_OF < len) {

        const char *e = strchr(p, ' ');
        if (e == NULL)
            e = p + len;

        if (strncmp(tag_name, p, e - p) == 0) {
            //printf("ignore(%s)", tagname);
            return 1;
        }

        p = e + 1;
    }


    return 0;
}

/**
 * 
 */
void html_strip_tags(FILE *in, FILE *out) {

    int in_tag = 0;
    int in_tag_name = 0;
    char tag_name[255];
    char *tag_name_p = tag_name;

    int ignore_tag_content = 0;

    int c;
    int last_printed = 0;

    while ((c = fgetc(in)) != EOF) {

        if (c == '<') {

            in_tag = 1;
            in_tag_name = 1;
            tag_name_p = tag_name;
            ignore_tag_content = 0;

            if (!is_whitespace(last_printed))
                fprintf(out, " ");

        } else if (c == '>') {

            if (in_tag_name) {
                *(tag_name_p++) = '\0';
                //printf("(%s)", tag_name);
                ignore_tag_content = ignore_content(tag_name);
            }
            in_tag = 0;
            in_tag_name = 0;

        } else if (in_tag && is_whitespace(c) && in_tag_name) {

            in_tag_name = 0;
            *(tag_name_p++) = '\0';
            //printf("(%s)", tag_name);
            ignore_tag_content = ignore_content(tag_name);

        } else if (in_tag_name) {

            *(tag_name_p++) = tolower(c);

        } else if (!in_tag) {

            if (!ignore_tag_content) {
                fprintf(out, "%c", c);
                last_printed = c;
            }
        }
    }
}

#endif //CTEXTUTILS_HTML_STRIP_TAGS