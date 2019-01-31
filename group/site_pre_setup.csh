#
# This file is used for local setups / env defines
# which needs to be done prior to executing the
# group_env script
#
# Developpers can still re-define XROOTDSYS in their cshrc
# to switch to a beta version for testing purposes.
#

set self="site_pre_setup"

# This should not happen
if( ! $?AFS_RHIC)   setenv AFS_RHIC  /afs/rhic.bnl.gov

# Switch to CVMFS if defined
if ( ! $?STAR_BASE_PATH ) setenv STAR_BASE_PATH ""
if ( $?USE_CVMFS ) then
    if ( -d "/cvmfs/star.sdcc.bnl.gov") then
	setenv STARCVMFS /cvmfs/star.sdcc.bnl.gov
	setenv STAR_BASE_PATH ${STARCVMFS}

	# Assume Linux only? but this logic is in STAR_SYS
	source ${GROUP_DIR}/STAR_SYS
	if ( $?DECHO ) echo "$self :: setting STAR_SYS to ${STAR_SYS}"
    endif
endif



# Define DOMAINNAME if does not exists
if( ! $?DOMAINNAME) then
    if ( -x "/bin/domainname" ) then
       setenv DOMAINNAME `/bin/domainname`
    else
       # Be aware that NIS/YP could be disabled 
       setenv DOMAINNAME "(none)"
    endif
    if ( "$DOMAINNAME" == "(none)") then 
       setenv DOMAINNAME `/bin/hostname | /bin/sed 's/^[^\.]*\.//'`
    endif
endif



switch ($DOMAINNAME)
   # BNL
   case "rhic.bnl.gov":
   case "rcf.bnl.gov":
   case "usatlas.bnl.gov":
    # This detects everything
    if ( ! $?XROOTDSYS ) then
	# in AFS land?
	if ( $STAR_BASE_PATH != "" ) then
	    set xrootd=${STAR_BASE_PATH}/star/ROOT/Xrootd/prod
	else
	    set xrootd=${AFS_RHIC}/star/ROOT/Xrootd/prod
	endif

	if ( -d $xrootd ) then
	    setenv XROOTDSYS $xrootd
	endif
    endif

    # We have it valid for Linux only
    if ( $STAR_BASE_PATH != "" ) then
	set PP=${STAR_BASE_PATH}/Grid/OSG/WNC
    else
	set PP=${AFS_RHIC}/star/Grid/OSG/WNC
    endif
    if ( -d $PP ) then
	if ( `/bin/uname` == "Linux") then
	    setenv WNOSG $PP
	endif
    endif

    # users coming with this defined would mess perl up (RT  #2307)
    if ( $?LC_CTYPE ) then
    	unsetenv LC_CTYPE
    endif
    breaksw



  default:
    # DO NOTHING
endsw     





# Experimental
if ( -x "/usr/bin/scl" ) then
    set dts=`/usr/bin/scl -l | /usr/bin/tail -1`
    if ( "$dts" != "") then
	set test=`echo $PATH | grep $dts`
	if ( "$test" == "") then
	    if ( -d /opt/rh/$dts/root/usr ) then
		if ( $?DECHO ) then
		    echo "$self :: Devtoolset version $dts is available"
		endif
		# ---> that's one possibility but requires more massage
		#      PATH wise
		#/usr/bin/scl enable $dts '/bin/tcsh -l'
		#logout
		#<--- 
		#if ( ! $?USE_GCC_DIR ) then
		#    if ( $?DECHO ) echo "$self :: setting GCC DIR to $dts"
		#    setenv USE_GCC_DIR /opt/rh/$dts/root/usr 
		#endif
	    endif
	endif
    else
	if ( $?DECHO ) echo "We are using devtoolset $dts"
    endif
endif
