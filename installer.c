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
#include <unistd.h>

/** A library for easier working with path */
#include "cwalk.h"


/** Mark a variable as unused */
#define UNUSED(X) (void)(X)


/** Hard failure with an error message */
#define PANIC(...) \
    do { \
        fprintf(stderr, "[Error] " __FILE__ " %s %d: ", __func__, __LINE__); \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n"); \
        exit(1); \
    } while (0)


/** Make a syscall and fail if it returned -1. */
#define SYS(E) \
    if ((E) == -1) { \
        PANIC("Call " #E " failed with error: %s (code %d).", strerror(errno), errno); \
    }

/** Make a function call and fail if it returned a 0-value (NULL). */
#define NONZERO(E) \
    if ((E) == 0) { \
        PANIC("Call " #E " failed."); \
    }


#define ARRAY_LENGTH(A) (sizeof(A) / sizeof((A)[0]))


bool str_starts_with(const char* str, const char* prefix) {
    while (prefix[0] != 0) {
        if (str[0] != prefix[0]) return false;
        str++;
        prefix++;
    }
    return true;
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
};
#define N_FILES ARRAY_LENGTH(files)


int main(int argc, char* argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    char cwd[PATH_MAX];
    char linkPath[PATH_MAX];
    char linkValue[PATH_MAX];
    char targetPath[PATH_MAX];

    // Initialize cwd.
    NONZERO(getcwd(cwd, PATH_MAX));

    // Link all the files in the table.
    for (size_t i = 0; i < N_FILES; i++) {
        const LinkEntry* e = &files[i];

        // Initialize the buffers above.
        linkPath[0] = targetPath[0] = 0;
        cwk_path_join(cwd, e->targetPath, targetPath, PATH_MAX);
        if (str_starts_with(e->linkPath, "~/")) {
            cwk_path_join(getenv("HOME"), e->linkPath + 2, linkPath, PATH_MAX);
        } else {
            cwk_path_join(cwd, e->linkPath, linkPath, PATH_MAX);
        }

        // TODO: Read the link, and check if it is already pointing to the
        // value.
        // Do the linking!
        printf("Link %s ↦ %s\n", linkPath, targetPath);
        SYS(symlink(targetPath, linkPath));
    }

    return 0;
}
