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

#define INDEXER_MAX_LENGTH_DOC_ID 254
#define INDEXER_MAX_LENGTH_WORD 254

#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

#include "cutils/src/sort_util.h"

int fulltext_db_connect(const char *host, const char *user, const char *pwd,
                        const char *db, unsigned int port);

void fulltext_db_disconnect();

int fulltext_db_add_word(const char *doc_user_id, const char *word);

struct fulltext_id_list *fulltext_db_get_documents(int word_count, const char *words[]);


struct fulltext_id_list {
    unsigned long id;
    unsigned long count;
    double score;
    struct fulltext_id_list *prev;
    struct fulltext_id_list *next;
};


/**
 * Required for usage of <code>linked_list_sort</code> to get next entry
 */
void *__fulltext_id_list_next(void *curr) { return ((struct fulltext_id_list *) curr)->next; }

/**
 * Required for usage of <code>linked_list_sort</code>.
 * Compare  fulltext_id_list->score, return 1 if a &gt; b, 0 otherwise
 */
int __fulltext_id_list_compare(void *a, void *b) {
    return ((struct fulltext_id_list *) b)->score > ((struct fulltext_id_list *) a)->score ? 1 : 0;
}

/**
 *  Required for usage of <code>linked_list_sort</code>.
 */
void __fulltext_id_list_move_after(void *move, void *after) {
    struct fulltext_id_list *m = (struct fulltext_id_list *) move;
    struct fulltext_id_list *a = (struct fulltext_id_list *) after;

    //Remove m
    if (m->prev != NULL)
        m->prev->next = m->next;
    if (m->next != NULL)
        m->next->prev = m->prev;


    //Append m
    if (a->next != NULL) {
        m->next = a->next;
        a->next->prev = m;
    } else {
        m->next = NULL;
    }
    a->next = m;
    m->prev = a;
}
/**
 * Sort by fulltext_id_list->score
 */
struct fulltext_id_list *fulltext_id_list_sort(struct fulltext_id_list *first) {
    return (struct fulltext_id_list *) linked_list_sort(
            first,
            &__fulltext_id_list_next,
            &__fulltext_id_list_compare,
            &__fulltext_id_list_move_after
    );
}

/**
 * Append new item having given <code>id</code> to given <code>append_to</code>.
 */
struct fulltext_id_list *fulltext_id_list_append(struct fulltext_id_list *append_to, unsigned long id) {

    struct fulltext_id_list *ret = malloc(sizeof(struct fulltext_id_list));
    ret->id = id;
    ret->count = 0;
    ret->score = 0;
    ret->prev = append_to;
    ret->next = NULL;

    if (append_to != NULL)
        append_to->next = ret;

    return ret;
}

/**
 * Find item having given <code>id</code>, start with <code>start_at</code>
 */
struct fulltext_id_list *fulltext_id_list_get(struct fulltext_id_list *start_at, unsigned long id) {
    struct fulltext_id_list *curr = start_at;
    while (curr != NULL) {
        if (curr->id == id)
            return curr;
        curr = curr->next;
    }
    return NULL;
}


/**
 *
 */
void fulltext_id_list_free(struct fulltext_id_list *item) {
    struct fulltext_id_list *curr = item;
    struct fulltext_id_list *next = NULL;
    while (curr != NULL) {
        next = curr->next;
        free(curr);
        curr = next;
    }
}


/**
 * Add given word to document.
 */
int fulltext_add_word(const char *doc_id, const char *word) {
    return fulltext_db_add_word(doc_id, word);
}

/**
 * Find all documents containing given words.
 */
struct fulltext_id_list *fulltext_get_documents(int word_count, const char *words[]) {
    return fulltext_db_get_documents(word_count, words);
}

#endif //CTEXTUTILS_INDEXER