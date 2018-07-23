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

### Commit 16

- Added support for Quoted(Q)-Expressions
  - In lang (`{}`)
  - In LVal types
- Use macros for simplifying code (`LASSERT`)
- Refactor code for DRY
- Added logic for handling `qexpr`
- Derived `sexpr` functions for use with `qexpr`
- Added operator for `qexpr` like head, tail, etc.

### Commit 17

- Updated prompt to display lispy version v0.0.5
- Added documentation
- Divided source file using comments

### Commit 18

- Release Lispy v0.0.6!

### Commit 19

- Forward declare structs `LVal`, `LEnv` along with their `typedef`'s
- Forward declare function pointer `LBuiltin`
- Update `LVal` struct to include `LBuiltin`
- Declare all public functions of lval.h inside lval.c
- Remove dead code inside lval.h
- Restructure lval.c
- Added comments in lval.c
- Updated prompt.c and Makefile to use libedit instead of readline
- Also added a newline character to exit message
- Commit to make shorter discriptive commits!

### Commit 20

- Define `LEnv` struct
- Modify `lval_wrap_err` to parse variable arguments as error string
- Add `LVal` wrapper for `LBuiltin`'s
- Add function to print type of `LVal`
- Add logic to handle `LBuiltins` in `lval_del`, `lval_print`
- Add a function to deep copy `LVal`'s
- Commented some functions
- More re-ordering and restructuring of files

### Commit 21

- Declare and define functions to operate on `LEnv`'s
- Finally have an idea of how `lenv_get` works
  - i.e the lval passed is itself a symbol instead of a value
