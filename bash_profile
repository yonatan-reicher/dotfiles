# ~/.bash_profile

if test -r ~/.profile; then
    . ~/.profile
fi

if test -r ~/.bashrc; then
    . ~/.bashrc
fi

# export PS1='\[\e[0;31m\]\u\[\e[0m\]\[\e[1;31m\]@\[\e[0m\]\[\e[0;31m\]\H\[\e[0m\] \w $ '
