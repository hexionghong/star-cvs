#!/bin/csh
#       $Id: group_env.csh,v 1.238 2011/03/03 19:21:50 jeromel Exp $
#	Purpose:	STAR group csh setup
#
# Revisions & notes
#    2001-2009  Maintained J. Lauret
#    24 Apr 01  J. Lauret  Disabled echoing in ! prompt.
#                          DO NOT MODIFY THIS !!!
#     2 Apr 01  J. Lauret  Insure path added
#     3 Mar 98  T. Wenaus  HP Jetprint added (for sol)
#    17 Feb 98  Created Y.Fisyak (BNL)
#
# Should be loaded by star_login itself loaded executed by
# our individual .login files.
#
#
set ECHO = 1;
set FAIL = "";

if ($?STAR == 1)   set ECHO = 0
if ( ! $?prompt)   set ECHO = 0
if ($?SILENT == 1) set ECHO = 0

# This variable was added for the ECHOD debug mode
set self="group_env"
if ( $?DECHO && $?STAR_LEVEL ) then
    echo "$self :: Receiving STAR_LEVEL $STAR_LEVEL"
endif


# possible path for utilities
setenv AFS       /usr/afsws


# check if AFS_RHIC is readable
set READ_AFS=`echo $AFS_RHIC | /bin/grep Path_Not_Found`

if ( $?DECHO) echo "$self :: READ_AFS is [$READ_AFS]"

if (! $?STAR_ROOT) then
    if ( $?DECHO) echo "$self :: checking STAR_ROOT"
    if ( "$READ_AFS" == "") then
	if ( $?DECHO) echo "$self ::  Defining STAR_ROOT as AFS based if -d checks"
	if ( -d ${AFS_RHIC}/star ) then
	    setenv STAR_ROOT ${AFS_RHIC}/star
        endif
    else
       if ( -d /usr/local/star ) then
	    # this is valid
	    if ( $?DECHO) echo "$self ::  Defining STAR_ROOT as /usr/local/star"
	    setenv STAR_ROOT /usr/local/star
       else
	    # We will fail (we know that)
	    echo "$self ::  Did not find a valid STAR_ROOT"
	    setenv STAR_ROOT /Path_Not_Found_STAR_Login_Failure
	    set FAIL="$FAIL STAR_ROOT"
       endif
    endif
endif


# Define /opt/star (or equivalent)
# X indicates points to the AFS reference
if ( ! $?XOPTSTAR ) then
    # keep a reference to the AFS one
    # this -e test may fail - don't do it
    if ( "$READ_AFS" == "" ) then
	setenv XOPTSTAR ${AFS_RHIC}/opt/star
    endif
endif

if ( ! $?OPTSTAR ) then
    # local first - BEWARE this may be a link over
    # AFS as well and as it turns out, -e test locks as well

    # there is not even a /opt, -e will be safe
    # if there is no star in /opt, also safe to do -e
    # note that ALL ls must be escaped to avoid argument aliasing
    # forcing color, fancy display etc ... all doing a form of stat
    # hence locking again
    set IS_OPTSTAR_AFS=""
    set TEST=""

    if ( -d /opt ) then
        set TEST=`/bin/ls /opt/ | /bin/grep star`
	if ( "$TEST" == "star" )  then
            set IS_OPTSTAR_AFS=`/bin/ls -ld /opt/star | /bin/grep afs`
	endif
    endif

    if ( "$IS_OPTSTAR_AFS" == "" || "$READ_AFS" == "") then
	if ( $?DECHO) echo "$self :: Safe to test -e on /opt/star"
	if ( -e /opt/star ) then
	    setenv  OPTSTAR /opt/star
	endif
	#else -> note that eventually, we could set blindly OPTSTAR if TEST!=""
    endif

    # remote second
    if ( $?DECHO) echo "$self :: Not safe to check /opt/star OPTSTAR_AFS=[$IS_OPTSTAR_AFS] READ_AFS=[$READ_AFS]"
    if ( $?XOPTSTAR && ! $?OPTSTAR ) then
        setenv OPTSTAR ${XOPTSTAR}
    else
        setenv FAIL "$FAIL OPTSTAR"
    endif
endif

# define but feedback later
if ( $?DECHO) echo "$self :: Defining GROUP_DIR STAR_PATH"
if ( ! $?GROUP_DIR )   setenv GROUP_DIR ${STAR_ROOT}/group     # Defined to AFS Group Dir
if ( $?STAR_PATH == 0) setenv STAR_PATH ${STAR_ROOT}/packages;


if ( $?DECHO) echo   "$self :: Value of GROUP_DIR = ${GROUP_DIR}"

# make this additional test ahead
if ( ! -e $STAR_PATH ) then
    set FAIL="$FAIL STAR_PATH"
endif


if ( "$FAIL" != "") then
    if ($?DECHO) echo "$self :: FAIL is [$FAIL], something is not right (checking)"

    # we can add this only now because setup may be AFS-free
    if ( "$READ_AFS" != "" ) then
	set FAIL="$FAIL AFS_RHIC"
    endif

    if ($ECHO) then
	echo ""
	echo "***************************************************************"
	echo "  ERROR Cannot find a valid Path for                           "
	echo "    $FAIL                                                      "
	echo "  STAR Login is incomplete                                     "
	echo "                                                               "

	# we can try to guess the reason but it may not be the whole story
	set failafs=0
	if ( `echo $FAIL | /bin/grep AFS` != "" &&  `echo $FAIL | /bin/grep STAR_PATH` != "") then
	    # if AFS detection failed and STAR_PATH was not defined we have no options
	    set failafs=1
	endif
	if ( `echo $STAR_ROOT | /bin/grep $AFS_RHIC` != "" &&  `echo $STAR_PATH | /bin/grep $STAR_ROOT` != "" && `echo $FAIL | /bin/grep STAR_PATH` != "") then
	    # ! -e STAR_PATH but defined as AFS resident is the second sign of failure
	    # it does seem like the above but this second test is necessary due to client
	    # file caching
	    set failafs=1
	endif

	if ( $failafs ) then
	echo "  Reason: It appears the AFS lookup has failed and             "
	else
	# any other reason, display a generic message
	echo "  Reason: Improper or incomplete installation                  "
	endif
	echo "          You do not have a local installation of the STAR     "
	echo "          software stack.                                      "
	echo "                                                               "
	echo "    If you are installing for the first time, ignore & proceed "
	echo "    with installation. Our documentation is available at       "
        echo "    http://drupal.star.bnl.gov/STAR/comp/sofi/installing       "
	echo "***************************************************************"
	echo ""
	# disable messages
	if( ! $?DECHO) set ECHO = 0
    endif
else
    if ($?DECHO) echo "$self :: FAIL is NULL, we are fine so far"
    if ($ECHO) then
	echo ""
	echo "         ----- STAR Group Login from $GROUP_DIR/ -----"
	echo ""
	echo "Setting up STAR_ROOT = ${STAR_ROOT}"
	echo "Setting up STAR_PATH = ${STAR_PATH}"
    endif
endif


setenv WWW_HOME http://www.star.bnl.gov/
if ($ECHO) echo   "Setting up WWW_HOME  = $WWW_HOME"


# Defined in CORE. GROUP_PATH/GROUPPATH are global
# while GROUP_DIR may be local
if ( ! $?GROUP_PATH )  setenv GROUP_PATH ${STAR_ROOT}/group
setenv GROUPPATH  $GROUP_PATH




# Default value (some if not already defined)
if ($?STAR_LEVEL == 0) setenv STAR_LEVEL pro

if ( $?DECHO) echo "$self :: Setting STAR_VERSION"

setenv STAR_VERSION ${STAR_LEVEL}
if ($STAR_LEVEL  == "old" || $STAR_LEVEL  == "pro" || $STAR_LEVEL  == "new" || $STAR_LEVEL  == "dev" || $STAR_LEVEL  == ".dev") then
  # i.e. replace with link value instead
  if ( $?DECHO ) echo "Will test -e $STAR_PATH/${STAR_LEVEL}"
  # exit

  if( -e $STAR_PATH/${STAR_LEVEL}) then
    # be carefull, it may not be "seen" as a soft link
    # at all ... Some AFS client do not show the link.
    # No even speaking of absolute path ...
    if ( $?DECHO ) echo "Will ls -ld $STAR_PATH/${STAR_LEVEL}"
    set a = `/bin/ls -ld $STAR_PATH/${STAR_LEVEL}`
    set b = `/bin/ls -ld $STAR_PATH/${STAR_LEVEL} | /usr/bin/cut -f2 -d">"`
    if ( $?DECHO ) echo "Checked $a $b"
    if ( "$a" != "$b") then
	setenv STAR_VERSION $b
    else
	setenv STAR_VERSION $STAR_LEVEL
    endif
  endif
endif


if ( $?DECHO) echo "$self :: Setting STAF_VERSION"

if ($?STAF_LEVEL == 0) then
 if ( -e $STAR_PATH/StAF/${STAR_LEVEL}) then
    setenv STAF_LEVEL $STAR_LEVEL
 else
    setenv STAF_LEVEL pro
 endif
endif

setenv STAF_VERSION ${STAF_LEVEL}
if ($STAF_LEVEL  == "old" || $STAF_LEVEL  == "pro" || $STAF_LEVEL  == "new" || $STAF_LEVEL  == "dev" || $STAF_LEVEL  == ".dev") then
  if( -e $STAR_PATH/StAF/${STAF_LEVEL}) then
    set a = `/bin/ls -ld $STAR_PATH/StAF/${STAF_LEVEL}`
    set b = `/bin/ls -ld $STAR_PATH/StAF/${STAF_LEVEL} | /usr/bin/cut -f2 -d">"`
    if ( "$a" != "$b") then
	setenv STAF_VERSION $b
    else
	setenv STAF_VERSION ${STAF_LEVEL}
    endif
  endif
endif

#+
# use alternate gcc installations
# Needs to be here because STAR_SYS will set vars based on the
# command 'gcc'
#-
if ( $?USE_GCC_DIR ) then
    if ( -x $USE_GCC_DIR/bin/gcc && -d $USE_GCC_DIR/lib ) then
        # do not redefine it if already done to avoid having
	# a messed up path and ldpath
        if ( ! $?ALT_GCC ) then
            setenv ALT_GCC $USE_GCC_DIR
	
            set path=($USE_GCC_DIR/bin $path)
	    if ( $?LD_LIBRARY_PATH ) then
	        setenv LD_LIBRARY_PATH $USE_GCC_DIR/lib:${LD_LIBRARY_PATH}
	    else
	        setenv LD_LIBRARY_PATH $USE_GCC_DIR/lib
	    endif
	endif
    endif
endif



# Clear this out. First block STAF, second STAR
if ( $?DECHO) echo "$self :: Executing STAR_SYS"
source ${GROUP_DIR}/STAR_SYS;

#
# The above logic forces the creation of "a" compiler
# specific path prior to setting up $OPTSTAR . This was
# made on purpose so the environment would revert to a
# default $OPTSTAR in case things are not quite in place.
#

# There is a second chance to define XOPTSTAR
if ( $?DECHO) echo "$self :: Checking  XOPTSTAR "
if ( ! $?XOPTSTAR ) then
    if ( -e ${AFS_RHIC}/${STAR_SYS}/opt/star ) then
	setenv XOPTSTAR ${AFS_RHIC}/${STAR_SYS}/opt/star
    else
	# well, as good as anything else (we cannot find a
	# global reference)
	setenv XOPTSTAR /dev/null
    endif
endif

if ( $?OPTSTAR ) then
    if (!  $?optstar ) setenv  optstar  ${OPTSTAR}
    if (! $?xoptstar ) setenv xoptstar ${XOPTSTAR}

    if ( -e ${OPTSTAR}/${STAR_HOST_SYS} ) then
	# Redhat > 7.3  transition ; adding one level
	setenv OPTSTAR    ${optstar}/${STAR_HOST_SYS}
    endif
    if ( -e ${xoptstar}/${STAR_HOST_SYS} ) then
	setenv XOPTSTAR  ${xoptstar}/${STAR_HOST_SYS}
    endif
endif


# Display the messages here now
if (  $?OPTSTAR ) then
    if ($ECHO) echo   "Setting up OPTSTAR   = ${OPTSTAR}"
else
    # nothing found, so set it to nothing and the login
    # will be able to proceed (at least, repair will be
    # possible)...
    setenv OPTSTAR

endif
if (  $XOPTSTAR == "/dev/null" ) then
    if ($ECHO) echo   "WARNING : XOPTSTAR points to /dev/null (no AFS area for it)"
else
    if ($ECHO) echo   "Setting up XOPTSTAR  = ${XOPTSTAR}"
endif





# STAF
setenv STAF ${STAR_PATH}/StAF/${STAF_VERSION} ;   if ($ECHO) echo   "Setting up STAF      = ${STAF}"
setenv STAF_LIB  $STAF/.${STAR_HOST_SYS}/lib  ;   if ($ECHO) echo   "Setting up STAF_LIB  = ${STAF_LIB}"
setenv STAF_BIN  $STAF/.${STAR_HOST_SYS}/bin  ;   if ($ECHO) echo   "Setting up STAF_BIN  = ${STAF_BIN}"
# STAR
setenv STAR      $STAR_PATH/${STAR_VERSION}   ;   if ($ECHO) echo   "Setting up STAR      = ${STAR}"
if ( $STAR_LEVEL == "cal" ) then
    if ( ! $?STAR_BIN ) then
	# make a default
	setenv STAR_BIN $STAR_PATH/dev/.${STAR_HOST_SYS}/bin
    endif
    if ( -e $STAR/.${STAR_HOST_SYS}/bin ) then
	# overwrite if exists
	setenv STAR_BIN $STAR/.${STAR_HOST_SYS}/lib 
    endif
    if ( ! $?STAR_LIB ) then
	setenv STAR_LIB $STAR_PATH/dev/.${STAR_HOST_SYS}/lib
    endif
    setenv STAR_lib  $STAR/.${STAR_HOST_SYS}/lib
    setenv MINE_lib  $STAR/.${STAR_HOST_SYS}/lib
else
    setenv STAR_LIB  $STAR/.${STAR_HOST_SYS}/lib
    setenv STAR_BIN  $STAR/.${STAR_HOST_SYS}/bin
endif
                                                  if ($ECHO) echo   "Setting up STAR_LIB  = ${STAR_LIB}"
setenv MINE_LIB        .${STAR_HOST_SYS}/lib
setenv MY_BIN          .${STAR_HOST_SYS}/bin



# YP fix
if( ! $?DOMAINNAME) then
    if ( -x "/bin/domainname" ) then
	setenv DOMAINNAME `/bin/domainname`
    else
	setenv DOMAINNAME "(none)"
    endif

    # Fake it
    if ( "$DOMAINNAME" == "(none)") then
       setenv DOMAINNAME `/bin/hostname | /bin/sed 's/^[^\.]*\.//'`
    endif
endif




#
# ATTENTION - This support for $SITE need extending
# at each new site.
#
# Each Grid site should have an entry.
# Only sites having local DB rules could have an entry.
#
if ( ! $?SITE ) then
    switch ($DOMAINNAME)
	case "nersc.gov":    # <--- or whatever domainame returns
	    setenv SITE "LBL"
	    breaksw

	case "rhic.bnl.gov":
	case "rcf.bnl.gov":
	case "star.bnl.gov":
	case "starp.bnl.gov":
	    setenv SITE "BNL"
	    breaksw

	case "if.usp.br":
	    setenv SITE "USP"
	    breaksw

	case "cluster.phy.uic.edu":
	    setenv SITE "UIC"
	    breaksw

	default:
	    # Not implemented
	    setenv SITE "generic"
	    breaksw
    endsw
endif




# db related
if ( $?SITE ) then
    #if ( ! $?DB_SERVER_LOCAL_CONFIG ) then
	if ( -e ${STAR_PATH}/conf/dbLoadBalancerLocalConfig_${SITE}.xml ) then
	    # 2008/08 new location and unique for all libraries - SL08e or above
	    setenv DB_SERVER_LOCAL_CONFIG ${STAR_PATH}/conf/dbLoadBalancerLocalConfig_${SITE}.xml
	else
	    # old method and value for backward compat - this is the part preventing
	    # from protecting against redefining. In fact, if not in the global
	    # area, we MUST redefine. File was removed from this path starting from
	    # SL10g
	    setenv DB_SERVER_LOCAL_CONFIG ${STAR}/StDb/servers/dbLoadBalancerLocalConfig_${SITE}.xml
	endif
    #endif
endif



# Options my alter *_BIN and/or add *_lib. All options should
# be treated here. Defaults hould be preserved above.
if ($?INSURE) then
  # Do it conditional because this is a late addition.
  # The directory structure may not exist for all library version.
  if( -e $STAR/.${STAR_HOST_SYS}/ILIB) then
   if (-f $GROUP_DIR/parasoftenv.csh) then
     source $GROUP_DIR/parasoftenv.csh
     setenv STAR_lib  $STAR/.${STAR_HOST_SYS}/ILIB ;  if ($ECHO) echo   "Setting up STAR_lib  = ${STAR_lib}"
     setenv MINE_lib        .${STAR_HOST_SYS}/ILIB
     setenv STAR_BIN  $STAR/.${STAR_HOST_SYS}/IBIN
     setenv MY_BIN          .${STAR_HOST_SYS}/IBIN
   else
     if ($ECHO) echo "Setting up STAR_lib  = Insure not found (not set)"
   endif
  else
   if ($ECHO) echo  "Setting up STAR_lib  = Cannot Set (missing tree)"
  endif

else if ($?GPROF) then
  setenv STAR_lib  $STAR/.${STAR_HOST_SYS}/GLIB ;  if ($ECHO) echo   "Setting up STAR_lib  = ${STAR_lib}"
  setenv MINE_lib        .${STAR_HOST_SYS}/GLIB
  setenv STAR_BIN  $STAR/.${STAR_HOST_SYS}/GBIN
  setenv MY_BIN          .${STAR_HOST_SYS}/GBIN

else if ($?NODEBUG) then
  setenv STAR_lib  $STAR/.${STAR_HOST_SYS}/LIB ;  if ($ECHO) echo   "Setting up STAR_lib  = ${STAR_lib}"
  setenv MINE_lib        .${STAR_HOST_SYS}/LIB
  setenv STAR_BIN  $STAR/.${STAR_HOST_SYS}/BIN
  setenv MY_BIN          .${STAR_HOST_SYS}/BIN

else
  if ( $STAR_LEVEL != "cal" ) then
    if ($?DECHO)    echo   "$self :: unseting STAR_lib and MINE_lib for Level=[$STAR_LEVEL]"
    if ($?STAR_lib) unsetenv STAR_lib
    if ($?MINE_lib) unsetenv MINE_lib
  endif
endif

if ($ECHO)    echo   "Setting up STAR_BIN  = ${STAR_BIN}"

# Common stuff
setenv STAR_SCRIPTS $STAR_PATH/scripts
setenv STAR_CGI  $STAR_PATH/cgi
setenv STAR_MGR  $STAR/mgr
setenv STAR_PAMS $STAR/pams;            if ($ECHO) echo   "Setting up STAR_PAMS = ${STAR_PAMS}"
setenv STAR_DATA ${STAR_ROOT}/data;     if ($ECHO) echo   "Setting up STAR_DATA = ${STAR_DATA}"
setenv CVSROOT   $STAR_PATH/repository; if ($ECHO) echo   "Setting up CVSROOT   = ${CVSROOT}"



# The block below will be enabled only if there is botha ROOT_LEVEL
# and a CERN_LEVEL file in $STAR/mgr/. If so, ROOT and CERN levels
# will be set to the explicit version. Otherwise, some historical
# deefault will be assumed.
if ( $?DECHO ) echo "$self :: ROOT_LEVEL and CERN_LEVEL"
if ( -f $STAR/mgr/ROOT_LEVEL && -f $STAR/mgr/CERN_LEVEL ) then
  setenv ROOT_LEVEL `/bin/cat $STAR/mgr/ROOT_LEVEL`
  setenv CERN_LEVEL `/bin/cat $STAR/mgr/CERN_LEVEL`

  # try with post-fix
  if ( -f $STAR/mgr/CERN_LEVEL.${STAR_SYS} ) then
    # Overwrite
    setenv CERN_LEVEL `/bin/cat $STAR/mgr/CERN_LEVEL.${STAR_SYS}`
  endif
  if ( -f $STAR/mgr/CERN_LEVEL.${STAR_HOST_SYS} ) then
    # Overwrite
    setenv CERN_LEVEL `/bin/cat $STAR/mgr/CERN_LEVEL.${STAR_HOST_SYS}`
  endif

  # try with post-fix
  if ( -f $STAR/mgr/ROOT_LEVEL.${STAR_SYS} ) then
    # Overwrite
    setenv ROOT_LEVEL `/bin/cat $STAR/mgr/ROOT_LEVEL.${STAR_SYS}`
  endif
  if ( -f $STAR/mgr/ROOT_LEVEL.${STAR_HOST_SYS} ) then
    # Overwrite
    setenv ROOT_LEVEL `/bin/cat $STAR/mgr/ROOT_LEVEL.${STAR_HOST_SYS}`
  endif

  # now check if CERN exists
  if ( $?CERN ) then
    if ( ! -e $CERN/$CERN_LEVEL ) then
	if ( $?DECHO) echo "$self :: Caught $CERN_LEVEL from config in $STAR/mgr/ but not found"
	setenv CERN_LEVEL pro
    endif
  endif

else
 # this block should really not be expanded - use the
 # method above instead to change version so we do not
 # have to maintain this long list of switch statements  
 switch ( $STAR_VERSION )

  case SL98l:
    setenv ROOT_LEVEL 2.20
    breaksw

  case SL99a:
  case SL99b:
  case SL99c:
    setenv ROOT_LEVEL 2.21
    setenv CERN_LEVEL 99
    breaksw

  case SL99d:
  case SL99e:
    setenv ROOT_LEVEL 2.21.08
    setenv CERN_LEVEL 99
    breaksw

  case SL99f:
  case SL99g:
    setenv ROOT_LEVEL 2.22
    setenv CERN_LEVEL 99
    breaksw

  default:
    setenv ROOT_LEVEL 5.12.00

  endsw
endif


# At this point, CERN_LEVEL should be defined but if not,
# the global setup will define it to a default
if ( -x $GROUP_DIR/setup  ) then
    source $GROUP_DIR/setup CERN
else
    setenv CERN_ROOT  $CERN/$CERN_LEVEL
endif
if ($ECHO) echo   "Setting up ROOT_LEVEL= ${ROOT_LEVEL}"


if ( $?DECHO ) echo "$self :: Paths alter for STAR_MGR, STAR_SCRIPTS STAR_CGI etc ..."
if ( -x ${GROUP_DIR}/dropit) then
    setenv GROUPPATH `${GROUP_DIR}/dropit -p ${GROUP_DIR} -p mgr -p ${STAR_MGR} -p ${STAR_SCRIPTS} -p ${STAR_CGI} -p ${MY_BIN} -p ${STAR_BIN} -p ${STAF}/mgr -p ${STAF_BIN}`
    setenv PATH `${GROUP_DIR}/dropit -p ${OPTSTAR}/bin -p $PATH`
else
    setenv GROUPPATH ${GROUP_DIR}:mgr:${STAR_MGR}:${STAR_SCRIPTS}:${STAR_CGI}:${MY_BIN}:${STAR_BIN}:${STAF}/mgr:${STAF_BIN}
    setenv PATH  ${OPTSTAR}/bin:$PATH
endif


# ROOT
if ( $?DECHO ) echo "$self :: Conditional exec of rootenv.csh"
if ( -f $GROUP_DIR/rootenv.csh) then
  source $GROUP_DIR/rootenv.csh
endif

if ( $?DECHO ) echo "$self :: Re-adjusting xxPATH for OPTSTAR and STAR_PATH"
if ( $?DECHO ) echo "$self :: PATH is now $PATH"
if ( -x ${GROUP_DIR}/dropit) then
  # clean-up PATH
  setenv MANPATH `${GROUP_DIR}/dropit -p ${OPTSTAR}/man -p ${MANPATH}`
  setenv PATH    `${GROUP_DIR}/dropit -p ${PATH} GROUPPATH`
  setenv PATH    `${GROUP_DIR}/dropit -p ${PATH} $STAR_PATH`
  #if ($?LD_LIBRARY_PATH == 1) setenv LD_LIBRARY_PATH `${GROUP_DIR}/dropit -p ${LD_LIBRARY_PATH} $STAR_PATH`
  setenv LD_LIBRARY_PATH `${GROUP_DIR}/dropit -p "${LD_LIBRARY_PATH}" $STAR_PATH`
  if ($?SHLIB_PATH == 1)      setenv SHLIB_PATH      `${GROUP_DIR}/dropit -p ${SHLIB_PATH} $STAR_PATH`

  setenv PATH `${GROUP_DIR}/dropit -p ${GROUPPATH} -p /usr/afsws/bin -p /usr/afsws/etc -p ${OPTSTAR}/bin -p /usr/sue/bin -p /usr/local/bin -p ${PATH}`
endif


## Put mysql on path if available
if ( -d /usr/local/mysql/bin) then
  if ( -x ${GROUP_DIR}/dropit) setenv PATH `${GROUP_DIR}/dropit -p ${PATH} -p /usr/local/mysql/bin`
endif

if ($?MANPATH == 1) then
  ##VP   setenv MANPATH ${MANPATH}:${STAR_PATH}/man
  setenv MANPATH `${GROUP_DIR}/dropit -p ${MANPATH} -p ${STAR_PATH}/man`
else
  setenv MANPATH ${STAR_PATH}/man
endif


if ( $?DECHO ) echo "$self :: OS Specific tasks. Our OS=$STAR_SYS"
switch ($STAR_SYS)
    case "rs_aix*":
        if ( -x ${GROUP_DIR}/dropit) setenv MANPATH `${GROUP_DIR}/dropit -p {$MANPATH} -p /usr/share/man`
        breaksw
    case "alpha_osf32c":
	breaksw
    case "hp700_ux90":
	breaksw

    case "hp_ux102":
      if ($?SHLIB_PATH == 0) setenv SHLIB_PATH
      if ( -x ${GROUP_DIR}/dropit) setenv SHLIB_PATH `${GROUP_DIR}/dropit -p ${SHLIB_PATH} $STAR_PATH`
      if ($?MINE_lib == 1 && $?STAR_lib == 1) then
        setenv SHLIB_PATH `${GROUP_DIR}/dropit -p ${MINE_lib} -p ${MINE_LIB} -p ${STAR_lib} -p ${STAR_LIB} -p ${STAF_LIB} -p ${SHLIB_PATH}`
      else
	if ( -x ${GROUP_DIR}/dropit) setenv SHLIB_PATH `${GROUP_DIR}/dropit -p ${SHLIB_PATH} .${STAR_HOST_SYS}/LIB`
##VP         setenv SHLIB_PATH ${MINE_LIB}:${STAR_LIB}:${STAF_LIB}:${SHLIB_PATH}
        setenv SHLIB_PATH `${GROUP_DIR}/dropit -p ${MINE_LIB} -p ${STAR_LIB} -p ${STAF_LIB} -p ${SHLIB_PATH}`
      endif
      setenv LD_LIBRARY_PATH ${SHLIB_PATH}
      setenv BFARCH hp_ux102
      limit  coredumpsize 0
      breaksw

    case "x8664_*":
    case "i386_*":
      #  ====================
      # make sure that afws is in the path
      if (! -d /usr/afsws/bin) setenv PATH `${GROUP_DIR}/dropit -p $PATH -p ${AFS_RHIC}/i386_redhat50/usr/afsws/bin`


      # PGI
      if ( $?redhat ) then
	# from SL5 onward, stop loading PGI automatically
	set loadPGI=`echo "$redhat < 50" | /usr/bin/bc`
	if ( $loadPGI  ) then
	    if ( $?DECHO ) echo "$self :: RH/SL < 5.0 - will attempt to load PGI"
	    if( -x $GROUP_DIR/setup ) then
		if ( $?DECHO ) echo "$self :: Executing setup PGI"
		source $GROUP_DIR/setup PGI
		if ( $?DECHO ) then
		    echo "$self :: PGI = $PGI"
		endif
	    else
		if ($ECHO)    echo   "Could not setup PGI environment"
	    endif
	endif
	unset loadPGI
      endif


      ## This is no longer used right ??
      #if (-d /usr/local/KAI/KCC.flex-3.4f-1/KCC_BASE) then
      #  setenv KAI /usr/local/KAI/KCC.flex-3.4f-1/KCC_BASE
      #  setenv PATH `${GROUP_DIR}/dropit -p $KAI/bin -p $PATH`
      #endif

      setenv PATH  `${GROUP_DIR}/dropit -p $PATH  -p /usr/local/bin/ddd`
      if ($?LD_LIBRARY_PATH == 0) setenv LD_LIBRARY_PATH


      # Final path adjustement
      if ($?MINE_lib == 1 && $?STAR_lib == 1) then
       setenv LD_LIBRARY_PATH `${GROUP_DIR}/dropit -p ${MINE_lib} -p ${MINE_LIB} -p ${STAR_lib} -p ${STAR_LIB} -p ${STAF_LIB} -p ${LD_LIBRARY_PATH}`
      else
       if ( -x ${GROUP_DIR}/dropit) setenv LD_LIBRARY_PATH `${GROUP_DIR}/dropit -p ${LD_LIBRARY_PATH} .${STAR_HOST_SYS}/LIB`
       setenv LD_LIBRARY_PATH `${GROUP_DIR}/dropit -p ${MINE_LIB} -p ${STAR_LIB} -p ${STAF_LIB} -p ${LD_LIBRARY_PATH}`
      endif

      #  cygwin tcsh has no 'limit' command embedded
      if ( `echo $STAR_SYS | grep _nt` == "") then
	limit  coredump 0
	setenv BFARCH Linux2
      endif
      breaksw

    case "sun4*":
      #  ====================
      # Sun/Solaris version 4
      #  ====================
      if ( ! $?SUNWS ) then
	if ( -r $STAR_MGR/sunWS ) then
	    setenv SUNWS `/bin/cat $STAR_MGR/sunWS`
	    if ( ! -d /opt/$SUNWS ) then
		if ($ECHO) echo "$SUNWS Workshop not found. Reverting to SUNWspro"
		setenv SUNWS "SUNWspro"
	    endif
        else
	    # default packages distribution directory
	    setenv SUNWS "SUNWspro"
	endif
      endif

      if (! $?SUNOPT) setenv SUNOPT /opt

      set WSVERS=`echo $SUNWS  | /bin/sed "s/WS//"`   # full version number
      set WSMVER=`echo $WSVERS | /bin/sed "s/\..*//"` # major version number

      if ($?LD_LIBRARY_PATH == 0) setenv LD_LIBRARY_PATH
      setenv LD_LIBRARY_PATH `${GROUP_DIR}/dropit -p /usr/openwin/lib -p /usr/dt/lib -p /usr/local/lib -p ${LD_LIBRARY_PATH}`


      # Rebuild path - Basic
      if ( -x ${GROUP_DIR}/dropit) then
	setenv LD_LIBRARY_PATH `${GROUP_DIR}/dropit -p ${LD_LIBRARY_PATH} -p $SUNOPT/$SUNWS/lib -p $SUNOPT/$SUNWS/SC$WSVERS/lib -p $SUNOPT/$SUNWS/WS$WSMVER/lib`
	setenv PATH `${GROUP_DIR}/dropit -p $SUNOPT/$SUNWS/bin -p ${PATH}`
	setenv MANPATH `${GROUP_DIR}/dropit -p $SUNOPT/$SUNWS/man -p ${MANPATH}`

	if ($?MINE_lib == 1 && $?STAR_lib == 1 ) then
	    setenv LD_LIBRARY_PATH `${GROUP_DIR}/dropit -p ${MINE_lib} -p ${MINE_LIB} -p ${STAR_lib} -p ${STAR_LIB} -p ${STAF_LIB} -p ${LD_LIBRARY_PATH}`
        else
	    setenv LD_LIBRARY_PATH `${GROUP_DIR}/dropit -p ${MINE_LIB} -p ${STAR_LIB} -p ${STAF_LIB} -p ${LD_LIBRARY_PATH}`
        endif
      endif

      setenv  BFARCH SunOS5
      if ("${STAR_HOST_SYS}" == "sun4x_56_CC5") setenv BFARCH SunOS5_CC5
      limit   coredump 0
      unlimit descriptors
      breaksw

    case "alpha_dux*":
      limit datasize unlimited
      limit stacksize unlimited
      breaksw

    default:
	#  ====================
	breaksw
endsw

if ( $?DECHO ) echo "$self :: PATH is now $PATH"

# ==================================================================
# Extra package support
# ==================================================================
if ( $?DECHO ) echo "$self :: Extraneous packages check"

# Support for LSF
if ( -d /usr/local/lsf/bin && ! $?LSF_ENVDIR ) then
    if ( -e /etc/profile.d/lsfsetup.csh ) then
	source /etc/profile.d/lsfsetup.csh
    else
	setenv LSF_DIR    /usr/local/lsf
	setenv LSF_ENVDIR $LSF_DIR/mnt/conf
	set path=($path $LSF_DIR/bin)
	setenv MANPATH ${MANPATH}:$LSF_DIR/mnt/man
    endif
endif
if ( $?LSF_ENVDIR && ! $?LSF_MANPATH ) then
    # may not be full proof
    if ( $?LSF_BINDIR ) then
	set ver=`echo $LSF_BINDIR | /bin/sed "s/\// /g" | /bin/awk '{print $3}'`
	if ( -d /usr/lsf/$ver/man ) then
	    setenv MANPATH  ${MANPATH}:/usr/lsf/$ver/man
	    setenv LSF_MANPATH /usr/lsf/$ver/man
	endif
    endif
endif


# Support for JAVA/JDK
if ( ! $?JAVA_ROOT ) then
    # Search for a default path
    if ( -d /usr/java ) then
	set a = `/bin/ls /usr/java | /usr/bin/tail -1`
	if ( "$a" != "") then
	    setenv JAVA_ROOT /usr/java/$a
	endif
    else
	if ( -d /opt/VDT ) then
	    set a = `/bin/ls /opt/VDT | /bin/grep -e jdk -e j2sdk | /usr/bin/tail -1`
	    if ( "$a" != "") then
		setenv JAVA_ROOT /opt/VDT/$a
	    endif
	endif
    endif
endif
if ( $?JAVA_ROOT ) then
    if ( -d $JAVA_ROOT/ ) then
	if ( `echo $PATH | /bin/grep kerberos` != "") then
	    # Will need to find a better way ... java has
	    # a 'kinit'
	    set path=(/usr/kerberos/bin $JAVA_ROOT/bin $path)
	else
	    set path=($JAVA_ROOT/bin $path)
	endif
	setenv MANPATH ${MANPATH}:$JAVA_ROOT/man
	#CLASSPATH anyone ??
    endif
endif


# Support for GraXML
if ( ! $?GRAXML_HOME && -d ${STAR_PATH}/GeoM ) then
    if ( -d ${STAR_PATH}/GeoM/${STAR_LEVEL}/GraXML ) then
	setenv GRAXML_HOME ${STAR_PATH}/GeoM/${STAR_LEVEL}/GraXML
    else
	# revert to a default if exists
	if ( -e ${STAR_PATH}/GeoM/dev/GraXML ) then
	    setenv GRAXML_HOME ${STAR_PATH}/GeoM/dev/GraXML
	endif
    endif
endif
if ( $?GRAXML_HOME ) then
    set path=($path $GRAXML_HOME/bin)
endif


# Support for subversion if installed in a sub-directory
# Will start with simple one location
if ( -d /usr/local/subversion ) then
    setenv SVNDIR /usr/local/subversion
    set path=($path $SVNDIR/bin )
    setenv MANPATH ${MANPATH}:$SVNDIR/man
    setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:$SVNDIR/lib
endif


# Support for Qt
if (! $?QTDIR ) then
    if ( -d $OPTSTAR/qt ) then
	setenv QTDIR $OPTSTAR/qt
	# set path=($path $QTDIR/bin)
	setenv MANPATH ${MANPATH}:$QTDIR/man
	setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:$QTDIR/lib
    else
	# make some more conditional logic - we assume
	# those will be soft-links form example. Qt4 will
	# take precedence over Qt3 in this scheme
	if ( ! $?QTDIR && -d $OPTSTAR/qt4 ) then
	    setenv QTDIR $OPTSTAR/qt4
	endif
	if ( ! $?QTDIR && -d $OPTSTAR/qt3 ) then
	    setenv QTDIR $OPTSTAR/qt3
	endif
    endif
endif
if ( $?QTDIR ) then
    setenv MANPATH ${MANPATH}:$QTDIR/man
    setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:$QTDIR/lib
endif



# ==================================================================
# END
# The above setups may mess path and append without checking
# if already defined. dropit will "fix" duplicates
if ( -x ${GROUP_DIR}/dropit ) then
    setenv LD_LIBRARY_PATH `${GROUP_DIR}/dropit -p "${LD_LIBRARY_PATH}"`
    setenv PATH  `${GROUP_DIR}/dropit -p "${PATH}"`
endif
# ==================================================================


if ( $?DECHO ) then
    echo "$self :: Final touch ..."
    echo "$self :: LD_LIBRARY_PATH -> $LD_LIBRARY_PATH"
    echo "$self :: PATH            -> $PATH"
endif

# We need this aliases even during BATCH
if (-r $GROUP_DIR/group_aliases.csh) source $GROUP_DIR/group_aliases.csh

# Scratch space ... Also in star_login but defined here in case
# undefined
if ($?SCRATCH == 0) then
    setenv SCRATCH /tmp/$LOGNAME
endif


# User Scratch directory
if ( ! -d $SCRATCH ) then
    /bin/mkdir -p $SCRATCH && /bin/chmod 755 $SCRATCH
endif
if ($ECHO) echo   "Setting up SCRATCH   = $SCRATCH"


# Echo CERN level information
if ($?CERN_ROOT == 1 ) then
    if ($ECHO) echo   "CERNLIB version "$CERN_LEVEL" has been initiated with CERN_ROOT="${CERN_ROOT}
endif

# CLHEP library support
if (! $?CLHEP_BASE_DIR ) then
    setenv CLHEP_BASE_DIR ${OPTSTAR}
endif


# HP Jetprint
if ( -d /opt/hpnp ) then
  if ($ECHO) echo   "Paths set up for HP Jetprint"
  setenv MANPATH `${GROUP_DIR}/dropit -p $MANPATH -p /opt/hpnp/man`
  setenv PATH    `${GROUP_DIR}/dropit -p $PATH  -p /opt/hpnp/bin -p /opt/hpnp/admin`
endif
setenv PATH `${GROUP_DIR}/dropit -p $HOME/bin -p $HOME/bin/.$STAR_HOST_SYS -p $PATH -p $CERN_ROOT/bin -p $CERN_ROOT/mgr .`


# clean-up PATH
if ( $?DECHO ) echo "$self :: Paths cleanup ..."
#if ( -d /cern/../usr.local/lib) setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/cern/../usr.local/lib
if ( -x ${GROUP_DIR}/dropit) then
    if ("$CERN_LEVEL" != "pro") then
	setenv PATH  `${GROUP_DIR}/dropit cern`
	setenv PATH `${GROUP_DIR}/dropit -p ${PATH} -p ${CERN_ROOT}/bin`
    endif
    setenv PATH `${GROUP_DIR}/dropit -p ${OPTSTAR}/bin -p ${PATH}`
    switch ($STAR_SYS)
	case "hp_ux102":
	#  ====================
	setenv SHLIB_PATH `${GROUP_DIR}/dropit -p ${SHLIB_PATH} -p ${OPTSTAR}/lib`
	if ( -d ${OPTSTAR}/lib/mysql ) then
	    setenv SHLIB_PATH `${GROUP_DIR}/dropit -p ${SHLIB_PATH} -p ${OPTSTAR}/lib/mysql`
	endif
	setenv SHLIB_PATH `${GROUP_DIR}/dropit -p "$SHLIB_PATH"`
	breaksw

    default:
	#  ====================
	setenv LD_LIBRARY_PATH `${GROUP_DIR}/dropit -p ${LD_LIBRARY_PATH} -p ${OPTSTAR}/lib`
	if ( -d ${OPTSTAR}/lib/mysql ) then
	    setenv LD_LIBRARY_PATH `${GROUP_DIR}/dropit -p ${LD_LIBRARY_PATH} -p ${OPTSTAR}/lib/mysql`
	endif
	setenv LD_LIBRARY_PATH `${GROUP_DIR}/dropit -p "$LD_LIBRARY_PATH"`
	breaksw
    endsw
    setenv MANPATH `${GROUP_DIR}/dropit -p ${MANPATH}`
    setenv PATH `${GROUP_DIR}/dropit -p ${PATH} GROUPPATH`
endif












#
# Display this message as it is likely the environment is
# screwed up if this happens.
#
if ( "$OPTSTAR" == "") then
    if ($ECHO) then
	    echo ""
	    echo "          ########################################"
	    echo "          ########################################"
	    echo "          ##                                    ##"
	    echo "          ## /!\  OPTSTAR is undefined  /!\     ##"
	    echo "          ##                                    ##"
	    echo "          ## NO local or AFS based installation ##"
	    echo "          ##                                    ##"
	    echo "          ## You have ONLY a PARTIALLY working  ##"
	    echo "          ## STAR environment                   ##"
	    echo "          ##                                    ##"
	    echo "          ########################################"
	    echo "          ########################################"
	    echo ""

	    # turn some echo OFF now so this message is
	    # not cluttered
	    setenv SILENT 1
    endif
endif




if ($ECHO) then
    echo "STAR setup on" `/bin/hostname` "by" `/bin/date` " has been completed"
    echo   "LD_LIBRARY_PATH = $LD_LIBRARY_PATH"
    unset ECHO
endif




#
# Uncomment to get statistics on version used at
# login level.
#
#set date="`date`"
#/bin/cat >> $GROUP_DIR/statistics/star${STAR_VERSION} << EOD
#$USER from $HOST asked for STAR_LEVEL=$STAR_LEVEL / STAR_VERSION=$STAR_VERSION  $date
#EOD
#END


#echo "$STAR"
#echo "$LD_LIBRARY_PATH"
