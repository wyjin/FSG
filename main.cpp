#include <iostream>
#include <iomanip>
#include <unordered_map>

using namespace std;

class Solution {
public:
	// does everything
	void run() {
		build_application_mapping();
		while(true) {
			// listens for file system read/writes
			pair<pid_t, string> access = get_access();
			string app = applications[access.first];
			string file = access.second;
			accesses[app].add(file);
			cout << app << " accessed file " << file << endl;
		}
	}
private:
	void build_application_mapping() {
		// get a list of pid's
	}

	pair<pid_t, string> get_access();

	// maps pid to application name
	unordered_map<pid_t, string> applications;

	// maps applications to which files they accessed
	unordered_map<string, unordered_set<string>> accesses;

	// maps pid to parent pids (if parent is 0, it's a top level process)
	unordered_map<pid_t, pid_t> parents;

	unordered_set<string> files_to_track;
}

void parse_command_line(int argc, char *argv[],vector<string> & files_to_track) {
	static struct option longopts[] = {
		{ "verbose",		no_argument,		nullptr,	'v' },
		{ "files",			required_argument,	nullptr,	'f' },
		{ nullptr,			0,					nullptr,	'\0' }
	};

	int idx = 0;
	char c;
	while ((c = char((getopt_long(argc, argv, "vf:", longopts, &idx)))) != -1) {
		switch (c) {
		case 'v':
			VERBOSE = true;
			break;
		case 'f':
      // TODO: make this work
      files_to_track.push_back(optarg);
			break;
		}
	}
}

int main(int argc, char** argv) {
  vector<string> files_to_track;
  parse_command_line(argc, argv, files_to_track);
  Solution s;//TODO: pass this what it needs from command line
  s.run();
  return 0;
}
