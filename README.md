A repo for my configuration!

Note: Neovim configuration stored in a different repo (under the name "init.lua"
on GitHub).

Supports an install script written in C that can be ran with `make`. In the
future, might support Windows, for now only Mac and Linux.

## Currently Working On

Removing "cwalk" and switching it with an implementation of my own.
Documenting this code.

## TODO

Nicer logging from the installer.
Put neovim configuration here, or, make the installer clone the neovim
configuration, initialize the project (submodules) and move it to the given
folder. It would also be nice to have a "bare-bones" neovim configuration, for
when you are in a machine that does not have enough space or has an old neovim
version, or is missing some critical tools. Need to benchmark the space that the
repo takes with all the submodules and see if it's worth it.

Move gitconfig to git/config, I think this way neovim will show us syntax
highlighting for the file.
