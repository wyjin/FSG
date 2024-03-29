#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>
#include <mutex>
#include <thread>

#include <getopt.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <sys/fanotify.h>

#include "utils_snapshot.hpp"
#include "utils_real_time.hpp"

#define CHK(expr, errcode) if((expr)==errcode) perror(#expr), exit(EXIT_FAILURE)

using namespace std;

mutex files_to_track_lock;
static vector<string> files_to_track;

char buf[4096];
string fdpath;
        char path[PATH_MAX + 1];
        struct fanotify_event_metadata* metadata;

//class Solution {
//private:
    bool verbose;
    string logfile;
    string mode;
//public:
    // Solution(int argc, char** argv) : verbose(false) {
    //     parse_command_line(argc, argv);
    //     set_logfile_name();
    // }

    void get_user_input();
    void get_realtime();
    void get_snapshot();
    void set_logfile_name();
    void repeatedly_scan_files();
    string get_current_time();
    void print_formatted_snapshot_result(const string& result);
    string format_output(const string& path, const string& app, pid_t pid);
    string application(pid_t pid);
    bool path_exists(const string& path);


    void run() {
        if(!freopen(logfile.c_str(), "w", stdout)) {
            cerr << "Failed to open " << logfile << endl;
            exit(1);
        }
        if (mode == "SNAPSHOT") {
            get_snapshot();
        }
        else {
            get_realtime();
        }

    }
//private:
    // if user failed to provide a file, this will set it
    // to LOGFILE_date/time
    void set_logfile_name() {
        if (logfile.empty()) {
            logfile = "LOGFILE";
        }
        logfile = "logs/" + logfile + "_" + get_current_time();
    }

    string get_current_time() {
        auto t = time(nullptr);
        auto tm = localtime(&t);
        char buf[50];
        // YYYY-MM-DD-HH-MM-SS
        sprintf(buf, 
                "%.4d-%.2d-%.2d-%.2d-%.2d-%.2d", 
                1901 + tm->tm_year, 
                tm->tm_mon, 
                tm->tm_mday, 
                tm->tm_hour, 
                tm->tm_min, 
                tm->tm_sec);
        return string(buf);
    }

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
        string result = call_lsof(command);
        if (result.empty()) {
            cout << "None of the files are being accessed." << endl;
        }
        else {
            print_formatted_snapshot_result(result);
        }
    }

    void print_formatted_snapshot_result(const string& result) {
        vector<string> lines = split(result, "\n");
        lines.pop_back(); // get rid of trailing newline
        for (uint i = 0; i < lines.size();) {
            if (lines[i][0] == 'p') {
                pid_t pid = pid_t(stoi(lines[i].substr(1)));
                string app = lines[i + 1].substr(1);
                uint k = i + 2;
                vector<string> paths_for_this_process;
                while(k < lines.size() && lines[k][0] != 'p') {
                    if (lines[k][0] == 'n') {
                        paths_for_this_process.push_back(
                            lines[k].substr(1));
                    }
                    ++k;
                }
                i = k;
                for (uint r = 0; r < paths_for_this_process.size(); ++r) {
                    cout << format_output(
                        paths_for_this_process[r], app, pid) << endl;
                }
            }
        }
    }

    string format_output(const string& path, const string& app, pid_t pid) {
        string result = path + " opened by application " + app;
        if (verbose) {
            result += " (process " + to_string(pid) + ")";
        }
        return result;
    }

    void scan_files(int fan) {
        CHK(fan, -1);
        cerr << "size in scan_files: " << files_to_track.size() << endl;
        for (const string& file : files_to_track) {
            cerr << "the files name is: " << file << endl;
            int mark = fanotify_mark(
                fan,
                FAN_MARK_ADD,
                FAN_OPEN | FAN_EVENT_ON_CHILD,
                AT_FDCWD,
                file.c_str());
            cerr << "mark is " << mark << endl;
            CHK(mark, -1);
            cerr << "make it past the check" << endl;
            ssize_t buflen = read(fan, buf, sizeof(buf));
            cerr << "buflen: " << buflen << endl;
            CHK(buflen, -1);
            metadata = (struct fanotify_event_metadata*)&buf;
            if (!FAN_EVENT_OK(metadata, buflen)) {
                cerr << "FAN event is a baaaaaad boi" << metadata << endl;
            }
            int counter = 0;
            while(counter++ < 3 && FAN_EVENT_OK(metadata, buflen)) {
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
                pid_t pid = pid_t(metadata->pid);
                cerr << format_output(
                    string(path), application(pid), pid) << endl;
                close(metadata->fd);
                metadata = FAN_EVENT_NEXT(metadata, buflen);
            }
        }
    }

    void get_realtime() {
        // makes it track everything if user specified no files
        if (files_to_track.empty()) {
            files_to_track.push_back("/");
            cerr << "size: " << files_to_track.size() << endl;
            repeatedly_scan_files();
            //thread t(repeatedly_scan_files);
            //thread t(&Solution::repeatedly_scan_files, this);
            //t.join();
        }
        else {
            thread t(repeatedly_scan_files);
            thread t2(get_user_input);
            //thread t(&Solution::repeatedly_scan_files, this);
            //thread t2(&Solution::get_user_input, this);
            t.join();
            t2.join();
        }
    }

    void repeatedly_scan_files() {
        int fan = fanotify_init(FAN_CLASS_NOTIF, O_RDONLY);
        while(true) {
            //lock_guard<mutex> lock(files_to_track_lock);
            scan_files(fan);
        }
    }

    void get_user_input() {
        string file;
        while (true) {
            cin >> file;
            if (path_exists(file)) {
                lock_guard<mutex> lock(files_to_track_lock);
                files_to_track.push_back(file);
            }
        }
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
            { "verbose",    no_argument,        nullptr,    'v'     },
            { "log",        required_argument,  nullptr,    'l'     },
            { "mode",       required_argument,  nullptr,    'm'     },
            { "help",       no_argument,        nullptr,    'h'     },
            { nullptr,      0,                  nullptr,    '\0'    }
        };
        int idx = 0;
        char c;
        string temp;
        while ((c = char((getopt_long(argc, argv, "vl:hm:", longopts, &idx)))) != -1) {
            switch (c) {
            case 'v':
                verbose = true;
                break;
            case 'l':
                logfile = string(optarg);
                break;
            case 'h':
                cout << description << endl;
                exit(1);
                break;
            case 'm':
                temp = string(optarg);
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
//}; // Solution

int main(int argc, char** argv) {
    parse_command_line(argc, argv);
    set_logfile_name();
    run();
    //Solution s(argc, argv);
    //s.run();
    return 0;
}
