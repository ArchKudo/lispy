# Exercise soulutions

## Chapter 06. Parsing

Q.1. Write a regular expression matching strings of all a or b such as aababa or bbaa.

Ans: `[ab]+`

Q.2. Write a regular expression matching strings of consecutive a and b such as ababab or aba.

Ans: `\b((ab)+|(ba)+)+\b`

Q.3. Write a regular expression matching pit, pot and respite but not peat, spit, or part.

Ans: `^.{0}(.{3})?(p[io]t)`
