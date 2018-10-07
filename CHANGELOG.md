# Changelog

## Update 1

Added program [prompt.c](./prompt.c)

- Added a simple user prompt
- Released lispy v0.0.1!

## Update 2

Updated [prompt.c](./prompt.c) and [Makefile](./Makefile)

- Support for readline and command history.
- Proper error handling of input
- No limit on size of input
- Linked readline library
- Update Makefile
- Released lispy v0.0.2!

## Update 3

- Fixed typo
- Define TRUE/FALSE in [prompt.c](./prompt.c)

## Update 4

- Added mpc as submodule

## Update 5

- Symlinked [mpc.h](./mpc.h) and [mpc.c](./mpc.c)
- Solved some exercises of Chapter 06 at [exercises.md](./exercises.md)
- Created Parsers for 'Reverse Polish notation' and defined language using Regex
- Modified prompt to echo ast on correct syntax or, display error
- Cleanup mpc parsers at exit
- Released lispy v0.0.3!

## Update 6

- Added modulo(symbol `%`) in list of operators
- Realized parsers doesn't handle bad input (eg: Bad number of operands)

## Update 7

- Added .clang-format
- Formatted prompt.c

## Update 8

- Updated .clang-format
- Added .gitignore
- Initial support for evaluating expressions

## Update 9

- Added some comments
- Renamed some functions
- Refactor if-else

## Update 10

- Removed debug print statments
- Released lispy v0.0.4!

## Update 11

- Removed unnecessary function to calculate total number of nodes

## Update 12

- Move evaluation of ast to seperate [header](./eval.h)
- Add some documentation
- Updated Makefile to include eval

## Update 13

Created [lval.h](./lval.h) and [lval.c](./lval.c)

- Store numbers and errors as `LVal`'s (Lispy values)
- Create methods to operate on `LVal`'s
- Modify `eval` and `calc` functions to accept and return `LVal`'s
- Add some kind of error handling using enums
- Note: `LERR_BAD_OP` is handled by parser...
- Modified Makefile to include lval.c
- Removed unnecessary includes from files

## Update 14

- Disabled evaluation for refactoring
- Removed a lot of memory leaks present from Day 1
- LVal struct now holds more fields
- Error are now strings
- A lot of dead code present
- Program compiles and gives correct output

## Update 15

- Document public/private functions in lval.c
- Evaluate expressions
- Print result of expression rather than expression itself
- Released lispy v0.0.5!

## Update 16

- Added support for Quoted(Q)-Expressions
  - In lang (`{}`)
  - In LVal types
- Use macros for simplifying code (`LASSERT`)
- Refactor code for DRY
- Added logic for handling `qexpr`
- Derived `sexpr` functions for use with `qexpr`
- Added operator for `qexpr` like head, tail, etc.

## Update 17

- Updated prompt to display lispy version v0.0.5
- Added documentation
- Divided source file using comments

## Update 18

- Released lispy v0.0.6!

## Update 19

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

## Update 20

- Define `LEnv` struct
- Modify `lval_wrap_err` to parse variable arguments as error string
- Add `LVal` wrapper for `LBuiltin`'s
- Add function to print type of `LVal`
- Add logic to handle `LBuiltins` in `lval_del`, `lval_print`
- Add a function to deep copy `LVal`'s
- Commented some functions
- More re-ordering and restructuring of files

## Update 21

- Declare and define functions to operate on `LEnv`'s
- Finally have an idea of how `lenv_get` works
  - i.e the lval passed is itself a symbol instead of a value

## Update 22

- Updated `LASSERT` to be variadic
- Added other assertions
- Added some documentation on the macros
- Breaks previous `LASSERT`'s

## Update 23

- Fix `LASSERT_NOT_EMPTY` to check for children of child of `LVal` instead, of child itself
- Update various builtins signature to pass `LEnv`'s
- Rename `LASSERT_*` to `LASSERT_CHILD_*` for explicitness
- Accidentally replaced all `a`'s with `lval` in comments..
  - Similarly with `e` and `lenv`
  - Resolved using my superb REGEX skills and determination!
- Also made some corrections along the way to comments
- For some weird reason I find `An LVal, An LEnv` more natural than using `A ...`
- Builtins like list, head, tail, eval, etc. now also require passing an LEnv
  - Passed an `LEnv` even when not required,
  - which led to -Wunused-parameter which was resolved adding a `void` statment
- Added modulo operator logic in `builtin_op` which to my suprise was missing!
- Added methods to declare and intialize `builtin`'s inside `LEnv`'s

## Update 24

- Update signature and definition of `lval_eval*` to accept `LEnv` for getting symbols
- Also updated their documentation
- Improved error-handling in `lval_eval_sexpr`
- Fixed `lval_eval` signature in `builtin_eval` (which should have been fixed already in earlier commits...)

## Update 25

- Update documentation for `lval_eval` from previous commit
- Made some `LEnv` related functions public for prompt.c
- Created a global `LEnv` for storing session symbols in prompt.c
- Updated `sym` regex
- Still need to debug some memory managment issues but somewhat complete
  - Like dumps core on passing wrong number of arguments to `def`
  - Reachable blocks are still present even after moving from readline to libedit

## Update 26

- Turns out: >   - Like dumps core on passing wrong number of arguments to `def`
  - Dumps core for every builtin
- Fixed the above described bug by removing typo s/`sizeof(MAX_ERR)`/`MAX_ERR`
- Fixed some error string formatting
- Add modulo to list of symbols
- Still a bit hazy how `LBuiltin`'s work as a whole..
- Released lispy v0.0.7!

## Update 27

- Modify `LVal` struct to have additional fields related to `LBuiltin`'s
- s/`fun`/`lbuiltin`

## Update 28

- Added wrapper function for lambda expressions
- Fixed a typo
- Added cases for `LVAL_FUN` to handle builtins and user-defined lambda expressions
- Code does not compile due to `lenv_copy` not defined

## Update 29

- Builtin for lambda expressions
- Added builtin to `lenv_add_builtin`
- Still does not compile, due to previous error.

## Update 30

- Added field `parent` inside `LEnv` struct
- Updated `lenv_new` to also intialize `parent`
- Updated `lenv_get` to check for local and global environment for variables
- Implemented `lenv_copy` to copy environments
- Implemented `lenv_put_global` to add variable directly to parent environment rather than local.
- Typecast `lenv` to `void` in `builtin_lambda`
- ~Won't compile~ Segfaults (hopefully) due to unimplemented builtins..

## Update 31

- Modified `builtin_def` to `builtin_var` for handling cases of different environment

## Update 32

- Updated macro comments
- Seperate cases for `LVAL_NUM` and `LVAL_FUN` in `lval_del`
- Fix bug in `lval_copy`
- Implement `lval_call`
- Invoke `lval_call` instead of `lbuiltin` in `lval_eval_sexpr`
- Fixed whitespace
- Added/Modified comments
- Replace `lval_wrap_str` with `lval_wrap_sym`
- Removed stupid allocation bug in `lval_copy`! Code runs!

## Update 33

- Replaced Commit messages in this readme with updates..
- Changed `first` to `lfun` in `lval_eval_sexpr` which I find more fitting..
- Grouped builtins in `lenv_init_builtins`
- Removed unnecessary level of indentation inside quotes

## Update 34

- Added code to handle variable number of arguments
- Fixed some comments/variables
- Realized `builtin_sub` is broken!

## Update 35

- Added some comments
- Released lispy v0.0.8!

## Update 36

- Added builtins for conditionals
- Added declaration of a couple of functions and descriptive comments
- Added if builtin
- Added a missing `\n`
- Moved some code within the file

## Update 37

- Released lispy v0.0.9!

## Update 38

- Support for strings
- Updated corresponding functions/parser syntax
- Added support functions for printing/reading strings
- Added related documentation
- Also some code movement
- Added support for comments

## Update 39

- Parsers are now declared in [parsers.h](./parsers.h) for sharing globals
- Added support for loading files using `mpc_parse_contents`
- Added support for printing strings
- Added support for creating errors from strings
- Added more functions required by `prompt.c` to `lval.h`
- Added support for running files using command-line arguments
- Released lispy v0.0.10!

## Update 40

- Replaced
  - `Notation` => `Lisp`
  - `lenv` => `lparent`
- Reorder assignments / parameters
- Moved changelog to here
- Started documenting `lispy` features in [usage.md](./usage.md)
- Added stub files for standard library [stdlib.lspy](./stdlib.lspy) and,
- Explanations of internals at [INTERNALS.md](./INTERNALS.md)

## Update 41

- Split [lval.c](./lval.c)
- Added declarations for `lval_copy`
- Updated Makefile

## Update 42

- Moved `*.c` files to [src/](./src/)
- Moved `*.h` files to [include/](./include/)
- Updated Makefile
- Changed symlinks for including additional depth
