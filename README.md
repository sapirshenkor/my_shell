 MyShell Project
=========================

Overview:
---------
This is a custom UNIX-style shell implemented in C for an Operating Systems assignment.
It supports basic built-in commands, redirection, single-stage piping, and execution
of both system binaries and custom utilities (e.g., a hand-written `tree` command).

Project Structure:
------------------
- `main.c`         – Entry point. Follows classic `fork` → `exec` → `wait` pattern.
- `functions.c`    – Implements parsing, built-in command logic, and process control.
- `functions.h`    – Header file with type definitions and function declarations.
- `tree.c`         – A standalone program mimicking the UNIX `tree` command.

Features:
---------
✔ Built-in Commands:
    - `cd <dir>`      – change working directory
    - `pwd`           – print current working directory
    - `clear`         – clear the screen
    - `exit`          – exit the shell cleanly
    - `rmdir <dir>`   – remove an empty directory

✔ External Commands:
    - Any binary in `$PATH` such as `ls`, `cat`, `grep`, `nano`, `chmod`, `cp`, etc.

✔ Shell Capabilities:
    - Output redirection: `command > file` and `command >> file`
    - Piping (one level): `command1 | command2` (e.g., `ls | grep file`)
    - Supports `pwd` as a left-hand command in pipes

✔ Custom Utilities:
    - `tree` – a recursive directory visualizer implemented from scratch (tree.c)

Compilation:
------------
Build the shell and custom tree utility:

    gcc -o my_shell main.c functions.c
    gcc -o tree tree.c

Usage:
------
Run the shell:

    ./my_shell

Then inside the shell, try commands like:

    pwd
    cd /tmp
    ls -l > output.txt
    pwd | grep home
    tree
    exit

Running `tree` inside the shell executes the separately compiled `tree` binary.
 
     Run the tree---->  ./tree

Example Tree Output:
--------------------
```
my_folder
├── file1.txt
├── dir1
│   ├── file2.txt
│   └── subdir
│       └── file3.txt
├── dir2
│   └── readme.md
```

Notes:
------
- Redirection and pipe logic is handled in `executeCommand()` in `functions.c`.
- `cd` and `exit` must be handled in the parent process, not a child.
- The shell uses `execvp()` to run external commands.
- `tree` is not part of the shell binary – it is executed independently.