#
# This file is used to setup site specific
# environment variables (like printers, 
# default web pages, proxies etc ...)
#
#  J.Lauret Aug 31  2006
#  Revised J.L Sept 9th 2008
#  Revised J.L Feb  9th 2012
#  Revised J.L Dec 2018 to get tokens if K-ticket
#
#

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
    setenv WWW_HOME     "http://www.rhic.bnl.gov"
    setenv NNTPSERVER   "news.bnl.gov"
    #setenv http_proxy   "http://proxy.sec.bnl.local:3128/"
    #setenv https_proxy  "http://proxy.sec.bnl.local:3128/"
    #setenv ftp_proxy    "http://proxy.sec.bnl.local:3128/"
    # try to get an AFS token out of a Kerberos one
    if ( -x /usr/bin/klist && -x /usr/bin/aklog ) then
	set test=`/usr/bin/klist -A | $GREP  afs`
	if ( "$test" != "") then
	    /usr/bin/aklog
	endif
    endif
    breaksw

  default:
    # DO NOTHING
endsw     

case 

# generic environment variables
# Those variables are part of the StDbLib/StDbServiceBroker.cxx
if ( $USER == "starreco") then
    setenv STAR_DEBUG_DB_RETRIES_ADMINS  "jlauret@rcf.rhic.bnl.gov,arkhipkin@rcf.rhic.bnl.gov"
    setenv STAR_DEBUG_DB_RETRIES_SECONDS 1800                                  # 30 mnts and default
endif


# SL7 stop gap to avoid  - JL 2017
#  "Couldn't connect to accessibility bus: Failed to connect to socke"
# dbus socket creation trick did not fix it (may require A11y layer)
#
setenv NO_AT_BRIDGE 1
