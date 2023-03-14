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

#ifndef CTEXTUTILS_INDEXER_MYSQL
#define CTEXTUTILS_INDEXER_MYSQL

#include <stdio.h>
#include <sysexits.h>

#include "fulltext_index.h"

#include <mysql/mysql.h>

#include "../submodules/cutils/src/mysql_util.h"
#include "../submodules/cutils/src/char_util.h"
#include "../submodules/cutils/src/util.h"

void *mysql = NULL;

struct __mysql_query {
    const char *get_sql;
    MYSQL_STMT *get_stmt;
    const char *add_sql;
    MYSQL_STMT *add_stmt;
    const char *update_sql;
    MYSQL_STMT *update_stmt;
};

struct __mysql_query *__mysql_word = NULL;
struct __mysql_query *__mysql_document = NULL;

/**
 *
 */
struct __mysql_query *__mysql_create_query(const char *get_sql, const char *add_sql, const char *update_sql) {
    struct __mysql_query *query = malloc(sizeof(struct __mysql_query));
    query->get_sql = str_copy(get_sql, strlen(get_sql));
    query->get_stmt = NULL;
    query->add_sql = str_copy(add_sql, strlen(add_sql));
    query->add_stmt = NULL;
    query->update_sql = str_copy(update_sql, strlen(update_sql));
    query->update_stmt = NULL;
    return query;
}

/**
 *
 */
void __mysql_create_queries() {

    __mysql_word = __mysql_create_query(
            "SELECT ID FROM WORD WHERE WORD=?;",
            "INSERT INTO WORD(WORD, COUNT) VALUES(?, 1);",
            "UPDATE WORD SET COUNT=COUNT+1 WHERE ID=?;"
    );

    __mysql_document = __mysql_create_query(
            "SELECT DOCUMENT FROM DOCUMENT_WORD WHERE DOCUMENT=? AND WORD=?;",
            "INSERT INTO DOCUMENT_WORD(DOCUMENT, WORD, COUNT) VALUES(?, ?, 1);",
            "UPDATE DOCUMENT_WORD SET COUNT=COUNT+1 WHERE DOCUMENT=? AND WORD=?;"
    );
}

/**
 *
 */
void __mysql_close_query(struct __mysql_query *query) {
    if (query->get_stmt != NULL)
        mysql_stmt_close(query->get_stmt);
    query->get_stmt = NULL;

    if (query->add_stmt != NULL)
        mysql_stmt_close(query->add_stmt);
    query->add_stmt = NULL;

    if (query->update_stmt != NULL)
        mysql_stmt_close(query->update_stmt);
    query->update_stmt = NULL;
}

/**
 * @return id if found, -1 on error, 0 if no data found
 */
unsigned long __mysql_get_word_id(const char *word) {

    if (!mysql_util_prepare_stmt(mysql, &__mysql_word->get_stmt, __mysql_word->get_sql))
        fail(EX_IOERR, "Failed to prepare statement");

    MYSQL_BIND *p = mysql_util_create_bind(MYSQL_TYPE_STRING, (void *) word);
    p->buffer_length = strnlen(word, INDEXER_MAX_LENGTH_WORD);

    unsigned long id = 0;
    MYSQL_BIND *r = mysql_util_create_bind(MYSQL_TYPE_LONG, &id);

    int ret = mysql_util_fetch_result(mysql, __mysql_word->get_stmt, p, r);
    if (ret == -1)
        fail(EX_IOERR, "Failed to execute statement");

    mysql_stmt_free_result(__mysql_word->get_stmt);
    free(r);
    free(p);

    if (ret == 0)
        return 0;

    return id;
}

/**
 * @return id on success, -1 on fail
 */
unsigned long __mysql_add_word(const char *word) {

    if (!mysql_util_prepare_stmt(mysql, &__mysql_word->add_stmt, __mysql_word->add_sql))
        fail(EX_IOERR, "Failed to prepare statement");

    MYSQL_BIND *p = mysql_util_create_bind(MYSQL_TYPE_STRING, (void *) word);
    p->buffer_length = strnlen(word, INDEXER_MAX_LENGTH_WORD);

    unsigned long id = mysql_util_execute_insert(mysql, __mysql_word->add_stmt, p);
    if (id == -1)
        fail(EX_IOERR, "Failed to execute statement");

    mysql_stmt_free_result(__mysql_word->add_stmt);
    free(p);

    return id;
}

/**
 * @return 1 on success, 0 on fail
 */
int __mysql_update_word_count(unsigned long word_id) {

    if (!mysql_util_prepare_stmt(mysql, &__mysql_word->update_stmt, __mysql_word->update_sql))
        fail(EX_IOERR, "Failed to prepare statement");

    MYSQL_BIND *p = mysql_util_create_bind(MYSQL_TYPE_LONG, &word_id);

    if (mysql_util_execute_insert(mysql, __mysql_word->update_stmt, p) == -1)
        fail(EX_IOERR, "Failed to execute statement");

    mysql_stmt_free_result(__mysql_word->update_stmt);
    free(p);

    return 1;
}


/**
* @return id on success, -1 on fail
*/
unsigned long __mysql_get_document_word_exists(unsigned long document_id, unsigned long word_id) {

    if (!mysql_util_prepare_stmt(mysql, &__mysql_document->get_stmt, __mysql_document->get_sql))
        fail(EX_IOERR, "Failed to prepare statement");

    MYSQL_BIND *p = mysql_util_create_binds(2);
    p[0].buffer_type = MYSQL_TYPE_LONG;
    p[0].buffer = &document_id;
    p[1].buffer_type = MYSQL_TYPE_LONG;
    p[1].buffer = &word_id;

    unsigned long id = 0;
    MYSQL_BIND *r = mysql_util_create_bind(MYSQL_TYPE_LONG, &id);

    int ret = mysql_util_fetch_result(mysql, __mysql_document->get_stmt, p, r);
    if (ret == -1)
        fail(EX_IOERR, "Failed to execute statement");

    mysql_stmt_free_result(__mysql_document->get_stmt);
    free(r);
    free(p);

    if (ret == 0)
        return 0;

    return id;
}

/**
 * @return id on success, -1 on fail
 */
unsigned long __mysql_add_document_word(unsigned long document_id, unsigned long word_id) {

    if (!mysql_util_prepare_stmt(mysql, &__mysql_document->add_stmt, __mysql_document->add_sql))
        fail(EX_IOERR, "Failed to prepare statement");

    MYSQL_BIND *p = mysql_util_create_binds(2);
    p[0].buffer_type = MYSQL_TYPE_LONG;
    p[0].buffer = &document_id;
    p[1].buffer_type = MYSQL_TYPE_LONG;
    p[1].buffer = &word_id;

    unsigned long id = mysql_util_execute_insert(mysql, __mysql_document->add_stmt, p);
    if (id == -1)
        fail(EX_IOERR, "Failed to execute statement");

    mysql_stmt_free_result(__mysql_document->add_stmt);
    free(p);

    return id;
}

/**
 * @return 1 on success, 0 on fail
 */
int __mysql_update_document_word_count(unsigned long document_id, unsigned long word_id) {

    if (!mysql_util_prepare_stmt(mysql, &__mysql_document->update_stmt, __mysql_document->update_sql))
        fail(EX_IOERR, "Failed to prepare statement");

    MYSQL_BIND *p = mysql_util_create_binds(2);
    p[0].buffer_type = MYSQL_TYPE_LONG;
    p[0].buffer = &document_id;
    p[1].buffer_type = MYSQL_TYPE_LONG;
    p[1].buffer = &word_id;

    if (mysql_util_execute_insert(mysql, __mysql_document->update_stmt, p) == -1)
        fail(EX_IOERR, "Failed to execute statement");

    mysql_stmt_free_result(__mysql_document->update_stmt);
    free(p);

    return 1;
}


/**
 * @return 1 on success, 0 on fail
 */
int indexer_db_add_word(unsigned long doc_id, const char *word) {

    unsigned long word_id = __mysql_get_word_id(word);
    if (word_id == 0)
        word_id = __mysql_add_word(word);
    else
        __mysql_update_word_count(word_id);

    if (__mysql_get_document_word_exists(doc_id, word_id) == 0)
        __mysql_add_document_word(doc_id, word_id);
    else
        __mysql_update_document_word_count(doc_id, word_id);

    return word_id > 0 ? 1 : 0;
}

/**
 *
 */
int indexer_db_connect(const char *host, const char *user, const char *pwd,
                       const char *db, unsigned int port) {

    if (mysql != NULL)
        return 0;

    mysql = mysql_init(NULL);
    if (mysql == NULL)
        fprintf(stderr, "Init failed: %s", mysql_error(mysql));

    if (mysql_real_connect(mysql, host, user, pwd, db, port, NULL, 0) == 0) {
        fprintf(stderr, "%s\n", mysql_error(mysql));
        mysql_close(mysql);
        return 0;
    }

    __mysql_create_queries();

    return 1;
}

/**
 *
 */
void indexer_db_disconnect() {
    mysql_close(mysql);
}

#endif //CTEXTUTILS_INDEXER_MYSQL