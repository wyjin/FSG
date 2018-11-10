#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fanotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#define CHK(expr, errcode) if((expr)==errcode) perror(#expr), exit(EXIT_FAILURE)

using namespace std;

void pid2name(uint32_t pid, char* name_buf) {                                                                                                                                                                                             
  char comm_buf[32] = {'\0'};                                                                                                                                                                                                             
                                                                                                                                                                                                                                          
  sprintf(comm_buf, "/bin/ls /proc/%d/", pid);                                                                                                                                                                                            
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
		string command; // TODO: make lsof command from given files
		system(command);
	}

	void get_realtime() {
		int fan;
		char buf[4096];
		char fdpath[32];
		char path[PATH_MAX + 1];
		ssize_t buflen, linklen;
		struct fanotify_event_metadata *metadata;
		CHK(fan = fanotify_init(FAN_CLASS_NOTIF, O_RDONLY), -1);
		while(true) {
			for (auto & file : files_to_track) {
				CHK(fanotify_mark(fan, FAN_MARK_ADD | FAN_MARK_MOUNT, FAN_OPEN | FAN_EVENT_ON_CHILD, AT_FDCWD, file.c_str()), -1);
				CHK(buflen = read(fan, buf, sizeof(buf)), -1);
				metadata = (struct fanotify_event_metadata*)&buf;
				while(FAN_EVENT_OK(metadata, buflen)) {
					if (metadata->mask & FAN_Q_OVERFLOW) {
						// FIXME: Should we just exit if this happens???
						cout << "Queue overflow!" << endl;
						continue;
				  	}
				  	fdpath = "/proc/self/fd/" + string(metadata->fd);
				  	CHK(linklen = readlink(fdpath, path, sizeof(path) - 1), -1);
				  	path[linklen] = '\0';
				  	// TODO: convert pid to application name
				  	pid_t pid = (pid_t)(metadata->pid);
				  	cout << path << " opened by application " << application(pid);
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
		return "EECS_388_ROX_MY_SOX";
	}

	void parse_command_line(int argc, char *argv[]) {
		static struct option longopts[] = {
			{ "mode", 			required_argument, 	nullptr, 	'm'}
			{ "verbose",		no_argument,		nullptr,	'v' },
			{ "files",			required_argument,	nullptr,	'f' },
			{ nullptr,			0,					nullptr,	'\0' }
		};

		int idx = 0;
		char c;
		while ((c = char((getopt_long(argc, argv, "mvf:", longopts, &idx)))) != -1) {
			switch (c) {
			case 'm': 
				mode = optarg;
				if (mode != "SNAPSHOT" && mode != "REALTIME") {
					cerr << "Mode must be one of SNAPSHOT or REALTIME" << endl;
					exit(1);
				}
			case 'v':
				verbose = true;
				break;
			case 'f':
				// TODO: make this work
				files_to_track.push_back(optarg);
				break;
			}
		}
	}
};

int main(int argc, char** argv) {
	Solution s(argc, argv);
	s.run();
	return 0;
}
