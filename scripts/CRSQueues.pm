#
# This module is used to get information about the CRS queue
# system. Only a few routine implemented so far.
#
#

use Carp;

package CRSQueues;
require 5.000;
require Exporter;
@ISA = qw(Exporter);

@EXPORT= qw(CRSQ_submit
	    CRSQ_getcnt
	    CRSQ_check
	    );


$STATUS="/usr/local/bin/crs_status.pl -m";
$JOBINF="/usr/local/bin/crs_status.pl -c";
$SUBMIT="/usr/local/bin/crs_submit.pl";
#$SUBMIT="/usr/crs/bin/CRS_submit_awc.pl";


$CRSQ::MAXQ=5;                  # support up to that number of queues 
$CRSQ::PFACT=5;                 # some number of files arbitrary proportion factor
$CRSQ::OFFSYNC=20;              # queue submission will continue up to this %tage
                                # of off-sync.
$CRSQ::GETCNT=0;                # called the getcnt() routine (not a config var)


# %CRSQ::TOT     assoc-array of queue info (total slots)
# %CRSQ::RUN     assoc-array of running jobs pass1
# %CRSQ::FND     assoc-array of found jobs pass2
# %CRSQ::DIF     hold initial differences (available slots)
# %CRSQ::WARN    Avoid multiple warnings by flagging it

#
# This routine checks the job files matching $pat
# if they are anywhere in the queue. If not, the jobfiles
# found are moved to a directory named $mdir. Note that
# this function will perform its expected task only when
# the queue will contain something ...
#
sub CRSQ_check
{
    my($pat,$mdir)=@_;
    my($line,$jfile);
    my(@result,@JOBS);
    my($tmp);


    @result = `$JOBINF`;

    # If the following test is true, something is wrong or we are at
    # the end of the year period or we are no longer running ...
    # Most probably, something is wrong with the CRS nodes (or at 
    # least, we need to prevent it from happening again).
    if($#result == -1){ return;}


    # Get a list of jobs still in the queue
    foreach $line (@result){
	chomp($line);
	$job = (split("%",$line))[1];
	$JOBS{$job} = 1;
    }

    # Scan the current directory for all job files
    @all = &Glob($pat);
    
    foreach $jfile (@all){
	if( ! defined($JOBS{$jfile}) ){
	    # It is no longer in the queue. We actually cannot much more
	    # here than moving the file into the archive directory.
	    if ($jfile =~ m/(.*\/)(.*)/){
		# path was specified in pattern
		$tmp =  $2;
	    } else {
		# no path specified
		$tmp = $jfile;
	    }
	    print "CRSQ :: Job $tmp no longer in the queue on ".
		localtime()."\n";
	    rename($jfile,"$mdir/$tmp");
	}
    }
}




# Check the number of slots in queue $qnum. 
#
#  If $drop is specified, check all queues lt than $qnum but no less than
# $qnum - $drop. Not this routine calculates for MAX jobs for queue $qnum 
# and MAX-1 for lower queues. However, if $drop is negative, $MAX-1 will
# enter in the base calculation of the number of available slots for $qnum.
#
#  If $pat is specified, check files with that wildcard pattern. If plenty
# exists, return an error. See comments for this obscure mode ...
#  The input file parsing can be disabled by using $nchk parameter (in case the 
# job files are pre-generated, we do not want to do this check).
#
# Returns the total number of available slots.
#
sub CRSQ_getcnt
{
    my($qnum,$drop,$pat,$nchk)=@_;

    my($i,$line,$ratio);
    my(@result,@items);
    my($NOTA,$TOT,$DEBUG);
    my(%NODES);


    $DEBUG = 0;
    
    # Sanity check
    if($qnum >= $CRSQ::MAXQ){  
	return &Exceed("QueueNum",$CRSQ::MAXQ);
    }


    # Initialization
    $CRSQ::GETCNT = 1;
    $NOTA = 0;
    for($i=0 ; $i < $CRSQ::MAXQ ; $i++){   
	$CRSQ::FND{$i} = $CRSQ::RUN{$i} = $CRSQ::TOT{$i} = 0;
	if( ! defined($CRSQ::WARN{$i}) ){ $CRSQ::WARN{$i} = 1;}
    }


    # Get queue status and counts first -- Pass 1
    @result = `$STATUS`;
    foreach $line (@result){
	chomp($line);
	@items = split("%",$line);

	if( $items[1] ne "unavailable"){
	    $i = $items[4];
	    $CRSQ::TOT{$i}       += $items[2];
	    $CRSQ::RUN{$i}       += $items[3];
	    $NODES{$items[0]}    += $i;

	    if( ($i < $qnum && $drop && $i != 0) || ($drop < 0)) {
		# Reduce by one comparing to MAX.
		$CRSQ::TOT{$i}--;
	    } 

	} else {
	    # Number of un-available nodes for any reasons
	    $NOTA++;
	}
    }
    $drop = abs($drop);


    # Calculate the total number of available jobs 
    foreach $i (keys %CRSQ::TOT){
	if( $i == 0){ next;}
	#printf 
	#    "Queue=%d Tot=%3d Run=%3d Dif=%3d\n",
	#    $i,$CRSQ::TOT{$i},$CRSQ::RUN{$i},
	#    ($CRSQ::TOT{$i}-$CRSQ::RUN{$i});

	$CRSQ::DIF{$i} = $CRSQ::TOT{$i}-$CRSQ::RUN{$i};
	$TOT += $CRSQ::TOT{$i};
    }


    # Extra sanity check. If the first command did not return
    # any information, no need to continue ...
    if($TOT == 0){
	print "CRSQ :: Error on ".localtime().
	    ", none of the queues are available\n";
	return -1;
    }


    # This small test implies that the job files will be moved
    # after beeing off the queue.
    # There are cases (July 30th for example) when the CRS 
    # queues goes beserk and the crs_status does not return 
    # any information. This leads to a bad count of jobs and
    # available slots (0 actually) and subsequently, too many 
    # submission ... The pattern find would prevent this. 
    if( defined($pat) && ! defined($nchk) ){
	@all = &Glob($pat);
	if($#all > $CRSQ::PFACT*$TOT ){
	    print 
		"CRSQ :: Warning on ".localtime().", there are ",
		"$#all job files found as $pat\n";
	    # Stop, this condition is ab-normal
	    return -1;
	} 
    }


    # --- Pass 2
    # Get the number of jobs actually on their way
    # to those queues. 'staging' or 'staged' jobs would not 
    # necessarily show up in the -m list. 
    # -c shows all jobs. Initial logic was a major bug because 
    # it did not account for this CRS queue peculiarity.
    # This part is relevant in no $drop mode only.
    @result = `$JOBINF`;

    foreach $line (@result){
	chomp($line);
	@items = split("%",$line);

	# complete the list. Easier for later testing.
	if( ! defined($NODES{$items[3]}) ){ $NODES{$items[3]} = 0;}

	$i = $NODES{$items[3]};
	if( $i != 0){
	    # This node is knows, hold queue $i and is busy
	    $CRSQ::FND{$i}++;
	} else {
	    # the best we can do is to count on the queue info
	    $CRSQ::FND{$items[4]}++;
	}
    }


    # Recalculate the total number of real slots we may use
    $TOT = 0;
    foreach $i (keys %CRSQ::TOT){
	if( $i == 0){ next;}
	if( ($i == $qnum && ! $drop)                         ||
	    ($i <= $qnum && $i >= ($qnum - $drop) && $drop)) {

	    if($DEBUG){
		printf 
		    ">> Queue=%d Tot=%3d Run=%3d Found=%3d Diff=%3d\n",
		    $i,$CRSQ::TOT{$i},$CRSQ::RUN{$i},$CRSQ::FND{$i},$CRSQ::DIF{$i};
	    }

	    # check if something else has submitted jobs there
	    if ( $CRSQ::DIF{$i} < 0 ){
		if( $CRSQ::WARN{$i} ){
		    $CRSQ::DIF{$i} = - $CRSQ::DIF{$i};		    
		    print 
			"CRSQ :: There are $CRSQ::DIF{$i} more jobs in queue $i ",
			"than what we intended to use on ".localtime()."\n";
		    $CRSQ::WARN{$i} = 0;
		}
		$CRSQ::DIF{$i}  = 0;

	    } elsif( $CRSQ::FND{$i}  > $CRSQ::RUN{$i}){
		# After the counting, we have found FND jobs but
		# RUN ones in pass 1 . Those are the un-accounted for
		# jobs in staged/staging or other weird states.

		# Make a diff adjustment
		if ( $CRSQ::DIF{$i} != 0 ){
		    $ratio = ($CRSQ::FND{$i}-$CRSQ::RUN{$i})/$CRSQ::DIF{$i}*100 ;
		} else {
		    $ratio = 100;
		}

		# The OFFSYNC value will decide on whether or not we
		# continue to submit jobs in this case.
		if( $ratio < $CRSQ::OFFSYNC ){
		    if( $CRSQ::WARN{$i} ){
			print 
			    "CRSQ :: Queue $i not in sync but within margin ",
			    "(Found $CRSQ::FND{$i} vs $CRSQ::RUN{$i} running over ",
			    " $CRSQ::DIF{$i} slots). Adjusting on ".localtime()."\n";
			$CRSQ::WARN{$i} = 0;
		    }
		    $CRSQ::DIF{$i} -= ($CRSQ::FND{$i}-$CRSQ::RUN{$i});
		    if( $CRSQ::DIF{$i} < 0){ $CRSQ::DIF{$i} = 0;}
		} else {
		    if( $CRSQ::WARN{$i} ){
			print 
			    "CRSQ :: Queue $i not in sync. Bootstrap ",
			    "found $CRSQ::FND{$i} but $CRSQ::RUN{$i} running ",
			    "on ".localtime()."\n";
			$CRSQ::WARN{$i} = 0;
		    }
		    $CRSQ::DIF{$i}  = 0;
		}

	    } else {
		# All OK (there are available slots) or nothing to do.
		$CRSQ::WARN{$i} = 1;

	    }


	    $TOT += $CRSQ::DIF{$i};

	} else {
	    # This needs to be reset for later use (submit takes 
	    # advantage of this).
	    $CRSQ::DIF{$i} = 0;
	}
    }

    # mem cleanup
    undef(@items);
    undef(@result);

    if( $TOT != 0){
	print "CRSQ :: ";
	foreach $i (keys %CRSQ::TOT){
	    print "Queue=$i ($CRSQ::DIF{$i}) " if ($CRSQ::TOT{$i} != 0);
	}
	print "\n";
    }

    #die "$TOT\n";

    # We don't care of what is happening to the spill-over
    # queue pool. If the count is incorrect there, just set
    # to 0.
    return $TOT;
}



#
# Submit $jfile with priority $prio. Eventually use a queue
# shift of $drop.
#
# Note that the last 2 arguments are not mandatory if
# the getcnt() routine was called. If you chose to submit
# that way, no jobs will be submitted after the getcnt()
# stack gets exhausted. If you still want to persist in
# submitting something, you must at least specify a default
# $qnum value. This latest mode is not encouraged ...
#
#
sub CRSQ_submit
{
    my($jfile,$prio,$qnum,$drop)=@_;
    my($res,$i,$q);

    if ( ! -e $jfile){
	print 
	    "CRSQ :: File $jfile does not exists. ",
	    "Cannot submit to queue $qnum\n";
	0;
    }
    if( ! defined($qnum) ){  $qnum = 0;}
    if( ! defined($drop) ){  $drop = 0;}
    $drop = abs($drop);

    if( $CRSQ::GETCNT ){
	#print "GETCNT() was called\n";
	foreach $i (sort {$b <=> $a} keys %CRSQ::DIF){
	    $q = $i;
	    if( $CRSQ::DIF{$i} != 0){ 
		$CRSQ::DIF{$i}--;
		last;
	    }
	}
	if( $q == 0 ){
	    # exhausted. Cannot make it that way
	    $CRSQ::GETCNT = 0;
	    # We will therefore comply with the exact command
	    # i.e. submitting in queue $qnum with $drop as
	    # arguments says. However, if we used that function
	    # with qnum=0, we have to leave now.
	    if( $qnum == 0){ return 0;}
	} else {
	    $drop = 0;
	    $qnum = $q;
	    #print "We have selected queue $q $CRSQ::DIF{$q}\n";
	}
    } 

    $res = `$SUBMIT $jfile $prio $qnum $drop`;
    $res =~ s/\n//g;
    if( $res =~ m/queue $qnum with priority $prio/){ 
	$qnum;
    } else {
	if ( $res =~ m/(queue\s+)(\d+)/ ){
	    print "CRSQ :: Failed to submit $jfile $qnum -> $2 => [$res]\n";
	} else {
	    print "CRSQ :: Failed to submit $jfile $qnum => [$res]\n";
	}
	0;
    }
}


# 
# Stupid blabla routine
#
sub Exceed
{
    my($param,$max)=@_;

    print "CRSQ :: Received a parameter exceeding expected range. $param < $max\n";
    return 0;
}

sub Glob
{
    my($pat)=@_;
    my(@all);

    # support perl patterns
    if( $pat =~ /\.\*/){
	# a .* like . 
	opendir(DIR,".");
	@all = grep { /$pat/ } readdir(DIR);
	close(DIR);
    } else {
	# use glob although this may fail
	@all = glob($pat);
    }
    @all;
}


1;

