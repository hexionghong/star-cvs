#!/opt/star/bin/perl -w


#
# This script will function as a loop and will fill
# the operation DAQInfo table.
# BEWARE !!! This is an infite loop process filling
# information in the DAQInfo table.
#

use lib "/afs/rhic.bnl.gov/star/packages/scripts/";
use RunDAQ;

# Mode 1 will quit
$mode  = 1;
$sltime= 60;
$file  = "";   # DAQFill.log";

$mode   = shift(@ARGV) if ( @ARGV );
$arg1   = shift(@ARGV) if ( @ARGV );
$file   = shift(@ARGV) if ( @ARGV );

# We added this in 2004 for a bootstrap by run number
# range.
if ($arg1 > 0){ $sltime = $arg1;}


# We add an infinit loop around so the table will be filled
# as we go.
&Print("$0 starting on ".localtime()." ($sltime)\n");
do {
    $ctime = localtime();
    $dbObj = rdaq_open_odatabase();
    if( $dbObj){
	#&Print("O-Database opened\n");
	if ( ($obj = rdaq_open_rdatabase()) == 0){
	    &Print("Failed to open R-Database on ".localtime()."\n");
	} else {
	    #&Print("R-Database opened\n");

	    # get the top run. Note that <= 0 would get all
	    # runs ever entered in this database.
	    if ($arg1 < 0){
		$run = - $arg1;
		$prun= 0;
	    } else {
		$run = $mode*rdaq_last_run($dbObj);
		$prun= &GetRun();
	    }

	    if($prun ne $run){
		&Print("Last Run=$run on ".localtime()."\n");
	    }

	    $begin = 0;
	    $by    = 10000;
	    if($run >= 0){
		# fetch new records since that run number
		my($count,$total,$N);

		&Print("Bootstrap case Run >= 0, run=$run\n");
		$total = 0;
		rdaq_set_dlevel(1);
		
		do {
		    $count++; $total;
		    # cleanup
		    undef(@records);

		    @records = rdaq_raw_files($obj,$run,"$begin,$by");

		    # display info
		    if ($#records != -1){
			&Print("Fetched ".($#records+1)." records on $ctime, entered ");

			# record entries
			$N = rdaq_add_entries($dbObj,@records);
			&Print("Adding $N entries to $total so far\n");
			$total += $N;
		    }
		    $begin += $by;
		} while ($#records != -1);
		&Print("Got $total records in $count passes\n");
		
	    } else {
		&Print("Checking entries on ".localtime()."\n");
		$mode    = 0;  # main loop reset
		do {
		    @records = rdaq_raw_files($obj,$run,"$begin,$by");

		    &Print("Updating ".($#records+1)." on ".localtime()."\n");
		    rdaq_update_entries($dbObj,@records);
		    $begin += $by;
		} while ($#records != -1);
	    }

	    # close
	    rdaq_close_rdatabase($obj);

	}
	rdaq_close_odatabase($dbObj);
    } else {
	&Print("Failed to open O-Database on ".localtime()."\n");
    }

    sleep($sltime*$mode);
} while(1*$mode );


#
# perform IO on a file or STDOUT.
#
sub Print
{
    my(@all)=@_;

    if($file ne ""){
	while( ! open(FO,">>$file")){;}
	$FO = FO;
    } else {
	$FO = STDOUT;
    }
    foreach $el (@all){
	print $FO $el;
    }
    if($file ne ""){ close(FO);}
}

#
# Get last recorded run in file
#
sub GetRun
{
    my($line,$rv);

    $rv = "0.0";
    if( $file ne ""){
	$line = `tail -1 $file`;
	if ($line =~ m/(Run=)(\d+)\.(\d+)/){
	    $rv = "$2.$3";
	}
    }
    $rv;
}

