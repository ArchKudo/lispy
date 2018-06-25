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
- Released lispy v0.0.4!

### Commit 11

- Remove unnecessary function to calculate total number of nodes

### Commit 12

- Move evaluation of ast to seperate [header](./eval.h)
- Add some documentation
- Updated Makefile to include eval

### Commit 13

Created [lval.h](./lval.h) and [lval.c](./lval.c)

- Store numbers and errors as `LVal`'s (Lispy values)
- Create methods to operate on `LVal`'s
- Modify `eval` and `calc` functions to accept and return `LVal`'s
- Add some kind of error handling using enums
- Note: `LERR_BAD_OP` is handled by parser...
- Modified Makefile to include lval.c
- Removed unnecessary includes from files

### Commit 14

- Disabled evaluation for refactoring
- Removed a lot of memory leaks present from Day 1
- LVal struct now holds more fields
- Error are now strings
- A lot of dead code present
- Program compiles and gives correct output

### Commit 15

- Document public/private functions in lval.c
- Evaluate expressions
- Print result of expression rather than expression itself
- Released lispy v0.0.5!
