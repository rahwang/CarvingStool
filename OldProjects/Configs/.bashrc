set -o emacs

alias la="ls -a"
alias e="emacs"
alias grep='grep --color=auto'
alias josh='nc joshmiller.no-ip.biz 9000'
alias ls="ls -FG"
alias starwars="telnet towel.blinkenlights.nl"
export LSCOLORS="ExGxBxDxCxEgEdxbxgxcxd"
export PATH=/Users/rah/scripts:/usr/local/bin/:$PATH
function emacsf
{
    if [ -e "$@" ]
    then
        command open -a /Applications/Emacs.app "${@}"
    else
        touch "$@"
        command open -a /Applications/Emacs.app "${@}"
    fi
}
alias emacs="emacsf"

PS1='\n
${debian_chroot:+($debian_chroot)}\[\033[01;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\[\033[32m\]\$ \
\[\033[00m\]' 


# Setting PATH for Python 2.7
# The orginal version is saved in .bash_profile.pysave
PATH="/Library/Frameworks/Python.framework/Versions/2.7/bin:~/scripts:${PATH}"
export PATH

# Added by Canopy installer on 2013-07-07
# VIRTUAL_ENV_DISABLE_PROMPT can be set to '' to make bashprompt show that Canopy is active, otherwise 1
VIRTUAL_ENV_DISABLE_PROMPT=1 source /Users/rah/Library/Enthought/Canopy_32bit/User/bin/activate

# added by Anaconda 1.6.1 installer
export PATH="//anaconda/bin:$PATH:/Users/rah/scripts"
