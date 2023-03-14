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

    if (!fulltext_db_connect(
            "localhost",
            "Test",
            "Test",
            "Test",
            3306
    )) {
        return 0;
    }

    fulltext_get_documents(100, argc - 1, (const char **) &argv[1]);

    fulltext_db_disconnect();
}
