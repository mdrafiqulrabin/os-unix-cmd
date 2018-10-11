#include <iostream>
#include <vector>
#include <unistd.h>
#include <csignal>
#include "my_utils.h"

using namespace std;
bool debug = false;

vector<string> m_command;
vector<int> m_process_id;
vector<string> m_process_name;

void execute_builtin_cd() {
    string path = "";
    string cd_arg = "";

    vector <char *> cd_vec(m_command.size() + 1); // one extra for null
    for (size_t i = 0; i != m_command.size(); ++i) {
        cd_vec[i] = &m_command[i][0];
    }

    // set ~ when only cd
    if (cd_vec.size() > 2) { // one extra for null
        cd_arg = cd_vec[1];
    } else {
        cd_arg = "~"; // default HOME
    }

    // use HOME when cd with ~
    if (cd_arg[0] == '~') {
        path = getenv("HOME");
        for (int i=1; i < cd_arg.size(); ++i) {
            path += cd_arg[i];
        }
    } else {
        path = cd_arg;
    }

    if (chdir(path.c_str()) != 0) {
        perror("Error(***) ");
    }
}

void execute_builtin_exit() {
    int status;
    for(int i = 0; i < m_process_id.size(); ++i) {
        if (waitpid(m_process_id[i], &status, WNOHANG) == 0) { // WNOHANG: non-blocking, return immediately
            if (debug) cout << "kill [" << m_process_id[i] << "] \t" << m_process_name[i] << endl;
            kill(m_process_id[i], SIGTERM); // SIGKILL: forced kill
        }
    }
    exit(0); // 0: safe exit
}

bool apply_built_in() {
    string cmd = getLowerCaseStr(m_command.front());
    if (cmd == "cd") {
        execute_builtin_cd();
        return true;
    } else if (cmd == "exit") {
        execute_builtin_exit();
        return false;
    } else {
        return false;
    }
}
