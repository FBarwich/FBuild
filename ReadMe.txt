This will be my own Build-System.

Since I don't care for Linux or simplicity or other compilers than the one 
I use (wich is Visual C++ at the moment, but with Microsofts strongarming with
it's METRO-Crap and them dropping support for XP targets, I'm thinking of
switching to Intel).

All I care for is the best possible performance for incremeltal builds. If
there's nothing to do, it should take no time.

I'm Using Memory Mapped Files for the dependency check.
I'm using NTFS-Streams for Caching.
I'm using multithreading whereever possible (even or the dependency check).
I'm using batch compiliation instead of invoking the compiler for every file.

The "Makefile" is a lua script (http://www.lua.org)

There is no way to state dependencies between projects. The single
projects are just built in the order they are stated in the script.
So, if theres a dependency between projects, the dependee has
to be stated first.

I doubt it's of any use for anyone but me. Nevertheless I make this
public domain, because you never know...
