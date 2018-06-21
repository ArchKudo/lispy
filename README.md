# Build your own LISP

Exercises and programs based on this [tutorial](http://www.buildyourownlisp.com).

## Changelog

### Commit 1

Added program [prompt.c](./prompt.c)

- Added a simple user prompt
- Released lispy v0.0.1!

### Commit 2

Updated [prompt.c](./prompt.c) and [Makefile](./Makefile)

- Support for readline and command history.
- Proper error handling of input
- No limit on size of input
- Linked readline library
- Update Makefile
- Release lispy v0.0.2!

### Commit 3

- Fixed typo
- Define TRUE/FALSE in [prompt.c](./prompt.c)

### Commit 4

- Added mpc as submodule

### Commit 5

- Symlinked [mpc.h](./mpc.h) and [mpc.c](./mpc.c)
- Solved some exercises of Chapter 06 at [exercises.md](./exercises.md)
- Created Parsers for 'Reverse Polish notation' and defined language using Regex
- Modified prompt to echo ast on correct syntax or, display error
- Cleanup mpc parsers at exit
- Released lispy v0.0.3!

### Commit 6

- Added modulo(symbol `%`) in list of operators
- Realized parsers doesn't handle bad input (eg: Bad number of operands)

### Commit 7

- Added .clang-format
- Formatted prompt.c

### Commit 8

- Updated .clang-format
- Added .gitignore
- Initial support for evaluating expressions

### Commit 9

- Added some comments
- Renamed some functions
- Refactor if-else

### Commit 10

- Remove debug print statments
- Release lispy v0.0.4!

### Commit 11

- Remove unnecessary function to calculate total number of nodes

### Commit 12

- Move evaluation of ast to seperate [header](./eval.h)
- Add some documentation
- Updated Makefile to include eval
