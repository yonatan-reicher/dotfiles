#!/bin/sh

# This script is intended for POSIX compliant shells only.

# -F is --classify
alias ls='ls -F --color=auto'
alias la='ls -AF --color=auto'
alias ll='echo "permission L user group size date  time  name" && ls -AlFh --color=auto'
alias l='ls -F --color=auto'
