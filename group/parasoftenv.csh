#! /usr/local/bin/tcsh -f
switch ($STAR_SYS)
    case "i386_redhat6*":
    case "i386_linux2*":
	#setenv PARASOFT /usr/local/app/parasoft
	setenv PARASOFT /afs/rhic/app/insure-5.2
	setenv LD_LIBRARY_PATH `${GROUP_DIR}/dropit -p ${LD_LIBRARY_PATH} -p {$PARASOFT}/lib.linux2`
	if ( -x /afs/rhic/star/group/dropit) setenv PATH `dropit parasoft`
	set path = ($path $PARASOFT/bin.linux2)
	setenv MANPATH `${GROUP_DIR}/dropit -p ${MANPATH} -p {$PARASOFT}/man`
	breaksw
    case "i386_*":
	setenv PARASOFT /afs/rhic/i386_linux22/app/parasoft
	setenv LD_LIBRARY_PATH `${GROUP_DIR}/dropit -p ${LD_LIBRARY_PATH} -p {$PARASOFT}/lib.linux`
	if ( -x /afs/rhic/star/group/dropit) setenv PATH `dropit parasoft`
	set path = ($path $PARASOFT/bin.linux)
	setenv MANPATH `${GROUP_DIR}/dropit -p ${MANPATH} -p {$PARASOFT}/man`
	breaksw
    case "sun4*":
	setenv PARASOFT /afs/rhic/sun4x_56/app/parasoft
	setenv LD_LIBRARY_PATH `${GROUP_DIR}/dropit -p ${LD_LIBRARY_PATH} -p {$PARASOFT}/lib.solaris`
	if ( -x /afs/rhic/star/group/dropit) setenv PATH `dropit parasoft`
	set path = ($path $PARASOFT/bin.solaris)
	setenv MANPATH `${GROUP_DIR}/dropit -p ${MANPATH} -p {$PARASOFT}/man`
	breaksw 
    default:
	    echo "No parasoft setup  defined for platform $STAR_SYS"
    breaksw
endsw
#___________
