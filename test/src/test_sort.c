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

#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>


#include "cutils/src/sort_util.h"

int __curr_id = 0;
int __curr_num = 0;

struct item {
    int id;
    int num;
    struct item *prev;
    struct item *next;
};

struct item *append(struct item *prev) {
    struct item *ret = malloc(sizeof(struct item));
    ret->id = ++__curr_id;
    ret->num = ++__curr_num;
    ret->next = NULL;
    ret->prev = prev;

    if (prev != NULL)
        prev->next = ret;

    return ret;
}

void print_items(struct item *start) {
    printf("[");
    struct item *curr = start;
    int num = 0;
    while (curr != NULL) {
        if (num++ > 0)
            printf(", ");
        printf("%i=%i (ID%i)", num, curr->num, curr->id);
        curr = curr->next;
    }
    printf("]\n");
}

void *next(void *curr) { return ((struct item *) curr)->next; }

int compare(void *a, void *b) { return ((struct item *) a)->num > ((struct item *) b)->num ? 1 : 0; }

int chk = 0;

void move_after(void *move, void *after) {
    struct item *m = (struct item *) move;
    struct item *a = (struct item *) after;

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
 * 
 */
int main(int argc, char *argv[]) {

    struct item *first = append(NULL);

    struct item *curr = first;
    for (int i = 0; i < 20; i++)
        curr = append(curr);

    print_items(first);

    first = linked_list_sort(first, &next, &compare, &move_after);
    print_items(first);

    first->num = 15;
    first = linked_list_sort(first, &next, &compare, &move_after);
    print_items(first);

    first->num = 1;
    first = linked_list_sort(first, &next, &compare, &move_after);
    print_items(first);

    first->num = 22;
    first = linked_list_sort(first, &next, &compare, &move_after);
    print_items(first);


    first->num = 0;
    first = linked_list_sort(first, &next, &compare, &move_after);
    print_items(first);
}
