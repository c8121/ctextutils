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

#ifndef CTEXTUTILS_MIME_MESSAGE_PARSER
#define CTEXTUTILS_MIME_MESSAGE_PARSER

//to enable strcasestr(...)
#define __USE_GNU


#include <string.h>

#include "../submodules/cutils/src/util.h"
#include "../submodules/cutils/src/file_util.h"
#include "../submodules/cutils/src/char_buffer_util.h"

struct mime_header {
    char *name;
    char *value;
};

/**
 *
 */
void __mime_header_free(struct mime_header *header) {
    if (header != NULL) {
        if (header->name != NULL)
            free(header->name);
        if (header->value != NULL)
            free(header->value);
    }
}

/**
 * Parse header string:
 *   <name>: <value>
 * or
 *   <name>: <value>
 *      <value>...
 */
struct mime_header *__read_mime_part_header(struct char_buffer *header) {

    char *s = char_buffer_copy(header);
    size_t len = strlen(s);
    s[len - 1] = '\0'; //strip newline;

    struct mime_header *mime_header = malloc(sizeof(struct mime_header));

    char *p = strchr(s, ':');
    if (p != NULL) {
        mime_header->name = str_copy(s, p - s);
        for (p++; is_whitespace(*p); ++p);
        mime_header->value = str_copy(p, len - (p - s));
    } else {
        mime_header->name = str_copy(s, p - s);
    }
    free(s);

    printf("HEADER> name='%s', value='%s'\n", mime_header->name, mime_header->value);
    return mime_header;
}

/**
 * Check if given header is content-type header.
 * @return content-type or NULL
 */
char *__read_mime_part_header_content_type(struct mime_header *header) {
    if (header == NULL || header->name == NULL || header->value == NULL)
        return NULL;

    if (strcasestr(header->name, "Content-Type") != NULL) {
        char *p = header->value;
        char *end = strchr(p, ';');
        if (end == NULL)
            for (end = p; !is_whitespace(*end); ++end);
        char *ret = str_copy(p, end - p);
        printf("CONTENT-TYPE> '%s'\n", ret);
        return ret;
    }
    return NULL;
}

/**
 * Check if given header is content-type header and contains boundary
 * @return boundary or NULL
 */
char *__read_mime_part_header_boundary(struct mime_header *header) {
    if (header == NULL || header->name == NULL || header->value == NULL)
        return NULL;

    if (strcasestr(header->name, "Content-Type") != NULL) {
        char *p = strcasestr(header->value, "boundary=");
        if (p != NULL) {
            for (p += 9; *p == '"'; ++p);
            size_t len = strlen(p);
            char *ret = str_copy(p, strlen(p));
            for (p = ret + (len - 1); p > ret && (is_whitespace(*p) || *p == '"'); p--)
                *p = '\0';
            printf("BOUNDARY> '%s'\n", ret);
            return ret;
        }
    }
    return NULL;
}

/**
 * Check if given line marks next multipart
 * @return 1 if it marks next, 0 if not
 */
int is_boundary_next(char *line, char *boundary) {

    if (boundary == NULL)
        return 0;
    if (strncmp(line, "--", 2) != 0)
        return 0;
    if (strncmp(line + 2, boundary, strlen(boundary)))
        return 0;

    return 1;
}

/**
 * Check if given line marks end of multipart
 * @return 1 if it marks end, 0 if not
 */
int is_boundary_end(char *line, char *boundary) {

    if (!is_boundary_next(line, boundary))
        return 0;
    if (strncmp(line + 2 + strlen(boundary), "--", 2) != 0)
        return 0;

    return 1;
}

/**
 * Read mime-parts (checking multi-part messages)
 */
void __read_mime_part(FILE *in) {

    struct char_buffer *buf_header = NULL;
    struct mime_header *mime_header = NULL;
    char *boundary = NULL;
    char *content_type = NULL;

    int reading_header = 1;
    char *line;
    size_t len;
    while ((line = freadline(in)) != NULL) {

        len = strlen(line);

        if (reading_header && is_newline(line[0])) {
            //End of header block
            reading_header = 0;
            if (buf_header != NULL) { //Last header
                if (mime_header != NULL)
                    free(mime_header);
                mime_header = __read_mime_part_header(buf_header);
                if (content_type == NULL)
                    content_type = __read_mime_part_header_content_type(mime_header);
                if (boundary == NULL)
                    boundary = __read_mime_part_header_boundary(mime_header);
                char_buffer_free(buf_header);
            }
        }

        if (reading_header) {
            if (buf_header == NULL || is_whitespace(line[0]))
                //Header value continues
                buf_header = char_buffer_append(buf_header, line, len);
            else {
                if (mime_header != NULL)
                    free(mime_header);
                mime_header = __read_mime_part_header(buf_header);
                if (boundary == NULL)
                    boundary = __read_mime_part_header_boundary(mime_header);
                char_buffer_free(buf_header);
                buf_header = char_buffer_append(NULL, line, len);
            }
        } else if (is_boundary_end(line, boundary)) {
            printf("END> '%s'", line);
        } else if (is_boundary_next(line, boundary)) {
            printf("NEXT> '%s'", line);
        } else {
            printf("BODY> (%s) '%s'", content_type, line);
        }
        free(line);
    }

    freenn(boundary);
    freenn(content_type);
}

/**
 *
 */
void read_mime_message(FILE *in) {
    __read_mime_part(in);
    printf("EOM\n");
}


#endif //CTEXTUTILS_TOKENIZER