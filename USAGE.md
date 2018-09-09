# A brief overview of lispy builtins

## Simple calculations

lispy like all other lisps follows RPN grammar for describing expressions.

Grammar:

* The builtin operators include: `+, -, *, /, %`
* All numbers are internally represented as long, so watch out for `ERANGE`
* Operations can be wrapped as expressions by using parantheses `(`, `)`
* Each expression starts with an operator followed by one or more expressions

Examples:

```
❯ ./prompt
LISPY v0.0.10
Enter CTRL+C or, CTRL+D on an empty line to exit
lispy> + 1 1 ; Addition
2
lispy> - (+ 1 1 1) ; Substraction is a little tricky with RPN
-3
lispy> - 1 1 1 ; See the difference
-1
lispy> * (+ 1 1 1 (/ 5 3) (- 7 6) (% 5 4)) (1) ; A more complex example, Note the floating point is lost due to internal repr as long ints
6
lispy> / (* 100 2) (/ 45 5) (+ 3 3) ; Yet another example to sum it up
3
lispy> + 9223372036854775808 0 ; Entering numbers > LONG_MAX
Error: Number too large!
lispy> + 9223372036854775807 1 ; This is a bug which requires fixing, Currently only individual numbers are checked and, not the result of expressions
-922337203685477580
```

## S-Expressions

## Q-Expressions

## Strings

## Comments

## Language Builtins

## Lambda Expressions

## Standard Library
