#include "utils_real_time.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>

void pid2name(uint32_t pid, char* name_buf) {
    char comm_buf[32] = {0};
    sprintf(comm_buf, "/bin/cat /proc/%d/comm", pid);
    FILE *fp;
    fp = popen(comm_buf, "r");
    if (fp == NULL) {
        printf("error when trying to get process name for pid %d", pid);
        exit(1);
    }
    if(fgets(name_buf, sizeof(name_buf) - 1, fp) == NULL) {
        printf("error copying name for pid %d", pid);
        exit(1);
    }
    pclose(fp);
}

std::string trace_pid(uint32_t pid) {
    char buf[32] = {0};
    sprintf(buf, "pstree -sup %d", pid);
    FILE *fp = popen(buf, "r");
    if (fp == NULL) {
        printf("error when trying to find process with pid %d", pid);
        exit(1);
    }
    std::string result;
    while (fgets(buf, sizeof(buf)-1, fp)) {
        result += buf;
    }
    pclose(fp);
    std::vector<std::string> proc_name = split(result, "---");
    for (auto&& i: proc_name) {
        std::cout << i << std::endl;
    }
    return proc_name;
}

std::vector<std::string> split(const std::string& str, const std::string & delim) {
    std::size_t current, previous = 0;
    current = str.find(delim);
    cont = std::vector<std::string>;
    while (current != std::string::npos) {
        cont.push_back(str.substr(previous, current - previous));
        previous = current + 1;
        current = str.find(delim, previous);
    }
    cont.push_back(str.substr(previous, current - previous));
    return cont;
}
