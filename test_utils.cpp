#include<iostream>
#include<string>
#include<vector>
#include "utils_real_time.hpp"

using namespace std;

int main() {
    string info = 
    	"systemd(1)---kdeinit5(736,wnyjin)---"
    	"ksmserver(799)---plasmashell(868)---"
    	"dolphin(1240)---konsole(1252)---"
    	"zsh(1307)---vim(4201)";
    vector<string> procs = split(info, "---");
    for (const string& proc : procs) {
        cout << proc << endl;
        cout << format_info(proc) << endl;
    }
    return 0;
}
