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


#define WARNING(...) \
    do { \
        fprintf(stderr, "[Warning] " __FILE__ " %s %d: ", __func__, __LINE__); \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n"); \
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


bool file_exists(const char* path) {
    return access(path, F_OK) == 0;
}


bool file_readable(const char* path) {
    return access(path, R_OK) == 0;
}


bool file_writeable(const char* path) {
    return access(path, W_OK) == 0;
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
        if (file_exists(linkPath)) {
            if (file_readable(linkPath)) {
                read_link(linkPath, linkValue);
                if (strcmp(linkValue, "") == 0) {
                    PANIC("File %s exists and is not a symlink.", linkPath);
                } else if (strcmp(linkValue, targetPath) == 0) {
                    WARNING(
                        "Skipping link %s ↦ %s, already exists.",
                        linkPath, targetPath
                    );
                    continue;
                } else {
                    PANIC(
                        "Link %s already exists and points to %s instead of %s.",
                        linkPath, linkValue, targetPath
                    );
                }
            }
        }
        printf("Link %s ↦ %s\n", linkPath, targetPath);
        SYS(symlink(targetPath, linkPath));
    }

    return 0;
}
