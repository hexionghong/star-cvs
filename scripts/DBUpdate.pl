#!/opt/star/bin/perl -w

use lib "/afs/rhic.bnl.gov/star/packages/scripts";
use FileCatalog;
use Date::Manip;

if ($#ARGV == -1){

    print qq~

 Syntax is
  % DBUpdate.pl [options] BasePath [fileExtension] [RelPathOverwrite] [Substitute]
     [user] [password]

 Options are
   -o outputFile      redirect output to file outputFile
   -k subpath         strip subpath before comparing to HPSS, then clone
                      using full path
   -l                 consider soft-links in path
   -nocache           do not use caching


 Purpose
   This script scans a disk given as the first argument
   checks all files and update the database with a file
   location new entry if it finds the same entry as
   storage = HPSS.

   It uses the clone_location() method to update the
   database with that entry and is a very good example
   of how to do this ...

   This is REALLY a spider ... It is used to post-scan
   disk and catch entries which may be missing.

   Only clones (if there is no similar entries in the db,
   it will not add it).


 Arguments are
   ARGV0   the base path of a disk to scan (default /star/data06)
   ARGV1   the filetype (default .MuDst.root ). If null, it will
           search for all files
   ARGV2   this scripts limits it to a sub-directory "reco" starting
           from ARGV0. Use this argument to overwrite.
   ARGV3   A base path substitution for find the entry in HPSS
           Default is /home/starreco .

   ARGV4   a user name (default FC_admin)
   ARGV5   a password (default will be to use the
           get_connection() method as a guess try)


 Examples
  % DBUpdate.pl /star/data27
  % DBUpdate.pl /star/data27 ""
  % DBUpdate.pl /star/data03 .daq daq /home/starsink/raw
  % DBUpdate.pl /home/starlib/home/starreco -k /home/starlib -l

~;
    exit;
}

# BEWARE :
#  (1) $SITE and $HPSSD are global variables
#  (2) There is an hidden logic based on $path !~ /\/star\/data/
#      to recognized if the storage is local or NFS.

$SITE  = "BNL";
$HPSSD = "/home/starreco";
$CHKDIR= "/afs/rhic.bnl.gov/star/doc/www/html/tmp/pub/Spider";
$SELF  = "DBUpdate";
$LOUT  = 0;
$FLNM  = "";

# Those default should nt be changed here but via
# command line options
$SCAND  = "/star/data06";
$USER   = "";
$PASSWD = "";
$SUBPATH= "";
$SUB    = "reco";
$DOSL   = 0;
$DOCACHE= 1;

# Argument pick-up
$kk    = 0;
$FO    = STDERR;
$|     = 1;

for ($i=0 ; $i <= $#ARGV ; $i++){
    # Support "-XXX" options
    if ($ARGV[$i] eq "-o"){
	$FLNM = $ARGV[$i+1];

	# Be sure we check on the tmp file and do 
	# not have process clashing.
	if ( -e "$FLNM.tmp"){
	    my(@items)=stat("$FLNM.tmp");
	    my($deltatime)= time() - $items[9];
	    if ( $deltatime < 900){
                # this file is too recent i.e. less than 10 mnts
                open(FO,">>$FLNM");
                print FO 
		    "$FLNM.tmp detected and more recent than expected. ".
		    "Process $$ exit.\n";
                close(FO);
                exit;
	    }
	}
	if ( open(FO,">$FLNM.tmp") ){
	    $i++;
	    $FO = FO;
	}

    } elsif ($ARGV[$i] eq "-nocache"){
	$DOCACHE= 0;

    } elsif ($ARGV[$i] eq "-k"){
	$SUBPATH = $ARGV[++$i];

    } elsif ($ARGV[$i] eq "-l"){
	$DOSL    = 1;

    } else {
	# ... as well as previous syntax
	$kk++;
	$SCAND = $ARGV[$i] if ( $kk == 1);
	$FTYPE = $ARGV[$i] if ( $kk == 2);
	$SUB   = $ARGV[$i] if ( $kk == 3);
	$HPSSD = $ARGV[$i] if ( $kk == 4);
	$USER  = $ARGV[$i] if ( $kk == 5);
	$PASSWD= $ARGV[$i] if ( $kk == 6);
    }
}


# Get shorten string for path or base path for HPSS regexp
#@items  = split("/",$SCAND);
#$SCANDS = "/".$items[1]."/".$items[2];



#@ALL =( "$SCAND/$SUB/FPDXmas/FullField/P02ge/2002/013/st_physics_3013016_raw_0018.MuDst.root",
#	"$SCAND/$SUB/FPDXmas/FullField/P02ge/2002/013/st_physics_3013012_raw_0008.MuDst.root");


$DOIT  = ($#ALL == -1);


if ( ! defined($FTYPE) ){  $FTYPE = ".MuDst.root";}


if( $DOIT && -e "$SCAND/$SUB"){
    if ($FTYPE ne ""){
	print "Searching for all files like '*$FTYPE' in $SCAND/$SUB  ...\n";
	if ( $DOSL){
	    @ALL   = `/usr/bin/find $SCAND/$SUB -type l -name '*$FTYPE'`;
	    print "Found ".($#ALL+1)." links to add (x2)\n";
	} else {
	    @ALL   = `/usr/bin/find $SCAND/$SUB -type f -name '*$FTYPE'`;
	    print "Found ".($#ALL+1)." files to add (x2)\n";
	}

    } else {
	print "Searching for all files in $SCAND/$SUB ...\n";
	if ($DOSL){
	    @ALL   = `/usr/bin/find $SCAND/$SUB -type l`;
	    print "Found ".($#ALL+1)." links to add (x2)\n";
	} else {
	    @ALL   = `/usr/bin/find $SCAND/$SUB -type f`;
	    print "Found ".($#ALL+1)." files to add (x2)\n";
	}
    }
}

if ($#ALL == -1){ goto FINAL_EXIT;}


# Added algo to process by differences
if ( $DOCACHE ){
    $XSELF = "$SELF$SCANDS";
    $XSELF =~ s/[+\/\*]/_/g; 
    $kk=0;
    while ( -e "/tmp/$XSELF"."_$kk.lis"){  $kk++;}
    if ($kk != 0){
	# there is a previous $kk-1 file
	my(@count)=(0,0,0);
	
	if ( open(OCACHE,"/tmp/$XSELF"."_".($kk-1).".lis") ){
	    while ( defined($line = <OCACHE>) ){  
		chomp($line);
		$RECORDS{$line}=1;
		$count[0]++;
	    }
	    close(OCACHE);
	}
	push(@TEMP,@ALL); 
	if ( open(CACHE,">/tmp/$XSELF"."_".($kk).".lis") ){
	    undef(@ALL);
	    foreach $file (@TEMP){
		chomp($file);
		print CACHE "$file\n";
		if ( ! defined($RECORDS{$file}) ){  
		    $count[1]++;
		    push(@ALL,$file);
		}
		$count[2]++;
	    }
	    close(CACHE);
	}
	undef(@TEMP);
	if ($count[1] != $count[2]){
	    print "Previous pass had $count[0], found $count[2] and selected $count[1]\n";
	}
    } else {
	# still dump it all to an _0
	if ( open(CACHE,">/tmp/$XSELF"."_0.lis") ){
	    foreach $file (@ALL){
		chomp($file);
		print CACHE "$file\n";
	    }
	    close(CACHE);
	    print "Dumped initial ".($#ALL+1)." records\n";
	}
    }
}





$fC = FileCatalog->new();


# Get connection fills the blanks while reading from XML
# However, USER/PASSWORD presence are re-checked
#$fC->debug_on();
($USER,$PASSWD,$PORT,$HOST,$DB) = $fC->get_connection("Admin");
$port = $PORT if ( defined($PORT) );
$host = $HOST if ( defined($HOST) );
$db   = $DB   if ( defined($DB) );


if ( defined($USER) ){   $user = $USER;}
else {                   $user = "FC_admin";}

if ( defined($PASSWD) ){ $passwd = $PASSWD;}
else {                   print "Password for $user : ";
                         chomp($passwd = <STDIN>);}





#
# Now connect
#
if ( ! $fC->connect($user,$passwd,$port,$host,$db) ){
    &Stream("Error: Could not connect to $host $db using $user (passwd=OK)");
    goto FINAL_EXIT;
}

#$fC->debug_off();

$failed = $unkn = $old = $new = 0;

$fC->set_silent(1);                  # Turn OFF messaging
$fC->Require("V01.307");             # pathcomment and nodecomment requires a minimal version

# Make a main context
# Temporary so we get it once only
chomp($NODE    = `/bin/hostname`);
&Stream("We are on $NODE");




foreach  $file (@ALL){
    chomp($file);

    # If soft-link, check if real file is present or not
    if ( -l $file ){
	if ( $DOSL ){
	    $realfile = readlink($file);
	    next if ( ! -e $realfile);
	} else {
	    next;
	}
    } else {
	$realfile = "";
    }

    # Add hook file which will globally leave
    if ( -e "$CHKDIR/$SELF.quit"  && ! defined($ENV{SPDR_DEBUG}) ){
	print $FO "Warning :  $CHKDIR/$SELF.quit is present. Leaving\n";
	last;
    }

    # Skip some known pattern
    if ( $file =~ m/reco\/StarDb/){  next;}

    # We need to parse the information we can
    # save in the ddb
    $file =~ m/(.*\/)(.*)/;
    $path = $1; $file = $2;
    chop($path);
    $hpath= $path;

    if ($SUBPATH eq ""){
	$hpath=~ s/$SCAND/$HPSSD/;
    } else {
	$hpath=~ s/$SUBPATH//;
    }


    # Is a disk copy ??
    $fC->clear_context();
    if ( $path =~ m/\/star\/data/){
	$node    = "";
	$storage = "NFS";
	$fC->set_context("path = $path",
			 "filename = $file",
			 "storage = NFS",
			 "site = $SITE");

    } else {
	$storage = "local";
	$node    = $NODE;
	$fC->set_context("path = $path",
			 "filename = $file",
			 "storage = local",
			 "site = $SITE",
			 "node = $NODE");
    }
    @all1 = $fC->run_query("size");



    # HPSS copy (must be the last context to use clone_location() afterward )
    $fC->clear_context();
    $fC->set_context("path = $hpath",
		     "filename = $file",
		     "storage = HPSS",
		     "site = $SITE");
    @all = $fC->run_query("size");



    if ($#all == -1){
	$unkn++;
	&Stream("Warning : File not found as storage=HPSS -- $path/$file");

    } else {
	$mess = "Found ".($#all+1)." records for [$file] ";

	@stat   = stat("$path/$file");


	if ($#stat == -1){
	    &Stream("Error : stat () failed -- $path/$file");
	    next;
	}

	#if( $stat[7] != 0){
	#    $sanity = 1;
	#} else {
	#    $sanity = 0;
	#}


	if ($#all1 != -1){
	    $old++;
	    #print "$mess Already in ddb\n";

	} else {
	    #print "Cloning $hpath $file\n";
	    if ( ! $fC->clone_location() ){
		#print "Cloning of $file did not occur\n";

	    } else {
	        &Stream("$mess File cloned ".sprintf("%.2f %%",($new/($#ALL+1))*100))
		    if ($new % 10 == 0);
		$fC->set_context("persistent= 0");

		$fsize = $stat[7];
		@own   = getpwuid($stat[4]);
		$prot  = &ShowPerms($stat[2]);

		# Enabled, it may update createtime / not enabled, it will likely
		# set to previous value in clone context - Ideally, do NOT restore.
		#$dt    = &UnixDate(scalar(localtime($stat[10])),"%Y%m%d%H%M%S");
		$fC->set_context("path       = $path",
				 "storage    = $storage",
				 "persistent = 0",
				 "size       = $fsize",
				 "owner      = $own[0]",
				 "protection = $prot",
				 #"createtime = $dt",
				 "available  = 1",
				 "site       = $SITE");
		if ( $node ne ""){
		    #print "Setting node to $node\n";
		    $fC->set_context("node       = $node",
				     "nodecomment= 'Added by $SELF'",
				     "pathcomment= 'Added by $SELF'");
		}

		$fC->debug_on() if ( defined($ENV{SPDR_DEBUG}) );
		if ( ! $fC->insert_file_location() ){
		    &Stream("Error : Attempt to insert new location [$path] failed");
		    $failed++;
		} else {
		    $new++;
		}
		if ( defined($ENV{SPDR_DEBUG}) ){
		    die "DEBUG mode, Quitting\n";
		}
	    }
	}


    }
}

$fC->destroy();

FINAL_EXIT:
    if ($LOUT){
	print "Have lines, closing summary\n";
	print $FO
	    "$SELF :: Info :\n",
	    ($unkn  !=0 ? "\tUnknown = $unkn ".sprintf("%2.2f%%",100*$unkn/($unkn+$new+$old))."\n": ""),
	    ($old   !=0 ? "\tOld     = $old\n"   : ""),
	    ($new   !=0 ? "\tNew     = $new\n"   : ""),
	    ($failed!=0 ? "\tFailed  = $failed\n": "");

	# Check if we have opened a file
	if ($FO ne STDERR){
	    print $FO "Scan done on ".localtime()."\n";
	    close($FO);

	    # Save previous
	    if ( -e $FLNM.".last" ){  unlink($FLNM.".last");}
	    if ( -e $FLNM ){          rename($FLNM,$FLNM.".last");}
	    # rename new to final name
	    rename("$FLNM.tmp","$FLNM");
	}
    } else {
	# if nothing was output, delete ALL files (especially if they
	# were old)
	if ($FO ne STDERR){
	    close($FO);
	    unlink("$FLNM.tmp") if ( -e "$FLNM.tmp");
	    unlink("$FLNM")     if ( -e "$FLNM");
	}
    }

print "Done\n";



# Writes to file or STD and count lines
sub Stream
{
    my(@lines)=@_;

    foreach $line (@lines){
	$LOUT++;
	chomp($line);
	print $FO "$SELF :: $line\n";
    }
}


# This sub has been taken from fcheck software
# as-is (was too lazzy to get it done by myself)
sub ShowPerms
{
    local ($mode) = @_;

    local (@perms) = ("---", "--x",  "-w-",  "-wx",  "r--",  "r-x",  "rw-",  "rwx");
    local (@ftype) = ("?", "p", "c", "?", "d", "?", "b", "?", "-", "?", "l", "?", "s", "?", "?", "?");
    local ($setids) = ($mode & 07000)>>9;
    local (@permstrs) = @perms[($mode & 0700) >> 6, ($mode & 0070) >> 3, ($mode & 0007) >> 0];
    local ($ftype) = $ftype[($mode & 0170000)>>12];
    if ($setids){
	# Sticky Bit?
	if ($setids & 01) { $permstrs[2] =~ s/([-x])$/$1 eq 'x' ? 't' : 'T'/e; }
	# Setuid Bit?
	if ($setids & 04) { $permstrs[0] =~ s/([-x])$/$1 eq 'x' ? 's' : 'S'/e; }
	# Setgid Bit?
	if ($setids & 02) { $permstrs[1] =~ s/([-x])$/$1 eq 'x' ? 's' : 'S'/e; }
    }
    return (join('', $ftype, @permstrs));
}







