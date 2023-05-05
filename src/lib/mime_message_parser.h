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

#define MIME_MESSAGE_READ_HEADER 1
#define MIME_MESSAGE_READ_BODY 2
#define MIME_MESSAGE_READ_BOUNDARY 3

#include <string.h>

#include "cutils/src/util.h"
#include "cutils/src/file_util.h"
#include "cutils/src/char_buffer_util.h"

#include "content_types.h"

struct mime_header {
    char *name;
    char *value;
    struct mime_header *next;
    struct mime_header *parent; //link to first header of parent part
};

/**
 *
 */
void __mime_header_free(struct mime_header *header) {
    struct mime_header *curr = header;
    struct mime_header *f;
    while (curr != NULL) {
        f = curr;
        curr = curr->next;

        freenn(f->name);
        f->name = NULL;
        freenn(f->value);
        f->value = NULL;
        free(f);

        f->parent = NULL;
    }
}

/**
 * Parse header string:
 *   <name>: <value>
 * or
 *   <name>: <value>
 *      <value>...
 */
struct mime_header *__read_mime_part_header(struct char_buffer *header, struct mime_header *append_to) {

    char *s = char_buffer_copy(header);
    size_t len = strlen(s);
    s[len - 1] = '\0'; //strip newline;

    struct mime_header *mime_header = malloc(sizeof(struct mime_header));
    mime_header->next = NULL;
    mime_header->parent = NULL;

    char *p = strchr(s, ':');
    if (p != NULL) {
        mime_header->name = str_copy(s, p - s);
        for (p++; is_whitespace(*p); ++p);
        mime_header->value = str_copy(p, len - (p - s));
    } else {
        mime_header->name = str_copy(s, len);
        mime_header->value = NULL;
    }
    free(s);

    if (append_to != NULL) {
        append_to->next = mime_header;
        mime_header->parent = append_to->parent;
    }

    //printf("HEADER> name='%s', value='%s'\n", mime_header->name, mime_header->value);
    return mime_header;
}

/**
 * Check if given header has given name.
 * @return header or NULL
 */
struct mime_header *__is_mime_part_header(struct mime_header *header, const char *name) {
    if (header == NULL || header->name == NULL || header->value == NULL)
        return NULL;

    if (strlen(header->name) != strlen(name))
        return NULL;

    if (strcasestr(header->name, name) == header->name)
        return header;

    return NULL;
}

/**
 * Find attribute in header-value.
 * If attribute_name is null, find get first value delimited by ';'
 * @return Header value or NULL
 * Caller must free result
 */
char *__read_mime_part_header_attribute(struct mime_header *header, const char *name, const char *attribute_name) {

    if (__is_mime_part_header(header, name) != NULL) {

        //printf("%s %s> '%s'\n", name, attribute_name, header->value);

        size_t len = strlen(header->value);
        char *p;
        if (attribute_name != NULL) {
            char *find = str_cat(attribute_name, "=");
            p = strcasestr(header->value, find);
            if (p != NULL) {
                p += strlen(find);
                if (p >= (header->value + len))
                    p = NULL;
            }
            free(find);
        } else {
            p = header->value;
        }

        if (p != NULL) {
            char *end = strchr(p, ';');
            if (end == NULL)
                end = p + strlen(p);
            char *ret = str_copy(p, end - p);
            ltrim(ret, "\r\n\t\v\f ");
            rtrim(ret, "\r\n\t\v\f ");
            //printf("%s (%s)> '%s'\n", name, attribute_name, ret);
            return ret;
        }
    }
    return NULL;
}

/**
 * Check if given header is content-type header.
 * @return content-type or NULL
 */
char *__read_mime_part_header_content_type(struct mime_header *header) {
    char *content_type = __read_mime_part_header_attribute(header, "Content-Type", NULL);
    //if (content_type != NULL)
    //    printf("CONTENT-TYPE> '%s'\n", content_type);
    return content_type;
}

/**
 * Check if given header is content-type header and contains boundary
 * @return boundary or NULL
 */
char *__read_mime_part_header_boundary(struct mime_header *header) {
    char *boundary = __read_mime_part_header_attribute(header, "Content-Type", "boundary");
    if (boundary != NULL) {
        ltrim(boundary, "\r\n\t\v\f \"");
        rtrim(boundary, "\r\n\t\v\f \"");
    }
    return boundary;
}

/**
 * Check if given line marks next multipart
 * @return 1 if it marks next, 0 if not
 */
int is_boundary_next(const char *line, const char *boundary) {

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
int is_boundary_end(const char *line, const char *boundary) {

    if (!is_boundary_next(line, boundary))
        return 0;
    if (strncmp(line + 2 + strlen(boundary), "--", 2) != 0)
        return 0;

    return 1;
}

/**
 * Read mime-parts (checking multi-part messages)
 */
void __read_mime_part(FILE *in, struct mime_header *parent_headers, const char *read_until_boundary,
                      int (*handle_message_line)(struct mime_header *mime_headers, int read_state, const char *line)) {

    struct char_buffer *buf_header = NULL;
    struct mime_header *mime_headers = NULL;
    struct mime_header *curr_header = NULL;
    char *boundary = NULL;
    char *content_type = NULL;

    int reading_header = 1;
    char *line;
    size_t len;
    while ((line = freadline(in)) != NULL) {

        int is_bnd_end = is_boundary_end(line, read_until_boundary);
        int is_bnd_next = !is_bnd_end ? is_boundary_next(line, read_until_boundary) : 0;
        int state = reading_header
                    ? MIME_MESSAGE_READ_HEADER
                    : ((is_bnd_end || is_bnd_next) ? MIME_MESSAGE_READ_BOUNDARY : MIME_MESSAGE_READ_BODY);

        if (!handle_message_line(mime_headers, state, line))
            return;

        len = strlen(line);
        if (is_bnd_end) {
            //printf("BOUNDARY END/EXIT> '%s'\n", read_until_boundary);
            break;
        } else if (is_bnd_next) {
            //printf("BOUNDARY NEXT/EXIT> '%s'\n", read_until_boundary);
            __read_mime_part(in, parent_headers, read_until_boundary, handle_message_line);
            break;
        }

        if (reading_header && is_newline(line[0])) {
            //End of header block
            reading_header = 0;
            if (buf_header != NULL) { //Last header
                curr_header = __read_mime_part_header(buf_header, curr_header);
                if (mime_headers == NULL) {
                    mime_headers = curr_header;
                    mime_headers->parent = parent_headers;
                }
                if (content_type == NULL)
                    content_type = __read_mime_part_header_content_type(curr_header);
                if (boundary == NULL)
                    boundary = __read_mime_part_header_boundary(curr_header);
                char_buffer_free(buf_header);
            }
            continue;
        }

        if (reading_header) {
            if (buf_header == NULL) {
                buf_header = char_buffer_append(buf_header, line, len);
            } else if (line[0] == '\t') {
                //Header value continues, keep tab
                buf_header = char_buffer_append(buf_header, line, len);
            } else if (is_whitespace(line[0])) {
                //Header value continues, remove whitespace, unfold
                buf_header->curr->s[buf_header->curr->len - 1] = '\0';
                buf_header->curr->len -= 1;
                buf_header = char_buffer_append(buf_header, line + 1, len - 1);
            } else {
                curr_header = __read_mime_part_header(buf_header, curr_header);
                if (mime_headers == NULL) {
                    mime_headers = curr_header;
                    mime_headers->parent = parent_headers;
                }
                if (content_type == NULL)
                    content_type = __read_mime_part_header_content_type(curr_header);
                if (boundary == NULL)
                    boundary = __read_mime_part_header_boundary(curr_header);
                char_buffer_free(buf_header);
                buf_header = char_buffer_append(NULL, line, len);
            }
        } else if (is_boundary_end(line, boundary)) {
            //printf("END> '%s'", line);
            __mime_header_free(mime_headers);
            mime_headers = NULL;
        } else if (is_boundary_next(line, boundary)) {
            //printf("NEXT> '%s'", line);
            __read_mime_part(in, mime_headers, boundary, handle_message_line);
        } else {
            //printf("BODY> (%s) '%s'", content_type, line);
        }

        free(line);
    }

    __mime_header_free(mime_headers);
    freenn(boundary);
    freenn(content_type);
}

/**
 * Read mime message from *in.
 * Call *handle_message_line for each message-line
 * Stop reading if *handle_message_line returns 0
 */
void read_mime_message(FILE *in, int (*handle_message_line)(struct mime_header *mime_headers, int read_state,
                                                            const char *line)) {
    __read_mime_part(in, NULL, NULL, handle_message_line);
    //printf("EOM\n");
}


/**
 * Find header by name
 */
char *get_header_value(struct mime_header *mime_headers, const char *name) {
    struct mime_header *curr = mime_headers;
    while (curr != NULL) {
        if (__is_mime_part_header(curr, name) != NULL)
            return curr->value;

        curr = curr->next;
    }
    return NULL;
}

/**
 * Caller must free result
 */
char *get_header_attribute(struct mime_header *mime_headers, const char *name, const char *attribute_name) {
    struct mime_header *curr = mime_headers;
    char *value;
    while (curr != NULL) {
        value = __read_mime_part_header_attribute(curr, name, attribute_name);
        if (value != NULL)
            return value;
        curr = curr->next;
    }
    return NULL;
}

/**
 * Get the filename from headers content-disposition or content-type
 *
 * @param create_default If not null, a default name will created.
 *                       A matching extension will be determined from content type.
 *
 * Caller must free result;
 */
char *get_attachment_filename(struct mime_header *mime_headers, const char *default_name) {

    char *ret = NULL;

    char *filename = get_header_attribute(mime_headers, "content-disposition", "filename");
    if (filename == NULL)
        filename = get_header_attribute(mime_headers, "content-type", "name");

    if (filename == NULL && default_name != NULL) {

        char *content_type = get_header_attribute(mime_headers, "content-type", NULL);
        const char *ext = get_extension_by_content_type(content_type);
        if (ext != NULL)
            ret = str_cat(default_name, ext);

    } else {
        ret = str_copy(filename, strlen(filename));
        ltrim(ret, " \"\r\n\t\f");
        rtrim(ret, " \"\r\n\t\f");
    }

    return ret;
}

#endif //CTEXTUTILS_MIME_MESSAGE_PARSER