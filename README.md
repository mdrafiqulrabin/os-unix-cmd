# OS-UNIX-CMD
Programming Assignment: 2018FA-COSC6360-Operating Systems \
Task: Implement the basic functionality of unix commands in C++.

- - -

# Guidelines:

- Open prompt for user input from standard input (i.e., "cmd: " or "cmd$ ").
- Parse command, check accessibility, fork a processe, and execute system call.
  - Basic Commands: pwd, ls, cd, cat, touch, rm, mkdir, echo and so on.
  - Input/output Redirection: ">" indicates that the standard output should be redirected to a file, and "<" indicates vice versa.
  - Background Processes: Command starts with the keyword "background" should be executed in the background, and "processes" command should display all background processes.
- Hints:
  - Study access(), chdir(), fork(), wait(), execv() vs execvp(), dup(), close(), exit() vs \_exit(), kill(), etc.
  - Find executable path by searching though the list of paths in the environment variable PATH.
  - Execute built-in commands for cd and exit, but do not use system() directly.

- - -

# Sample Input/Output:

> run main.cpp

```
cmd: pwd
/Users/mdrafiqulrabin/Desktop/os-unix-cmd
cmd: ls
cmd_utils.h		my_utils.h
README.md		main.cpp
cmd: cd ..
cmd: pwd
/Users/mdrafiqulrabin/Desktop
cmd: ls
os-unix-cmd
cmd: whoami
mdrafiqulrabin
cmd: whoami > me.txt
cmd: ls
me.txt		os-unix-cmd
cmd: cat me.txt
mdrafiqulrabin
cmd: wc < me.txt
       1       1      15
cmd: 
```

# References:

- The Linux Programming Interface: https://man7.org/tlpi/
