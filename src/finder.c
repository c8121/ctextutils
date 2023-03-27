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
#include <sysexits.h>

#include "lib/fulltext_index.h"
#include "lib/fulltext_index_mysql.h"

#include "../dep/cutils/src/db_util.h"
#include "../dep/cutils/src/config_file.h"

#define DEFAULT_CONFIG_FILE "./config/default-config"

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
    printf("%s <word> [<word...>]\n", argv[0]);
}

/**
 * 
 */
int main(int argc, char *argv[]) {

    if (argc < 2) {
        usage_message(argc, argv);
        return EX_USAGE;
    }

    memset(&db, 0, sizeof(struct db_config));
    read_config_file_from_cli_arg("-config", argc, argv, 1, DEFAULT_CONFIG_FILE,
                                  &apply_config);

    if (!fulltext_db_connect(db.host, db.user, db.password, db.db, db.port))
        return 0;
    memset(&db, 0, sizeof(struct db_config));

    struct fulltext_id_list *result = fulltext_get_documents(argc - 1, (const char **) &argv[1]);
    struct fulltext_id_list *curr = result;
    while (curr != NULL) {
        printf("%li, %f\n", curr->id, curr->score);
        curr = curr->next;
    }

    fulltext_db_disconnect();
}
