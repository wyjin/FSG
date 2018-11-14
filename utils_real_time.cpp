#include "utils_real_time.hpp"
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fanotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>

using namespace std;

vector<string> trace_pid(pid_t pid) {
    char buf[32] = {0};
    sprintf(buf, "pstree -sup %d", pid);
    FILE* fp = popen(buf, "r");
    if (fp == NULL) {
        printf("error when trying to find process with pid %d", pid);
        exit(1);
    }
    string result;
    while (fgets(buf, int(sizeof(buf)) - 1, fp)) {
        result += buf;
    }
    pclose(fp);
    vector<string> proc_names = split(result, "---");
    return proc_names;
}

vector<string> split(const string& str, const string& delim) {
    size_t current, previous = 0;
    size_t pattern_len = delim.length();
    current = str.find(delim);
    vector<string> cont;
    while (current != string::npos) {
        cont.push_back(str.substr(previous, current - previous));
        previous = current + pattern_len;
        current = str.find(delim, previous);
    }
    cont.push_back(str.substr(previous, current - previous));
    return cont;
}

string format_info(const string& proc_info) {
    // the info string should look like proc_name(pid, user)
    size_t start_paren = proc_info.find('(');
    //size_t end_paren = proc_info.find(')');
    string proc_name = proc_info.substr(0, start_paren);
    string info_str = proc_name; 
    //string info_str = "process \"" + proc_name + "\"";
//    string proc_data = 
//        proc_info.substr(start_paren + 1, end_paren - start_paren - 1);
//    size_t pos = proc_data.find(',');
//    if (pos != string::npos) {
//        string pid = proc_data.substr(0, pos);
//        string user = proc_data.substr(pos + 1, proc_data.length() );
//        info_str += 
//            " (pid: " + string(pid) + ")" + " started by user: " + user;
//    } 
//    else {
//        info_str += " (pid: " + string(proc_data) + ")";
//    }
    return info_str;
}
