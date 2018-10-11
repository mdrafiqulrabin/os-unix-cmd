#include <iostream>
#include <vector>
#include <unistd.h>

using namespace std;

vector<string> m_directory;

string getLowerCaseStr(string str) {
    if (str.size() > 0) {
        transform(str.begin(), str.end(), str.begin(), ::tolower);
    }
    return str;
}

void find_environment_paths() {
    string buffer = "";
    string env_path = getenv("PATH");

    for (int i = 0; i < env_path.size(); ++i) {
        if (env_path[i] == ':') {
            m_directory.push_back(buffer);
            buffer = "";
        } else {
            buffer += env_path[i];
        }
    }

    if (buffer.size() > 0) {
        m_directory.push_back(buffer);
        buffer = "";
    }
}

string find_executable_path(string cmd) {
    // when cmd is full path (i.e. /bin/ls)
    if (access(cmd.c_str(), X_OK) == 0) { // accessibility check with X_OK mode
        return cmd;
    }

    // when cmd is not full path (i.e. ls)
    for (vector<string>::iterator it = m_directory.begin(); it != m_directory.end(); it++) {
        string tempPath = (*it) + "/" + cmd;
        if (access(tempPath.c_str(), X_OK) == 0) { // accessibility check with X_OK mode
            return tempPath;
        }
    }

    return "";
}

void display(vector<int> vec, string msg) {
    cout << endl << "Display: " << msg << endl;
    for (int i = 0; i < vec.size(); ++i) {
        cout << vec[i] << " ";
    }
    cout << endl;
}

void display(vector<string> vec, string msg) {
    cout << endl << "Display: " << msg << endl;
    for (vector<string>::iterator it = vec.begin(); it != vec.end(); it++) {
        cout << *it << endl;
    }
    cout << endl;
}

void display(vector<char *> vec, string msg) {
    cout << endl << "Display: " << msg << endl;
    int vec_size = vec.size();
    cout << endl << vec_size << endl;
    vector<char *>::iterator it = vec.begin();
    for (int i = 0; i < vec_size - 2; ++i) { // as one extra for null
        cout << *it << endl;
        it++;
    }
    cout << endl;
}
