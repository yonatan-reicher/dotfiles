#!/bin/bash

# If not running interactively, don't do anything
[[ $- != *i* ]] && return

# More intuitive `**` in paths
shopt -s globstar

# Source - https://stackoverflow.com/a/28938235
# Posted by Shakiba Moshiri, modified by community. See post 'Timeline' for change history
# Retrieved 2025-11-10, License - CC BY-SA 4.0

# Reset
C0='\033[0m'       # Text Reset
C1='\033[48;2;40;60;80m'
C2='\033[48;2;50;80;120m'
export PS1="\[${C1}\] \W \[${C2}\] \$ \[${C0}\] "

. ~/.config/aliases/all.sh
. ~/.config/aliases/posix.sh
