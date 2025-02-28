# EasyStenography

This is an attempt to make something which is:
- Easier to learn than stenography.
- Still increases typing speed.
- Works for programming in addition to English text.

**This program only works with X11 display server and only on Linux.**

## How it works

There is a big list of abbreviations.
Every time you type one, the program will replace it by the expansion.
To escape one word, type an apostrophe before it (e.g., type `sf` for `self` and `'sf` for `sf`).
When you type a whole word instead of using an abbreviation, a popup window showing showing you what abbreviation you could have used pops up.
This way, you can learn as you go.

It works with things like snake case, punctuation, etc. For example, typing `s_exd(` will be expanded to `is_expected(`.

## Usage

Start the program by running `make run`.
