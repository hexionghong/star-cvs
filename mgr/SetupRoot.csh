#! /usr/local/bin/tcsh -x
setenv MYSQL /opt/star
setenv ROOTBUILD debug
switch ( $STAR_HOST_SYS)  
    case i386*:
	setenv ARCH  linuxegcs
    breaksw
    case sun4x_56_CC5:
	setenv ARCH solarisCC5
    breaksw
    case sun4x_56:
	setenv ARCH solaris
    breaksw
    case hp_ux102:
	setenv ARCH hpuxacc
	setenv XPM $ROOTSYS/lib
    breaksw
    default:
     exit 1
endsw
switch ( $STAR_HOST_SYS)  
    case hp_ux102:
    unsetenv MYSQL
./configure $ARCH \
    --prefix=$ROOTSYS \
    --datadir=$ROOTSYS \
    --etcdir=$ROOTSYS/etc \
    --cintincdir=$ROOTSYS/cint \
    --with-ttf-incdir=/usr/local/include \
    --with-ttf-libdir=/usr/local/lib \
    --with-cern-libdir=/cern/pro/lib \
    --with-afs=/usr/awsfs/lib 
    breaksw
    case sun4x_56_CC5:
./configure $ARCH \
    --prefix=$ROOTSYS \
    --datadir=$ROOTSYS \
    --etcdir=$ROOTSYS/etc \
    --cintincdir=$ROOTSYS/cint \
    --with-ttf-incdir=/usr/local/include \
    --with-ttf-libdir=/usr/local/lib \
    --with-cern-libdir=/cern/pro/lib \
    --with-rfio=/usr/local/lib/libshift.a \
    --with-afs=/usr/awsfs/lib
    breaksw
    default:
./configure $ARCH \
    --prefix=$ROOTSYS \
    --datadir=$ROOTSYS \
    --etcdir=$ROOTSYS/etc \
    --cintincdir=$ROOTSYS/cint \
    --with-ttf-incdir=/usr/local/include \
    --with-ttf-libdir=/usr/local/lib \
    --with-cern-libdir=/cern/pro/lib \
    --with-rfio=/usr/local/lib/libshift.a \
    --with-thread=/usr/lib/libpthread.so \
    --with-afs=/usr/awsfs/lib \
    --with-pythia6=/cern/pro/lib/libpythia.a
endsw
