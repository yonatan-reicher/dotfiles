/*
 * This program installs all the configuration files on your machine! If it
 * supported that, that is...
 *
 * Note that this program is written in C and not something nicer like Python or
 * Rust because I want it to be easy to run on any development machine, and it
 * is not written in something like shell or bash because I want it to be robust
 * and I don't know how to do that in such a shell language (except for maybe in
 * nu, but that is not stable or generally available on development machines).
 */

#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "jrc/include/basic.h"
#include "jrc/include/path.h"
#include "jrc/include/str.h"

/** Make a function call and fail if it returned a 0-value (NULL). */
#define NONZERO(E)                                                             \
    if ((E) == 0) {                                                            \
        PANIC("Call " #E " failed.");                                          \
    }

/** An entry in a table of files to symlink. */
typedef struct LinkEntry {
    /// The link to create.
    const char linkPath[PATH_MAX];
    /// The path the link will be targeting.
    const char targetPath[PATH_MAX];
} LinkEntry;

/** A table of files to symlink. */
const LinkEntry files[] = {
    { "~/.config/aliases", "./aliases" },
    { "~/.config/nushell", "./nushell" },
    { "~/.bashrc", "./bashrc" },
    { "~/.bash_profile", "./bash_profile" },
    { "~/.config/git/config", "./gitconfig" },
    { "~/.config/kitty", "./kitty" },
    // { "~/.config/nvim", "./nvim" },
};
#define N_FILES ARRAY_LEN(files)

int main(int argc, char* argv[]) {
    UNUSED(argc);
    UNUSED(argv);

    // Link all the files in the table.
    for (size_t i = 0; i < N_FILES; i++) {
        const LinkEntry* e = &files[i];

        // Convert to path objects.
        Path target = path_expand(path_parse(e->targetPath));
        Path link = path_expand(path_parse(e->linkPath));
        // Read the link, and check if it is already pointing to the value. Do
        // the linking!
        char* link_str = path_to_str(&link);
        char* target_str = path_to_str(&target);
        if (path_exists(&link)) {
            if (path_readable(&link)) {
                Path link_value = path_read_link(&link);
                if (path_is_empty(&link_value)) {
                    PANIC("File %s exists and is not a symlink.", link_str);
                } else if (path_eq(&link_value, &target)) {
                    WARNING(
                        "Skipping link %s ↦ %s, already exists.",
                        link_str,
                        target_str
                    );
                    free(link_str);
                    free(target_str);
                    path_free(&link_value);
                    continue;
                } else {
                    PANIC(
                        "Link %s already exists and points to %s instead of "
                        "%s.",
                        link_str,
                        path_to_str(&link_value),
                        target_str
                    );
                }
            }
        }
        printf("Link %s ↦ %s\n", link_str, target_str);
        SYS(symlink(target_str, link_str));
        free(link_str);
        free(target_str);
        path_free(&link);
        path_free(&target);
    }

    // Unlink bad file
    Path link = path_home();
    path_add(&link, str_clone(".gitconfig"));
    char* link_str = path_to_str(&link);
    if (path_exists(&link)) {
        printf(
            "This system has a '%s' file. Would you like to remove it? (y/n) ",
            link_str
        );
        fflush(stdout);
        int c = 0;
        do {
            c = getchar();
            if (c == EOF) {
                PANIC("unexpected end of input");
            }
        } while (c != 'y' && c != 'n');
        if (c == 'y') {
            SYS(unlink(link_str));
        }
    }
    free(link_str);
    path_free(&link);

    // Source this `profile` file
    Path target = path_expand(path_parse("./profile"));
    link = path_expand(path_parse("~/.profile"));
    if (path_exists(&link) &&
        (!path_writable(&link) || !path_readable(&link))) {
        link_str = path_to_str(&link);
        PANIC("Cannot read or write file '%s'.", link_str);
    }
    char* target_str = path_to_str(&target);
    link_str = path_to_str(&link);
    char* line_to_append = malloc(2 + strlen(target_str));
    line_to_append[0] = 0;
    strcat(line_to_append, ". ");
    strcat(line_to_append + 2, target_str);
    FILE* f = fopen(link_str, "a+");
    rewind(f);
    char* line = NULL;
    size_t len = 0;
    ssize_t read = 0;
    bool found = false;
    while ((read = getline(&line, &len, f)) != -1) {
        str_remove_newline(line);
        if (str_eq(line, line_to_append)) {
            found = true;
            break;
        }
    }
    free(line);
    if (!found) {
        printf("Append '%s' to '%s'.\n", line_to_append, link_str);
        fseek(f, 0, SEEK_END);
        fwrite("\n", 1, 1, f);
        fwrite(line_to_append, 1, strlen(line_to_append), f);
    } else {
        WARNING(
            "Skipping updating '%s', '%s' already found.",
            link_str,
            line_to_append
        );
    }
    fclose(f);

    return 0;
}
