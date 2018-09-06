# Build your own LISP

Exercises and programs based on this [tutorial](http://www.buildyourownlisp.com).

## Install and build instructions

- For linux:

```bash
~
❯ cd Projects

~/Projects
❯ git clone git@github.com:ArchKudo/lispy.git
Cloning into 'lispy'...
remote: Counting objects: 201, done.
remote: Compressing objects: 100% (62/62), done.
remote: Total 201 (delta 134), reused 201 (delta 134), pack-reused 0
Receiving objects: 100% (201/201), 45.64 KiB | 44.00 KiB/s, done.
Resolving deltas: 100% (134/134), done.

~/Projects 15s
❯ cd lispy

~/Projects/lispy master
❯ git submodule update --init
Submodule 'mpc' (git@github.com:orangeduck/mpc.git) registered for path 'mpc'
Cloning into '/home/deen/Projects/lispy/mpc'...
Submodule path 'mpc': checked out '2b05913b14e04c436fc1af6776a037d2d6a9dc23'

~/Projects/lispy master 15s
❯ make
gcc -std=c99 -pedantic -O3 -g -Wall -Werror -Wextra -c -o prompt.o prompt.c
gcc -std=c99 -pedantic -O3 -g -Wall -Werror -Wextra -c -o mpc.o mpc.c
gcc -std=c99 -pedantic -O3 -g -Wall -Werror -Wextra -c -o lval.o lval.c
gcc -std=c99 -pedantic -O3 -g -Wall -Werror -Wextra -ledit -lncurses -o prompt prompt.o mpc.o lval.o

~/Projects/lispy master
❯ ./prompt
LISPY v0.0.10
Enter CTRL+C or, CTRL+D on an empty line to exit
lispy> ^D
Bye!
```

## Usage

For help on using the interactive prompt, see [USAGE.md](./USAGE.md)

## Internals

More about the implementations can be read about in [INTERNALS.md](./INTERNALS.md)

## Changelog

For changelog, see here: [CHANGELOG.md](./CHANGELOG.md)

## TODO

- [ ] Add tests / coverage metrics / CI
- [ ] Replace `mpc` with a handmade parser
- [ ] More internal types
- [ ] Use more C11 features
- [ ] Optional typing support
- [ ] Add/Improve `stdlib`
- [ ] Make the prompt more interactive
  - [ ] Support for expanding to newlines
  - [ ] `exit` command

## TODO (Meta)

- [ ] Add git-hooks
  - [ ] To enforce updating changelog before commit
  - [ ] Beautify files
  - [ ] Provide workflow for merging
- [ ] Explaing usage of lispy in [USAGE.md](./USAGE.md)
- [ ] Expain implementation in [INTERNALS.md](./INTERNALS.md)
