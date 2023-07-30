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
 *
 * Tokenize text from stdin (split at whitespace), write words to fulltext-index database.
 * Word will be related to given document id.
 */

#include <stdio.h>
#include <sysexits.h>

#include "lib/fulltext_index.h"
#include "lib/fulltext_index_mysql.h"
#include "lib/tokenizer.h"

#include "cutils/src/db_util.h"
#include "cutils/src/config_file.h"

#define DEFAULT_CONFIG_FILE "./config/default-config"

char *doc_id = NULL;

struct db_config db;

/**
 *
 */
void apply_config(char *section_name, char *name, char *value) {
    apply_db_config(&db, "mysql", section_name, name, value);
}

/**
 *
 */
void usage_message(int argc, char *argv[]) {
    printf("USAGE:\n");
    printf("%s <doc_id>\n", argv[0]);
}

/**
 *
 */
void add_word(const char *word) {
    fulltext_add_word(doc_id, word);
}

/**
 * 
 */
int main(int argc, char *argv[]) {

    if (argc < 2) {
        usage_message(argc, argv);
        return EX_USAGE;
    }

    doc_id = argv[1];
    if (is_null_or_empty(doc_id))
        fail(EX_USAGE, "Document id cannot be null or empty");

    memset(&db, 0, sizeof(struct db_config));
    read_config_file_from_cli_arg("-config", argc, argv, 1, DEFAULT_CONFIG_FILE,
                                  &apply_config);

    if (!fulltext_db_connect(db.host, db.user, db.password, db.db, db.port))
        return 0;
    memset(&db, 0, sizeof(struct db_config));

    tokenize(stdin, TOKENIZER_DEFAULT_DELIMITERS, INDEXER_MAX_LENGTH_WORD, &add_word);

    fulltext_db_disconnect();
}
