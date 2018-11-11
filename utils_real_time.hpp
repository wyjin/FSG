#pragma once

#include <string>
#include <vector>

/*
 possible problems here:
 1. We need to access system programs like `cat' or `ls' to get process name,
 which also accesses files.
 2. Issues when getting process names: directory does not exist.
 */

<<<<<<< HEAD
std::vector<std::string> trace_pid(uint32_t pid);

std::vector<std::string> split(const std::string& str, const std::string &delim);

std::string print_info(const std::string &proc_info);

/* utils_hpp */
=======
std::vector<std::string> trace_pid(pid_t pid);

std::vector<std::string> split(
    const std::string& str, const std::string& delim);

std::string print_info(const std::string& proc_info);
>>>>>>> f8c9c61cf65f31472f1df232fefc84b29697245a
