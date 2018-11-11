//
//  utils.cpp
//  FSG
//
//  Created by W.Jin on 11/10/18.
//  Copyright © 2018 W.Jin. All rights reserved.
//

#include "utils.hpp"

void pid2name(uint32_t pid, char* name_buf) {
    char comm_buf[32] = {'\0'};
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

void trace_pid(uint32_t pid, char* callstack_buf) {
    char comm_buf[32] = {'\0'};
    
    sprintf(comm_buf, "pstree -sup %d", pid);
    FILE *fp;
    fp = popen(comm_buf, "r");
    if (fp == NULL) {
        printf("error when trying to get process name for pid %d", pid);
        exit(1);
    }
    if(fgets(callstack_buf, sizeof(callstack_buf) - 1, fp) == NULL) {
        printf("error copying name for the call stack for pid %d", pid);
        exit(1);
    }
    pclose(fp);
}
