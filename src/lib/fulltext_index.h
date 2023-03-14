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

#ifndef CTEXTUTILS_INDEXER
#define CTEXTUTILS_INDEXER

#define INDEXER_MAX_LENGTH_WORD 254

#include <stdio.h>
#include <sysexits.h>

int fulltext_db_connect(const char *host, const char *user, const char *pwd,
                        const char *db, unsigned int port);

void fulltext_db_disconnect();

int fulltext_db_add_word(unsigned long doc_id, const char *word);

unsigned long *fulltext_db_get_documents(int max, int word_count, const char *words[]);

/**
 *
 */
int fulltext_add_word(unsigned long doc_id, const char *word) {
    return fulltext_db_add_word(doc_id, word);
}

/**
 *
 */
unsigned long *fulltext_get_documents(int max, int word_count, const char *words[]) {
    return fulltext_db_get_documents(max, word_count, words);
}

#endif //CTEXTUTILS_INDEXER