# Hangmane Game
C command-line Hangman Game using forks ( Father and Child process ), Signals, and pipes.

## Requirement

- GCC compilator
- C/C++ IDE
- Linux OS

## How to use Hangman?

The `hangman` executable expects a defined dictionary file .

Example:

Hangman will import the dictionary file & select a random word from it. Then it will show you the word & your tries left, and ask you to input a letter.

Beginning of a game example:

```
Guess the word : ----
type a character: a
Incorrect.

Attempt remaining: 4
Guess the word : ----
type a character: e
Correct

Guess the word : --e-
type a character:
```
