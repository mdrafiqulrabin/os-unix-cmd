#include <iostream>
#include <sys/wait.h>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include "cmd_utils.h"

using namespace std;

vector<string> io_input;
vector<string> io_output;

bool non_interactive_bg = false;

void initialize_parameters() {
    m_command.clear();
    io_input.clear();
    io_output.clear();
    cleanupBackgroundProcesses();
    non_interactive_bg = false;
}

void checkIOBuffer(string buffer, int check_io) {
    if (buffer.size() > 0) {
        if (check_io == 0) {
            m_command.push_back(buffer);
        } else {
            if (check_io == 1) {
                io_input.push_back(buffer);
            } else if (check_io == 2) {
                io_output.push_back(buffer);
            }
        }
    }
}

void parse_input_line(string cmd_user_input) {
    int check_io = 0;
    string buffer = "";

    for (int i = 0; i < cmd_user_input.size(); ++i) {
        switch (cmd_user_input[i]) {
            case ' ':
            case '\t': {
                if (buffer.size() == 0) {
                    //do nothing
                } else if (buffer == "background") {
                    //ignore background
                    non_interactive_bg = true;
                    buffer = "";
                } else {
                    checkIOBuffer(buffer, check_io);
                    check_io = 0;
                }
                buffer = "";
            }
                break;
            case '>':
            case '<': {
                checkIOBuffer(buffer, check_io);
                //save io redirection
                string io_op(1, cmd_user_input[i]);
                if (cmd_user_input[i] == '<') {
                    check_io = 1;
                    io_input.push_back(io_op);
                } else if (cmd_user_input[i] == '>') {
                    check_io = 2;
                    io_output.push_back(io_op);
                }
                buffer = "";
            }
                break;
            default: {
                //replace ~ by HOME
                if (!buffer.compare("~/")) {
                    buffer = string(getenv("HOME")) + "/";
                }
                buffer += cmd_user_input[i];
            }
        }
    }

    checkIOBuffer(buffer, check_io);
    check_io = 0;
}

string getProcessNameWithoutBgSpace() {
    string remove_bg_space = "";
    for (size_t i = 0; i != m_command.size(); ++i) {
        if (i > 0) {
            remove_bg_space += " ";
        }
        remove_bg_space += string(&m_command[i][0]);
    }
    return remove_bg_space;
}

void execute(string cmd_full_path) {
    pid_t kidpid;

    vector<char *> cmd_args_vec(m_command.size() + 1); // one extra for null
    for (size_t i = 0; i != m_command.size(); ++i) {
        cmd_args_vec[i] = &m_command[i][0];
    }

    if ((kidpid = fork()) < 0) { /*fork a process*/
        perror("Error: fork() failed...\n");
        exit(1); //EXIT_FAILURE
    } else if (kidpid == 0) { /*child process*/
        if (io_input.empty() != true || io_output.empty() != true) {
            if (io_output.empty() != true) {
                int fd = open(&io_output[1][0], O_WRONLY | O_CREAT | O_TRUNC, 0644); // open file
                close(1); // close stdout
                dup(fd); // dup fd into stdout
                close(fd); // cleanup
            }
            if (io_input.empty() != true) {
                int fd = open(&io_input[1][0], O_RDONLY); // open file
                close(0); // close stdin
                dup(fd); // dup fd into stdin
                close(fd); // cleanup
            }
        }
        //exec() system call: execv(full_pathname, argv_vector)
        if (execv(cmd_full_path.c_str(), cmd_args_vec.data()) < 0) {
            perror("Error: execv() failed...\n");
            _exit(1); //EXIT_FAILURE
        } else {
            _exit(0); //EXIT_SUCCESS
        }
    } else { /*parent process*/
        if (non_interactive_bg) {
            // no waits for background process
            string pname = getProcessNameWithoutBgSpace();
            m_process_id.push_back(kidpid);
            m_process_name.push_back(pname);
            non_interactive_bg = false;
        } else {
            // parent waits for child
            while (wait(0) != kidpid);
        }
    }
}

int main() {

    find_environment_paths();
    string cmd_user_input = "";

    while (true) {

        initialize_parameters();

        // take user input
        cout << "cmd: ";
        getline(cin, cmd_user_input);

        parse_input_line(cmd_user_input);

        // continue for empty input
        if (cmd_user_input.size() == 0 || m_command.size() == 0) {
            continue;
        }

        // check built-in (i.e. cd/exit/processes) otherwise call execv()
        if (apply_built_in()) {
            continue;
        } else {
            string cmd_full_path = find_executable_path(m_command.front());
            execute(cmd_full_path);
        }

    }

    return 0;
}
