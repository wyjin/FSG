#include "utils_snapshot.hpp"
#include "utils_real_time.hpp"
#include <iostream>
#include <array>

using namespace std;
string call_lsof(const string& command) {
	//redirects stderr to stdout
	string command_with_err = command + "2>&1";
	array<char, 128> buffer;
	string result;
	FILE* pipe = popen(command_with_err.c_str(), "r");
	if (!pipe) {
	    cerr << "Unknown error, exiting." << endl;
	    exit(1);
	}
	while (fgets(buffer.data(), 128, pipe) != NULL) {
	    result += buffer.data();
	}
	// TODO: check return code for error
	pclose(pipe);
	return result;
}
