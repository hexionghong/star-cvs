#!/usr/bin/csh -f
#       $Id: group_env.csh,v 1.80 1999/08/19 22:51:53 fisyak Exp $
#	Purpose:	STAR group csh setup 
#       $Log: group_env.csh,v $
#       Revision 1.80  1999/08/19 22:51:53  fisyak
#       Devorce STAR_LIB and StAF
#
#       Revision 1.79  1999/07/30 21:30:44  fisyak
#       old(SL99b) -> root 2.21(.04)
#
#       Revision 1.78  1999/07/12 22:18:26  wenaus
#       Update GEANT4 env variables
#
#       Revision 1.77  1999/07/12 22:14:42  wenaus
#       Update GEANT4 env variables
#
#       Revision 1.76  1999/07/08 21:50:24  fisyak
#       Add /opt/star/lib in LD_LIBRARY_PATH for all platforms
#
#       Revision 1.75  1999/06/19 02:13:16  fisyak
#       Use CERN_LEVEL and ROOT_LEVEL to synchronize version STAR_LIB and CERNLIB,ROOT
#
#       Revision 1.74  1999/06/08 22:25:59  fisyak
#       Change AFS -> /afs to /usr/afsws
#
#       Revision 1.73  1999/06/03 14:55:39  wenaus
#       '.' to end of path and other uncommitted changes
#
#       Revision 1.72  1999/05/06 15:43:47  fisyak
#       Introduce ROOT 2.22 for SL99f and add egcs-1.1.2
#
#       Revision 1.71  1999/04/13 20:54:34  fisyak
#       Set all Linuxes to redhat51
#
#       Revision 1.70  1999/03/28 18:41:34  fisyak
#       Add cleanup for STAR_LIB on HP
#
#       Revision 1.69  1999/03/27 14:56:48  fisyak
#       ROOT_LEVEL 2.21.08 for SL99e
#
#       Revision 1.68  1999/03/11 19:01:00  fisyak
#       Set ROOT_LEVEL 2.21.07 for SL99c
#
#       Revision 1.67  1999/03/10 20:48:21  fisyak
#       Change default for SL99d from ROOT 2.21/06 to 2.21/07
#
#       Revision 1.66  1999/03/09 02:03:55  fisyak
#       Add root 2.21.06 for SL99d
#
#       Revision 1.65  1999/03/03 23:06:11  fisyak
#       Add CC5 for Sun
#
#       Revision 1.64  1999/03/01 17:27:09  fisyak
#       Set CERNLIB 99 for SL99
#
#       Revision 1.63  1999/02/10 19:19:39  fisyak
#       Set CERN LEVEL 99 for SL99
#
#       Revision 1.62  1999/02/10 16:03:43  wenaus
#       Switch OBJY_ARCH on Linux to linux86  (and a few uncommitted mods by others)
#
#       Revision 1.61  1999/02/06 15:36:51  wenaus
#       Get CLHEP from /opt/star
#
#       Revision 1.60  1999/02/04 02:38:54  fisyak
#       Add SL99b
#
#       Revision 1.59  1999/01/21 13:43:22  wenaus
#       Add ObjectSpace STL to LD_LIBRARY_PATH on Solaris
#
#       Revision 1.58  1998/12/27 17:28:49  fisyak
#       Increase no. of file descriptors on Sun
#
#       Revision 1.57  1998/12/17 14:29:07  fisyak
#       Add parasoft to MANPATH
#
#       Revision 1.56  1998/12/04 17:42:43  fisyak
#       Take out SNIFF for linux -- because of hangup
#
#       Revision 1.55  1998/12/03 01:40:09  fisyak
#       Add user lib to LD_LIBARY_PATH
#
#       Revision 1.54  1998/12/01 01:55:57  fisyak
#       Merge with NT
#
#       Revision 1.53  1998/10/14 14:53:15  fisyak
#       Fix ROOT version
#
#       Revision 1.52  1998/10/12 00:50:03  fisyak
#       ROOT 2.12 for SL98j
#
#       Revision 1.51  1998/10/08 21:13:32  fisyak
#       Add SILENT option
#
#       Revision 1.50  1998/10/05 15:20:39  fisyak
#       Add BFARCH for sgi's
#
#       Revision 1.49  1998/09/20 01:54:29  fisyak
#       Add path to lsf
#
#       Revision 1.48  1998/09/18 16:30:47  fisyak
#       Replace afs path
#
#       Revision 1.47  1998/09/10 02:00:24  fisyak
#       Add protection for undefined STAR_SYS
#
#       Revision 1.46  1998/08/27 01:29:02  fisyak
#       Add  root 2/11 for development version (SL98g)
#
#       Revision 1.45  1998/08/23 18:47:02  fisyak
#       No debug name of libraries is LIB
#
#       Revision 1.44  1998/08/11 13:41:20  fisyak
#       remove user lib directory from LD_LIBRARY_PATH
#
#       Revision 1.43  1998/08/10 21:32:30  fisyak
#       add clean up of LD_LIBRARY_PATH
#
#       Revision 1.42  1998/08/05 13:07:17  wenaus
#       SNiFF+ setup
#
#       Revision 1.41  1998/07/31 19:40:36  fisyak
#       Add STAR_PROD to versioning SL
#
#       Revision 1.40  1998/07/27 20:23:58  fisyak
#       Add sgi as unsupport OBJY platform
#
#       Revision 1.39  1998/07/26 06:50:41  wenaus
#       Fix Objy paths for rsun00
#
#       Revision 1.38  1998/07/24 15:17:36  fisyak
#       Add redhat50
#
#       Revision 1.37  1998/07/23 14:28:48  wenaus
#       re-insert new Objy setup
#
#       Revision 1.36  1998/07/22 21:41:31  fisyak
#       Move STAR_SYS  up
#
#       Revision 1.35  1998/07/22 21:29:20  fisyak
#       Add SL98f
#
#       Revision 1.32  1998/07/19 01:30:40  fisyak
#       /usr/bin/ls -> /bin/ls
#
#       Revision 1.31  1998/07/19 01:26:15  fisyak
#       replace ls -> /usr/bin/ls to protect from user aliases
#
#       Revision 1.30  1998/07/12 23:14:03  fisyak
#       fix bug in LD_LIBRARY_PATH
#
#       Revision 1.29  1998/07/11 00:59:14  fisyak
#       add cern/LEVEL/bin to path
#
#       Revision 1.28  1998/07/10 21:18:40  fisyak
#       Fix NODEBUG flug for SL98c and SL98e
#
#       Revision 1.27  1998/07/10 14:06:42  fisyak
#       Keep for SL98a and SL98b the old version convention
#
#       Revision 1.26  1998/07/10 13:49:28  fisyak
#       Add cleaning of STAR_PATH
#
#       Revision 1.25  1998/07/10 12:53:32  fisyak
#       Use STAR_VERSION instead STAR_LEVEL for STAR top directory, to be in sunc. with starver script
#
#       Revision 1.24  1998/07/07 18:25:49  fisyak
#       Add STAR_PARAMS to STAR environment variables
#
#       Revision 1.23  1998/07/04 18:49:35  fisyak
#       Add STAF_LIB to LD_LIBRARYN32_PATH for sgi_64
#
#       Revision 1.22  1998/07/03 20:04:04  fisyak
#       Add MANPATH to DQS386
#
#       Revision 1.21  1998/07/01 23:28:08  fisyak
#       Add non debug version
#
#       Revision 1.20  1998/06/30 14:37:22  didenko
#       Add hp
#
#       Revision 1.19  1998/06/24 20:18:38  wenaus
#       BaBar setup only in conjunction with Objy setup
#
#       Revision 1.18  1998/06/24 18:35:52  wenaus
#       Objy setup must always be run
#
#       Revision 1.17  1998/06/24 18:01:23  wenaus
#       Add env variables for BaBar and G4 software
#
#       Revision 1.16  1998/06/20 01:05:23  fisyak
#       Add STAF_LIB in LD_LIBRARY_PATH
#
#       Revision 1.15  1998/06/14 19:48:00  fisyak
#       Add crean up of ROOT path
#
#       Revision 1.14  1998/06/12 15:45:32  fisyak
#       Resotore dev environment
#
#       Revision 1.13  1998/06/12 13:11:57  fisyak
#       Add version statistics
#
#       Revision 1.12  1998/06/11 20:29:49  wenaus
#       Set STAR_DB only if not yet set to allow offsite overrides
#
#       Revision 1.11  1998/06/11 20:23:13  wenaus
#       Add STAR_DB env variable
#
#       Revision 1.10  1998/05/14 19:04:25  fisyak
#       clean LD_LIBRARY_PATH
#
#       Revision 1.9  1998/05/12 12:14:20  fisyak
#       Clean up
#
#       Revision 1.8  1998/05/01 22:55:43  fisyak
#       add dropit for PATH
#
#       Revision 1.7  1998/04/24 17:08:52  fisyak
#       Set coredupsize=0
#
#       Revision 1.6  1998/04/04 14:47:22  fisyak
#       Add clean PATH
#
#       Revision 1.5  1998/03/26 16:41:34  fisyak
#       Add STAR_LEVELS
#
#       Revision 1.4  1998/03/24 00:04:09  fisyak
#       fix PATH
#
#       Revision 1.3  1998/03/23 02:29:15  fisyak
#       Fix group start-up
#
#	Author:		Y.Fisyak     BNL
#	Date:		27 Feb. 1998
#	Modified:
#     3 Mar 98  T. Wenaus  HP Jetprint added (for sol)
# 
#	STAR software group	1998
#
set ECHO = 1; if ($?STAR == 1) set ECHO = 0
if ($?SILENT == 1) set ECHO = 0;
if ($ECHO) then
  cat /afs/rhic/star/group/small-logo 
endif
setenv WWW_HOME http://www.star.bnl.gov/
                                             if ($ECHO) echo   "Setting up WWW_HOME  = $WWW_HOME"
setenv AFS       /usr/afsws
setenv AFS_RHIC  /afs/rhic
setenv STAR_ROOT ${AFS_RHIC}/star;           if ($ECHO) echo   "Setting up STAR_ROOT = ${STAR_ROOT}"         
# Defined by HEPiX
if ( ! $?GROUP_DIR )  setenv GROUP_DIR ${STAR_ROOT}/group
# Defined in CORE
if ( ! $?GROUP_PATH ) setenv GROUP_PATH ${STAR_ROOT}/group
setenv GROUPPATH  $GROUP_PATH
setenv STAR_PATH ${STAR_ROOT}/packages;      if ($ECHO) echo   "Setting up STAR_PATH = ${STAR_PATH}"
if ($?STAR_LEVEL == 0) setenv STAR_LEVEL pro
if ($STAR_LEVEL  == "old" || $STAR_LEVEL  == "pro" || $STAR_LEVEL  == "new" || $STAR_LEVEL  == "dev" || $STAR_LEVEL  == ".dev") then
  setenv STAR_VERSION `/bin/ls -ld $STAR_PATH/${STAR_LEVEL} |cut -f2 -d">"`  
else
  setenv STAR_VERSION ${STAR_LEVEL}
endif
if ($?STAF_LEVEL == 0) setenv STAF_LEVEL pro
if ($STAF_LEVEL  == "old" || $STAF_LEVEL  == "pro" || $STAF_LEVEL  == "new" || $STAF_LEVEL  == "dev" || $STAF_LEVEL  == ".dev") then
  setenv STAF_VERSION `/bin/ls -ld $STAR_PATH/StAF/${STAF_LEVEL} |cut -f2 -d">"`  
else
  setenv STAF_VERSION ${STAF_LEVEL}
endif
source ${GROUP_DIR}/STAR_SYS; 
setenv STAR $STAR_PATH/${STAR_VERSION} ;        if ($ECHO) echo   "Setting up STAR      = ${STAR}"
setenv STAF $STAR_PATH/StAF/${STAF_VERSION} ;   if ($ECHO) echo   "Setting up STAF      = ${STAF}"
setenv STAF_LIB  $STAF/.${STAR_HOST_SYS}/lib  ; if ($ECHO) echo   "Setting up STAF_LIB  = ${STAF_LIB}"
setenv STAF_BIN  $STAF/.${STAR_HOST_SYS}/bin  ; if ($ECHO) echo   "Setting up STAF_BIN  = ${STAF_BIN}"
if ($?NODEBUG == 0) then
 setenv STAR_LIB  $STAR/.${STAR_HOST_SYS}/lib;  if ($ECHO) echo   "Setting up STAR_LIB  = ${STAR_LIB}"
 setenv MINE_LIB        .${STAR_HOST_SYS}/lib;
else
  setenv STAR_LIB  $STAR/.${STAR_HOST_SYS}/LIB; if ($ECHO) echo   "Setting up STAR_LIB  = ${STAR_LIB}"
  setenv MINE_LIB        .${STAR_HOST_SYS}/LIB;
endif
setenv STAR_BIN  $STAR/.${STAR_HOST_SYS}/bin  ; if ($ECHO) echo   "Setting up STAR_BIN  = ${STAR_BIN}"
setenv STAR_MGR $STAR/mgr
setenv STAR_PAMS $STAR/pams;                 if ($ECHO) echo   "Setting up STAR_PAMS = ${STAR_PAMS}"
setenv STAR_DATA ${STAR_ROOT}/data;          if ($ECHO) echo   "Setting up STAR_DATA = ${STAR_DATA}"
if ( ! $?STAR_DB ) setenv STAR_DB /star/sol/db;                 if ($ECHO) echo   "Setting up STAR_DB   = ${STAR_DB}"
setenv STAR_PARAMS ${STAR}/params;      if ($ECHO) echo   "Setting up STAR_PARAMS= ${STAR_PARAMS}"
setenv STAR_CALIB ${STAR_ROOT}/calib;   if ($ECHO) echo   "Setting up STAR_CALIB= ${STAR_CALIB}"
setenv STAR_PROD   $STAR/prod;          if ($ECHO) echo   "Setting up STAR_PROD = ${STAR_PROD}"
setenv CVSROOT   $STAR_PATH/repository; if ($ECHO) echo   "Setting up CVSROOT   = ${CVSROOT}"

if (-f $STAR/mgr/ROOT_LEVEL && -f $STAR/mgr/CERN_LEVEL) then
  setenv ROOT_LEVEL `cat $STAR/mgr/ROOT_LEVEL`
  setenv CERN_LEVEL `cat $STAR/mgr/CERN_LEVEL`
else
switch ( $STAR_VERSION )

  case SL98l: setenv ROOT_LEVEL 2.20
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

  default: setenv ROOT_LEVEL 2.13
endsw
endif
if ($ECHO) echo   "Setting up ROOT_LEVEL= ${ROOT_LEVEL}"
if ($?TEXINPUTS == 0) then
  setenv TEXINPUTS "${GROUP_DIR}/latex/styles"
else
  setenv TEXINPUTS "${TEXINPUTS}:${GROUP_DIR}/latex/styles"
endif
setenv GROUPPATH "${GROUP_DIR}:${STAR_MGR}:${STAR_BIN}:${STAF_BIN}"
if ( -x /afs/rhic/star/group/dropit) then
# clean-up PATH
  setenv TEXINPUTS `/afs/rhic/star/group/dropit -p ${TEXINPUTS}`
  setenv MANPATH `/afs/rhic/star/group/dropit -p ${MANPATH}`
  setenv PATH `/afs/rhic/star/group/dropit -p ${PATH} GROUPPATH`
  setenv PATH `/afs/rhic/star/group/dropit -p ${PATH} $STAR_PATH`
  if (${?LD_LIBRARY_PATH}) setenv LD_LIBRARY_PATH `/afs/rhic/star/group/dropit -p ${LD_LIBRARY_PATH} $STAR_PATH`
endif
setenv PATH "/usr/afsws/bin:/usr/afsws/etc:/opt/star/bin:/usr/sue/bin:/usr/local/bin:${GROUP_DIR}:${STAR_MGR}:${STAR_BIN}:${STAF_BIN}:${PATH}"
## Put mysql on path if available
if ( -d /usr/local/mysql/bin) then
  setenv PATH "${PATH}:/usr/local/mysql/bin"
endif
if ($?MANPATH == 1) then
  setenv MANPATH ${MANPATH}:${STAR_PATH}/man
else
  setenv MANPATH ${STAR_PATH}/man
endif
setenv PARASOFT /afs/rhic/star/packages/parasoft
setenv MANPATH ${MANPATH}:{$PARASOFT}/man
     setenv OBJY_ARCH  ""
switch ($STAR_SYS)
    case "rs_aix*":
#  ====================
	set path = ($path $PARASOFT/bin.aix4) 
        setenv MANPATH {$MANPATH}:/usr/share/man
    breaksw
    case "alpha_osf32c":
#  ====================
    breaksw
    case "hp700_ux90":
#  ====================
    breaksw
    case "hp_ux102":
#  ====================
      if (! ${?SHLIB_PATH}) setenv SHLIB_PATH
      if ( -x /afs/rhic/star/group/dropit) setenv SHLIB_PATH `/afs/rhic/star/group/dropit -p ${SHLIB_PATH} $STAR_PATH`
      setenv SHLIB_PATH ${SHLIB_PATH}:${MINE_LIB}:${STAR_LIB}:${STAF_LIB}
      setenv LD_LIBRARY_PATH ${SHLIB_PATH}
      setenv BFARCH hp_ux102
      limit coredumpsize 0
    breaksw
    case "sgi_5*":
#  ====================
	set path = ($path $PARASOFT/bin.sgi5)
        if (! ${?LD_LIBRARY_PATH}) setenv LD_LIBRARY_PATH 
	setenv LD_LIBRARY_PATH "${PARASOFT}/lib.sgi5:${MINE_LIB}:${STAR_LIB}:${STAF_LIB}:${LD_LIBRARY_PATH}"
        limit coredumpsize 0
     setenv BFARCH sgi_53
    breaksw
    case "sgi_62":
 #  ====================
	setenv CERN /afs/rhic/asis/sgi_62/cern
	setenv CERN_ROOT $CERN/pro
	if (! ${?LD_LIBRARY_PATH}) setenv LD_LIBRARY_PATH 
	setenv LD_LIBRARY_PATH "${MINE_LIB}:${STAR_LIB}:${STAF_LIB}:${LD_LIBRARY_PATH}"
        limit coredumpsize 0
     setenv BFARCH sgi_53
    breaksw
   case "sgi_64":
#  ====================
        setenv CERN_LEVEL pro
        setenv CERN_ROOT  /cern/pro
        if (! ${?LD_LIBRARYN32_PATH}) setenv LD_LIBRARYN32_PATH 
	setenv LD_LIBRARYN32_PATH "${MINE_LIB}:${STAR_LIB}:${STAF_LIB}:${LD_LIBRARYN32_PATH}"
        limit coredumpsize 0
     setenv BFARCH sgi_64
    breaksw
    case "i386_*":
#  ====================
# make sure that afws in the path
     if (! -d /usr/afsws/bin) set path = ($path /afs/rhic/i386_redhat50/usr/afsws/bin)
     if ( -d /usr/pgi ) then
       setenv PGI /usr/pgi
       set path = ( $PGI/linux86/bin $path)
       setenv MANPATH "$MANPATH":$PGI/man
       setenv LM_LICENSE_FILE $PGI/license.dat
       alias pgman 'man -M $PGI/man'
       if ("`echo $STAR_VERSION | cut -c3-4`" != "99") then
           setenv CERN_LEVEL pgf98
       endif
     endif
     setenv CERN_ROOT  $CERN/$CERN_LEVEL
     set path = ($path  /usr/local/bin/ddd)
#                                            /usr/local/DQS318/bin )
#     if ( -x /usr/local/DQS32/bin/qstat32) then
#       set path = ($path /usr/local/DQS32/bin )
#       setenv  MANPATH "$MANPATH":/usr/local/DQS32/man
#     endif
#    set path = ($path  /usr/local/bin/ddd /usr/local/DQS318/bin )
     set path = ($path $PARASOFT/bin.linux)
     if (! ${?LD_LIBRARY_PATH}) setenv LD_LIBRARY_PATH 
     setenv LD_LIBRARY_PATH "/usr/lib:${PARASOFT}/lib.linux:/usr/local/lib:${MINE_LIB}:${STAR_LIB}:${STAF_LIB}:${LD_LIBRARY_PATH}"
     limit coredump 0
     setenv BFARCH Linux2
     setenv OBJY_ARCH linux86
    breaksw
    case "sun4*":
#  ====================
      if (! ${?LD_LIBRARY_PATH}) setenv LD_LIBRARY_PATH
      setenv LD_LIBRARY_PATH "/opt/SUNWspro/lib:/usr/openwin/lib:/usr/dt/lib:/usr/local/lib:${PARASOFT}/lib.solaris:/afs/rhic/star/packages/ObjectSpace/2.0m/lib:${MINE_LIB}:${STAR_LIB}:${STAF_LIB}:${LD_LIBRARY_PATH}"
	set path = ($path $PARASOFT/bin.solaris)
      setenv BFARCH SunOS5
      if ("${STAR_HOST_SYS}" == "sun4x_56_CC5") setenv BFARCH SunOS5_CC5
      setenv OBJY_ARCH solaris4
      limit coredump 0
      unlimit descriptors
      if ("${STAR_HOST_SYS}" == "sun4x_56_CC5" || "${STAR_HOST_SYS}" == "sun4x_56_CC5C") then
          setenv LD_LIBRARY_PATH "/opt/WS5.0/lib:/opt/WS5.0/SC5.0/lib:/usr/openwin/lib:/usr/dt/lib:/usr/local/lib:${PARASOFT}/lib.solaris:/afs/rhic/star/packages/ObjectSpace/2.0m/lib:${MINE_LIB}:${STAR_LIB}:${STAF_LIB}:${LD_LIBRARY_PATH}"
          setenv PATH "/opt/WS5.0/bin:${PATH}"
        endif
    breaksw 
    case "sunx86_55":
#  ====================
        if (! ${?LD_LIBRARY_PATH}) setenv LD_LIBRARY_PATH
        setenv LD_LIBRARY_PATH "${MINE_LIB}:${STAR_LIB}:${STAF_LIB}:${LD_LIBRARY_PATH}"
        limit coredump 0
    breaksw
    default:
#  ====================
    breaksw
endsw
if ( -e /usr/ccs/bin/ld ) set path = ( $path /usr/ccs/bin /usr/ccs/lib )
if ( -d /usr/local/lsf/bin ) then
  if ( -x /afs/rhic/star/group/dropit) setenv PATH `/afs/rhic/star/group/dropit lsf`
  setenv LSF_ENVDIR /usr/local/lsf/mnt/conf
  set path=(/usr/local/lsf/bin $path)
  setenv MANPATH {$MANPATH}:/usr/local/lsf/mnt/man
endif
# We need this aliases even during BATCH
if (-r $GROUP_DIR/group_aliases.csh) source $GROUP_DIR/group_aliases.csh
#
if ($?SCRATCH == 0) then
if ( -w /scr20 ) then
        setenv SCRATCH /scr20/$LOGNAME
else if ( -w /scr21 ) then
        setenv SCRATCH /scr21/$LOGNAME
else if ( -w /scr22 ) then
        setenv SCRATCH /scr22/$LOGNAME
else if ( -w /scratch ) then
        setenv SCRATCH /scratch/$LOGNAME
else 
#	echo No scratch directory available. Using /tmp/$USER ...
        setenv SCRATCH /tmp/$LOGNAME
endif
 
if ( ! -d $SCRATCH ) then
        mkdir $SCRATCH
        chmod 755 $SCRATCH
endif
if ($ECHO) echo   "Setting up SCRATCH   = $SCRATCH"
endif
if ($ECHO) echo   "STAR library version "$STAR_VERSION" has been initiated with `which staf`"
if ($?CERN_ROOT == 1 ) then
if ($ECHO) echo   "CERNLIB version "$CERN_LEVEL" has been initiated with CERN_ROOT="${CERN_ROOT}
endif
# root
if ( -f $GROUP_DIR/rootenv.csh) then
  source $GROUP_DIR/rootenv.csh
endif

# Objectivity
if (`uname -s` == "SunOS" && `hostname` != "rcf.rhic.bnl.gov" && ! ${?OBJY_HOME} ) source $GROUP_DIR/ObjySetup.csh

# SNiFF+
setenv G4SYSTEM
switch ($STAR_SYS)
    case "sun4*":
#     ====================
      setenv SNIFF_DIR /star/sol/packages/sniff
      set path = ( $path $SNIFF_DIR/bin )
      setenv G4SYSTEM SUN-CC
      breaksw 
    case "i386_*":
#     ====================
      setenv SNIFF_DIR /star/sol/packages/sniff
#      set path = ( $path $SNIFF_DIR/bin )
      setenv G4SYSTEM Linux-g++
      breaksw
    default:
#     ====================
      breaksw
endsw

# Geant4 testing
setenv RWBASE         /afs/rhic/usatlas/software/geant4/rogue/$G4SYSTEM
setenv CLHEP_BASE_DIR /afs/rhic/usatlas/software/geant4/CLHEP/$G4SYSTEM/pro

# HP Jetprint
if ( -d /opt/hpnp ) then
  if ($ECHO) echo   "Paths set up for HP Jetprint"
  setenv MANPATH "$MANPATH":/opt/hpnp/man
# set PATH = ( $PATH':'/opt/hpnp/bin':'/opt/hpnp/admin )
  set path = ( $path /opt/hpnp/bin /opt/hpnp/admin )
endif
set path = ($HOME/bin $HOME/bin/.$STAR_SYS $path $CERN_ROOT/bin $CERN_ROOT/mgr .)
if ( -x /afs/rhic/star/group/dropit) then
# clean-up PATH
switch ($STAR_SYS)
    case "hp_ux102":
#  ====================
  setenv SHLIB_PATH "${SHLIB_PATH}:/opt/star/lib"
  setenv SHLIB_PATH `/afs/rhic/star/group/dropit -p "$SHLIB_PATH"`
    breaksw
    case "sgi_64":
#  ====================
  setenv LD_LIBRARYN32_PATH "${LD_LIBRARYN32_PATH}:/opt/star/lib"
  setenv LD_LIBRARYN32_PATH `/afs/rhic/star/group/dropit -p "$LD_LIBRARYN32_PATH"`
    breaksw
    default:
#  ====================
  setenv LD_LIBRARY_PATH "${LD_LIBRARY_PATH}:/opt/star/lib"
  setenv LD_LIBRARY_PATH `/afs/rhic/star/group/dropit -p "$LD_LIBRARY_PATH"`
    breaksw
endsw
  setenv MANPATH `/afs/rhic/star/group/dropit -p ${MANPATH}`
  setenv PATH `/afs/rhic/star/group/dropit -p ${PATH} GROUPPATH`
endif
if ($ECHO) then
echo "STAR setup on" `hostname` "by" `date` " has been completed"
unset ECHO
endif
set date="`date`"
cat >> $GROUP_DIR/statistics/star${STAR_VERSION} << EOD
$USER from $HOST asked for STAR_LEVEL=$STAR_LEVEL / STAR_VERSION=$STAR_VERSION  $date
EOD
#END



