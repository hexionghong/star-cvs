#!/opt/star/bin/perl -w
#
#  
#
#     read_mail.pl - script to read production emails and move
#          jobfiles if job crashed from archive to jobfiles to resubmit them  
#  L.Didenko
#
#  Modified J.lauret for inclusion of auto-submit in one script.
#        feature may be disabled by setting up the $SSUBM flag to 0.
#        or using 2nd argument.
#        Assumed syntax :
#           % readMail.pl [ProductionLevel] [{0|1}]
#
#  Configuration read from a file named readMail.conf if exists.
#  Supported keywords :
#       prodtag=XXXX       
#       qnum=XXX
#       dropq=XXX
#       autosub={0|1}
#
# By default, qnum=0 means ANY Email coming from the CRS system release
# a new job to the queue. If this parameter is specified, only Emails
# coming from that queue release a new job in that same queue for production
# version prodtag.
#
###############################################################################


my $mail_line;
my $status_line;
my $job_file = "none";
my $jbStat = "n/a";
my @parts;
my $nodeID = "n/a";
my $job_line; 
my @wrd;
my $date_line;
my ($sec,$min,$hour,$mday,$mon);
my $qnum=0;
my $drop=0;
my $AUTOS=1;

# Added J.Lauret June 11th 2001. Merged from the auto_submit.pl script.
# Merging is necessary since we are now running SMRSH (more practical
# in one script anyway).
# BTW, SMRSH seems to have some nasty side effects which will be later
# resolved i.e. it now thinks it is different script if an argument
# is passed ...
my $prodl;
if( defined($ARGV[0]) ){   
    $prodl = $ARGV[0];
} else {
    # default value for production level. DO NOT change this
    # but rather use the starreco:./readMail.conf file to
    # change the default.
    $prodl ="P01hg";
}


# ... so, also read the production tag from a conf file. 
# Warning : from a cronjob, $ENV{HOME} is null.
$HOME = $ENV{HOME};
if($HOME eq ""){ $HOME = ".";}
if( -e "$HOME/readMail.conf"){
    if (open(FI,"$HOME/readMail.conf") ){
	while( defined($line = <FI>) ){
	    chomp($line);
	    if($line ne ""){
		@items = split("=",$line);
		if( $items[0] =~ m/prodtag/i){
		    $prodl   = $items[1];
		} elsif ( $items[0] =~ m/qnum/i){   # qnum
		    $qnum    = $items[1];
		} elsif ( $items[0] =~ m/drop/i){   # dropq
		    $drop    = $items[1];
		} elsif ( $items[0] =~ m/auto/i){   # autosub
		    $AUTOS   = $items[1];
		}

	    }
	}
	close(FI);
    }
}


# root for a structure for an implied directory structure made of a
# 'jobfiles' directory and an 'archive' directory. This structure MUST
# be under nfs-tree since there would be a token issue otherwise.
my $SOURCE="/star/u/starreco/$prodl/requests/daq";
my $SUBMIT="/usr/local/bin/crs_submit.pl";         # crs submit script
my $PRIO=100;                                      # default job submit priority
my $SFLAG=0;                                       # flag for job sub. Auto set
my $SSUBM=1;                                       # set to 0 to disable submit

if( defined($ARGV[1]) ){  $SSUBM=$ARGV[1];}


# Some date for a mail file output.
($sec,$min,$hour,$mday,$mon) = localtime();

foreach my $int ( $mon,$mday ){
    $int < 10 and $int = '0'.$int;
    $thisday .= $int;
}


# This script also loses Email content. Not very good for
# debugging purposes so ...
open(FO,">>mbox.piped");


$outfile = "mail" . "_" .$thisday . "_" . "out"; 
$QFLAG   = 1==1;

while (<>) {
    chomp($mail_line = $_);
    print FO "$mail_line\n";

    if ($mail_line =~ /Date/) {
	$date_line = $mail_line;
    } elsif ($mail_line =~ m/(in queue )(\d+)( with priority)/){
	# turn submission ON/OFF based on queue number
	if( $qnum != $2 && $qnum != 0){
	    $QFLAG = 1==0;
	}
    }

    if ($mail_line =~ /job_\d+/) {
	$SFLAG += 1; # must be at least 2

	$status_line = $mail_line;
 
	if ( $status_line =~ /done/) {
	    $jbStat = "done";
	    @wrd = split (" ",$status_line);
	    $nodeID = $wrd[3];        

	} elsif ( $status_line =~ /staging failed/) {
	    $jbStat = "staging failed";
	    @wrd = split (" ",$status_line);
	    $nodeID = $wrd[4]; 

	} elsif ( $status_line =~ /queuing failed/) {
	    $jbStat = "queuing failed";
	    @wrd = split (" ",$status_line);
	    $nodeID = $wrd[4]; 

	} elsif ($status_line =~ /aborted/) {
	    $jbStat = "aborted";
	    @wrd = split (" ",$status_line);
	    $nodeID = $wrd[3]; 

	} elsif ($status_line =~ /killed/) {
	    $jbStat = "killed";
	    @wrd = split (" ",$status_line);
	    $nodeID = $wrd[3]; 

	} elsif ($status_line =~ /file not found/) {
	    $jbStat = "file not found";
	    #$nofiles_count++;
	    $nodeID = "n/a"; 

	} elsif ($status_line =~ /crashed/) {
	    $jbStat = "crashed";
	    @wrd = split (" ",$status_line);
	    $nodeID = $wrd[3]; 
	}
	
    } elsif ($mail_line =~ /Description/) {
	$SFLAG += 1; # must be at least 2

	@parts = split (":", $mail_line);
	$job_file = $parts[1];
    }

}
close(FO);

if( defined($date_line) ){
    open (OUT,">> $outfile") or die "Can't open $outfile";
    print OUT $date_line, "\n";
    print OUT "JobInfo:  %  $jbStat % $nodeID % $job_file % $qnum % $drop\n"; 
    close (OUT);
}

# SFLAG -> The job received was indeintified as a CRS job not some
#          other Emails.
# QFLAG -> The queue selection passed
# SSUBM -> AutoSub (arg2 or hard-coded value, bacward compat)
# AUTOS -> Configuration file parameter says OK
#
if ($SFLAG == 2 && $QFLAG && $SSUBM && $AUTOS){
    # Now, the logic for file submission. Simple and fast ...
    opendir(JDIR,"$SOURCE/jobfiles/");

    while ( defined($file = readdir(JDIR)) ){
	if( $file =~ /$prodl/ && $file !~ /\.lock/){
	    $lock = "$SOURCE/jobfiles/$file.lock";
	    if( ! -e "$lock" ){
		if ( open(FO,">$lock") ){
                    $cmd = "$SUBMIT $SOURCE/jobfiles/$file $PRIO ";
		    if( $qnum !=0){
			$cmd .= " $qnum ";
			if( $drop != 0){
			    $cmd .= " $drop";
			}
		    } 

		    system($cmd);
		    rename("$SOURCE/jobfiles/$file","$SOURCE/archive/$file");
		    # Just for the heck of it, output submit debugging
		    &ASLog("Job $SOURCE/jobfiles/$file submitted ($qnum/$drop)");
		    unlink($lock);
		    last;
		} else {
		    &ASLog("Lock $lock creation failed");
		}
	    }
	}
    }
    close(JDIR);
}

exit;


# Subroutines ...
sub ASLog
{
    my($line)=@_;

    if ( open(FL,">>autosubmit.log") ){
	print FL localtime()." $line\n";
	close(FL);
    }
}
