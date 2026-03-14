#!/bin/sh
export XDG_CONFIG_HOME=$HOME/.config
export XDG_CACHE_HOME=$HOME/.cache
export XDG_DATA_HOME=$HOME/.local/share
export XDG_STATE_HOME=$HOME/.local/state
export XDG_DATA_DIRS=/usr/local/share:/usr/share
# export XDG_CONFIG_DIRS=/etc/xdg
export EDITOR='nvim'
export VISUAL='nvim'
export TERMINAL='kitty'
export GREP_COLOR='1;36'
export HISTCONTROL='ignoredups'
export HISTSIZE=5000
export HISTFILESIZE=5000
export LSCOLORS='ExGxbEaECxxEhEhBaDaCaD'
export MANPAGER='nvim --clean +Man!'
export TZ='America/New_York'
