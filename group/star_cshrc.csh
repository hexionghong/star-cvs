#
# What is here is equivalent to what should be excuted in a cshrc
# i.e. aliases, terminal settings and key bindings.
#
# Note that this file is executed at EACH processes so DO NOT add
# unecessary instructions here.
#
# J.Lauret May 2001
#
setenv star_cshrc_csh 1
if ( ! $?star_login_csh ) then
    #
    # OK. Missing environment variables ... 
    # This is actually the case in 'r' or 's'-service calls.
    #
    if( ! $?AFS_RHIC)   setenv AFS_RHIC  /afs/rhic
    if( ! $?GROUP_DIR ) setenv GROUP_DIR $AFS_RHIC/star/group/

    if( -r $GROUP_DIR/star_login.csh ) then
    	source $GROUP_DIR/star_login.csh
    endif
endif


# --------------------------------------------------------------------------
# Aliases. Backward compatibility only.
# Somehow, I am sure that most of it is unused ...
# I have eliminated the following non-portable commands
#
# alias pp   '/bin/ps auxww | /bin/egrep '\''PID|\!*'\'' | /bin/grep -v grep'
# alias rs   'set noglob;eval `/usr/bin/X11/resize`;unset noglob'
# --------------------------------------------------------------------------
unalias ls
alias   nman       '/usr/bin/nroff -man \!* | $PAGER'
alias   ll         '/bin/ls -lA'
alias   pwd        'echo $cwd'
alias   h          'history'
alias   l          'ls -lt'
alias   lf         'ls -CF'
alias   terminal   'setenv TERM `/usr/bin/tset - \!*`'

# YP fix
if ( -x "/bin/domainname" ) then
    if( ! $?DOMAINNAME) then
	setenv DOMAINNAME `/bin/domainname`
    endif
    if ($DOMAINNAME != "") then
	alias passwd /usr/bin/yppasswd
    endif
endif


# This group file might be merged later on.
if( -r $GROUP_DIR/group_aliases.csh ) then
    source $GROUP_DIR/group_aliases.csh
endif



# -------------------------------------------------
# Now, this is the time to set the tty
# There will be last ultimate call to a group_login
# script which is a file containing commands NOT
# requird in batch mode.
# -------------------------------------------------


# support for several su mode
if ( -o /bin/su ) then
    if($USER == "root") then
	# root prompt
	alias setprompt 'set prompt="%s%b%m#%.04/> "'
    else
	# to another user ...
	alias setprompt 'set prompt="%s%b%m|%.04/> "'
    endif
else
    # user - bold %S
    alias setprompt 'set prompt="%B%S[%m]%s%b %.04/> "'
endif


# support csh/tcsh
set filec
set fignore=( .o .dvi .aux .toc .log .blg .bbl .bak .BAK .sav .old .trace)
set noclobber               
set ignoreeof
set notify
set savehist=50
set history=100

switch ($shell)
    case "/usr/local/bin/tcsh":
    case "/bin/tcsh":
	set correct = cmd
	set autolist=on
	set listjobs=long
	set showdots=on
	set ellispis=1
	set histfile=~/.history.$HOST
	set autologout=1440
	breaksw
    default:
	alias cd 'chdir \!* && setprompt'
	breaksw
endsw

# key bindings. 
if ($?tcsh) then
    bindkey '^[[1~'  exchange-point-and-mark
    bindkey '^[[2~'  overwrite-mode
    bindkey '^[[3~'  delete-char-or-list
    bindkey '^[[4~'  set-mark-command
    bindkey '^[[5~'  history-search-backward
    bindkey '^[[6~'  history-search-forward
    bindkey '^[Ol'   kill-line
    bindkey '^[Om'   yank
    bindkey '^[On'   set-mark-command
    bindkey '^[Op'   exchange-point-and-mark
    bindkey '^[Oq'   forward-word
    bindkey '^[Or'   spell-line
    bindkey '^[Os'   copy-prev-word
    bindkey '^[Ot'   beginning-of-line
    bindkey '^[Ou'   which-command
    bindkey '^[Ov'   end-of-line
    bindkey '^[Ow'   backward-word
    bindkey '^[Ox'   yank
    bindkey '^[Oy'   kill-region
endif
 
# This was taken from HEPIX 100 % as-is
# Make sure csh.login will be sourced on IRIX
if ( ! $?SHLVL ) then
    setenv SHLVL 1
endif   

setprompt




