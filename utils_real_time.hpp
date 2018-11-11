#pragma once

#include <string>
#include <vector>

/*
 possible problems here:
 1. We need to access system programs like `cat' or `ls' to get process name,
 which also accesses files.
 2. Issues when getting process names: directory does not exist.
 */

std::vector<std::string> trace_pid(pid_t pid);

std::vector<std::string> split(
    const std::string& str, const std::string& delim);

std::string format_info(const std::string& proc_info);
