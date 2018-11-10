#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <getopt.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fanotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "argparse.h"

#define CHK(expr, errcode) if((expr)==errcode) perror(#expr), exit(EXIT_FAILURE)

using namespace std;

void pid2name(uint32_t pid, char* name_buf) {                                                                                                                                                                                             
  char comm_buf[32] = {'\0'};                                                                                                                                                                                                             
                                                                                                                                                                                                                                          
  sprintf(comm_buf, "/bin/cat /proc/%d/comm", pid);                                                                                                                                                                                            
  FILE *fp;                                                                                                                                                                                                                               
  fp = popen(comm_buf, "r");                                                                                                                                                                                                              
  if (fp == NULL) {                                                                                                                                                                                                                       
    printf("error when trying to get process name for pid %d", pid);                                                                                                                                                                      
    exit(1);                                                                                                                                                                                                                              
  }                                                                                                                                                                                                                                       
  pclose(fp);                                                                                                                                                                                                                             
}  

class Solution {
private:
	bool verbose;
	string mode;// either SNAPSHOT or REALTIME
	vector<string> files_to_track;
public:
	Solution(int argc, char** argv) : verbose(false) {
		parse_command_line(argc, argv);
	}

	void run() {
		if (mode == "SNAPSHOT") {
			get_snapshot();
		}
		else {
			get_realtime();
		}

	}
private:
	void get_snapshot() {
		string command = "lsof -- ";
		for (string & file : files_to_track) {
			command += file + " ";
		}
		int status = system(command.c_str());
		if (status != 0) {
			cerr << "Something went wrong..." << endl;
			exit(status);
		}
	}

	void get_realtime() {
		int fan;
		char buf[4096];
		string fdpath;
		char path[PATH_MAX + 1];
		ssize_t buflen, linklen;
		struct fanotify_event_metadata *metadata;
		CHK(fan = fanotify_init(FAN_CLASS_NOTIF, O_RDONLY), -1);
		while(true) {
			for (auto & file : files_to_track) {
				CHK(
					fanotify_mark(
						fan, 
						FAN_MARK_ADD | FAN_MARK_MOUNT, 
						FAN_OPEN | FAN_EVENT_ON_CHILD, 
						AT_FDCWD, 
						file.c_str()), 
				-1);
				CHK(buflen = read(fan, buf, sizeof(buf)), -1);
				metadata = (struct fanotify_event_metadata*)&buf;
				while(FAN_EVENT_OK(metadata, buflen)) {
					if (metadata->mask & FAN_Q_OVERFLOW) {
						// FIXME: Should we exit if this happens?
						cout << "Queue overflow!" << endl;
						continue;
				  	}
				  	fdpath = "/proc/self/fd/" + to_string(metadata->fd);
				  	CHK(linklen = readlink(
				  		fdpath.c_str(), path, sizeof(path) - 1), -1);
				  	path[linklen] = '\0';
				  	pid_t pid = (pid_t)(metadata->pid);
				  	cout << path 
				  		 << " opened by application " 
				  		 << application(pid);
				  	if (verbose) {
				  		cout << " (process " << pid << ")";
				  	} 
				  	cout << endl;
				  	close(metadata->fd);
				  	metadata = FAN_EVENT_NEXT(metadata, buflen);
				}
			}
		}
	}

	// returns application name from pid
	string application(pid_t pid) {
		// TODO
		return "Application of pid " + to_string(pid);
	}

	void parse_command_line(int argc, char *argv[]) {
		int verbose_arg = 0;
	    const char *mode_arg = NULL;
	    struct argparse_option options[] = {
	        OPT_HELP(),
	        OPT_GROUP("Basic options"),
	        OPT_BOOLEAN('v', "verbose", &verbose_arg, "verbose"),
	        OPT_STRING('m', "mode", &mode_arg, 
	        	"mode to run in (SNAPSHOT or REALTIME)"),
	        OPT_END(),
	    };

	    const char *const usage[] = {
		    "Tracker [options] [[--] args]",
		    "Tracker [options]",
		    NULL,
		};
		string brief_description = 
			"\nThis is a file system access tracker, "
			"which provides information on which "
			"applications are accessing files in "
			"either real time or in snapshots";
		string long_description = 
			"\nWhen run in REALTIME mode, prints the "
			"name of every application that accesses "
			"any file provided. When run in SNAPSHOT "
			"mode, prints the name of every application "
			"accessing any currently open file in the "
			"list provided.";
	    struct argparse argparse;
	    argparse_init(&argparse, options, usage, 0);
	    argparse_describe(&argparse, brief_description, long_description);
	    argc = argparse_parse(&argparse, argc, argv);
	    if (verbose_arg != 0) {
	        verbose = true;
	    	cout << "Verbose flag set" << endl;
	    }
	    if (mode_arg != NULL) {
	    	mode = string(mode_arg);
	    	cout << "Mode: " << mode << endl;
	    }
	    if (argc != 0) {
	    	cout << "I don't know what this shit is..." << endl;
	        for (int i = 0; i < argc; i++) {
	            printf("argv[%d]: %s\n", i, *(argv + i));
	        }
	    }
	}
}; // Solution

int main(int argc, char** argv) {
	Solution s(argc, argv);
	s.run();
	return 0;
}
