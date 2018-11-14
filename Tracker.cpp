#include <iostream>
#include <fstream>
#include <vector>
#include <getopt.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <sys/fanotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "utils_snapshot.hpp"
#include "utils_real_time.hpp"

#define CHK(expr, errcode) if((expr)==errcode) perror(#expr), exit(EXIT_FAILURE)

using namespace std;

class Solution {
private:
    bool verbose;
    string mode;
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
    bool path_exists(const string& path) {
        ifstream f(path.c_str());
        return f.good();
    }

    void get_snapshot() {
        string command = "lsof -F cnf -- ";
        for (string & file : files_to_track) {
            if (!path_exists(file)) {
                cerr << file << " is not a valid file." << endl;
                exit(1);
            }
            command += file + " ";
        }
        call_lsof(command);
    }

    void get_realtime() {
        char buf[4096];
        string fdpath;
        char path[PATH_MAX + 1];
        struct fanotify_event_metadata* metadata;

        int fan = fanotify_init(FAN_CLASS_NOTIF, O_RDONLY);
        CHK(fan, -1);
        // makes it track everything
        if (files_to_track.empty()) {
            files_to_track.push_back("/");
        }
        while(true) {
            for (const string& file : files_to_track) {
                int mark = fanotify_mark(
                    fan,
                    FAN_MARK_ADD,
                    FAN_OPEN | FAN_EVENT_ON_CHILD,
                    AT_FDCWD,
                    file.c_str());
                CHK(mark, -1);
                ssize_t buflen = read(fan, buf, sizeof(buf));
                CHK(buflen, -1);
                metadata = (struct fanotify_event_metadata*)&buf;
                while(FAN_EVENT_OK(metadata, buflen)) {
                    if (metadata->mask & FAN_Q_OVERFLOW) {
                        // FIXME: Should we exit if this happens?
                        cerr << "Queue overflow!" << endl;
                        continue;
                    }
                    fdpath = "/proc/self/fd/" + to_string(metadata->fd);
                    ssize_t linklen = 
                        readlink(fdpath.c_str(), path, sizeof(path) - 1);
                    CHK(linklen, -1);
                    path[linklen] = '\0';
                    print_access(string(path), pid_t(metadata->pid));
                    close(metadata->fd);
                    metadata = FAN_EVENT_NEXT(metadata, buflen);
                }
            }
        }
    }

    void print_access(const string& path, pid_t pid) {
        cout << path 
             << " opened by application " 
             << application(pid);
        if (verbose) {
            cout << " (process " << pid << ")";
        }
        cout << endl;
    }

    // returns application name from pid
    string application(pid_t pid) {
        return format_info(trace_pid(pid).back());
    }

    void parse_command_line(int argc, char **argv) {
        string usage = "Tracker [options] [args]";
        string description =
            "This is a file system access tracker, "
            "which provides information on which "
            "applications are accessing files in "
            "either real time or in snapshots";
        if (argc == 1) {
            cerr << usage << endl;
            exit(1);
        }
        static struct option longopts[] = {
            { "verbose",    optional_argument,  nullptr,    'v' },
            { "mode",       required_argument,  nullptr,    'm' },
            { "help",       no_argument,        nullptr,    'h' },
            { nullptr,      0,                  nullptr,    '\0' }
        };
        int idx = 0;
        char c;
        while ((c = char((getopt_long(argc, argv, "vhm:", longopts, &idx)))) != -1) {
            switch (c) {
            case 'h':
                cout << description << endl;
                exit(1);
                break;
            case 'v':
                verbose = true;
                break;
            case 'm':
                string temp = string(optarg);
                if (temp != "REALTIME" && temp != "SNAPSHOT") {
                    cerr << "Mode must be one of REALTIME or SNAPSHOT" << endl;
                    exit(1);
                }
                mode = temp;
                break;
            }
        }
        while (optind < argc) {
            files_to_track.push_back(argv[optind]);
            optind++;
        }
    }
}; // Solution

int main(int argc, char** argv) {
    Solution s(argc, argv);
    s.run();
    return 0;
}
