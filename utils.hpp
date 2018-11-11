#ifndef utils_hpp
#define utils_hpp

#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fanotify.h>
#include <sys/stat.h>
#include <sys/types.h>

/*
 possible problems here:
 1. We need to access system programs like `cat' or `ls' to get process name,
 which also accesses files.
 2. Issues when getting process names: directory does not exist.
 */

void pid2name(uint32_t pid, char* name_buf);

void trace_pid(uint32_t pid);

#endif /* utils_hpp */
