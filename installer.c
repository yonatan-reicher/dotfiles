#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cwalk.h"


#define UNUSED(X) (void)(X)


typedef int Errno;


Errno my_link(const char* source, const char* new_link)
{
    int ret = symlink(source, new_link);
    return ret == 0 ? 0 : errno;
}


int main(int argc, char* argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    char cwd[PATH_MAX];
    char source[PATH_MAX];
    char target[PATH_MAX];

    getcwd(cwd, PATH_MAX);
    cwk_path_join(cwd, "./nushell", source, PATH_MAX);
    cwk_path_join(getenv("HOME"), ".config/nushell", target, PATH_MAX);
    Errno e = 0;
    printf("%s ↦ %s\n", target, source);
    e = my_link(source, target);
    return e;
}
