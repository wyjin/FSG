#include "utils_snapshot.hpp"
#include <iostream>
#include <array>

using namespace std;
void call_lsof(const string& command) {
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
	//auto returnCode = 
	pclose(pipe);
	if (result.empty()) {
		cout << "None of the files specified are currently being accessed." << endl;
	}
	else {
		cout << result << endl;
	}
//	if (returnCode != 0) {
//		cerr << "Exiting with status " << returnCode << endl;
//		cerr << "Result from lsof: " << result << endl;
//		exit(returnCode);
//	}
}
