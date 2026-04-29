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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "jrc/src/basic.h"
#include "jrc/src/path.h"
#include "jrc/src/str.h"

/** Make a function call and fail if it returned a 0-value (NULL). */
#define NONZERO(E) \
    if ((E) == 0) { \
        PANIC("Call " #E " failed."); \
    }


bool file_exists(const Path *path) {
    char *str = path_to_str(path);
    bool ret = access(str, F_OK) == 0;
    free(str);
    return ret;
}


bool file_readable(const Path *path) {
    char *str = path_to_str(path);
    bool ret = access(str, R_OK) == 0;
    free(str);
    return ret;
}


bool file_writeable(const Path *path) {
    char *str = path_to_str(path);
    bool ret = access(str, W_OK) == 0;
    free(str);
    return ret;
}


void read_link(const char* path, char buffer[PATH_MAX]) {
    struct stat st;
    SYS(lstat(path, &st));
    if (!S_ISLNK(st.st_mode)) {
        buffer[0] = 0;
        return;
    }
    ssize_t n = readlink(path, buffer, PATH_MAX - 1);
    if (n == -1) {
        PANIC("Call readlink(%s) failed with error: %s (code %d).", path, strerror(errno), errno);
    }
    buffer[n] = 0;
}


/** An entry in a table of files to symlink. */
typedef struct LinkEntry {
    const char linkPath[PATH_MAX];
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
    { "~/.config/nvim", "./nvim" },
};
#define N_FILES ARRAY_LEN(files)


int main(int argc, char* argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    Path cwd = path_cwd();
    Path home = path_parse(getenv("HOME"));
    Path link = path_empty(), target = path_empty();
    char linkValue[PATH_MAX];

    // Link all the files in the table.
    for (size_t i = 0; i < N_FILES; i++) {
        const LinkEntry* e = &files[i];

        // Initialize the buffers above.
        path_free(&link); path_free(&target);
        target = path_concat(path_clone(&cwd), path_parse(e->targetPath));
        if (str_starts_with(e->linkPath, "~/")) {
            link = path_concat(path_clone(&home), path_parse(e->linkPath + 2));
        } else {
            link = path_concat(path_clone(&cwd), path_parse(e->linkPath));
        }

        // TODO: Read the link, and check if it is already pointing to the
        // value.
        // Do the linking!
        char *link_str = path_to_str(&link);
        char *target_str = path_to_str(&target);
        if (file_exists(&link)) {
            if (file_readable(&link)) {
                read_link(link_str, linkValue);
                if (strcmp(linkValue, "") == 0) {
                    PANIC("File %s exists and is not a symlink.", link_str);
                } else if (strcmp(linkValue, target_str) == 0) {
                    WARNING(
                        "Skipping link %s ↦ %s, already exists.",
                        link_str, target_str
                    );
                    free(link_str);
                    free(target_str);
                    continue;
                } else {
                    PANIC(
                        "Link %s already exists and points to %s instead of %s.",
                        link_str, linkValue, target_str
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
    path_append(&link, path_clone(&home));
    path_add(&link, str_clone(".gitconfig"));
    char* link_str = path_to_str(&link);
    if (file_exists(&link)) {
        printf("This system has a '%s' file. Would you like to remove it? (y/n) ", link_str);
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
    path_append(&target, path_clone(&cwd));
    path_add(&target, str_clone("profile"));
    path_append(&link, path_clone(&home));
    path_add(&link, str_clone(".profile"));
    if (file_exists(&link) && (!file_writeable(&link) || !file_readable(&link))) {
        link_str = path_to_str(&link);
        PANIC("Cannot read or write file '%s'.", link_str);
    }
    char* target_str = path_to_str(&target);
    link_str = path_to_str(&link);
    char* line_to_append = malloc(2 + strlen(target_str));
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
        WARNING("Skipping updating '%s', '%s' already found.", link_str, line_to_append);
    }
    fclose(f);

    return 0;
}
