#! /usr/local/bin/perl -w
#
# L. Didenko
###############################################################################

 my $prodSer = $ARGV[0]; 

use DBI;

$dbhost="duvall.star.bnl.gov";
$dbuser="starreco";
$dbpass="";
$dbname="operation";

$JobStatusT = "JobStatus2010";


my $jobdir;
my $archdir;
my $lostdir;
my $loopdir;

 my @statlist = ();
 my @joblist = ();
 my @joblistf = ();
 my @jobsloop = ();
 my $timestamp ;
 my $fullname;


 @statlist = `farmstat`;

my $Ndone = 0;
my $Nerror = 0;

my $jobname;
my $crsjobname;
my @prt = ();
my @wrd = ();
my $year = ();
my $mfile;
my $filebase;
my $filestamp;
my @jobfilelist = ();
my $ii = 0;
my $prod;
my $trigset;
my @jobrr = ();
my $Nfatal = 0;
my $Nfail = 0;
my $Nloop = 0;
my $pathname;

  ($sec,$min,$hour,$mday,$mon,$yr) = localtime;

  
    $mon++;
if( $mon < 10) { $mon = '0'.$mon };
if( $mday < 10) { $mday = '0'.$mday };
if( $hour < 10) { $hour = '0'.$hour };
if( $min < 10) { $min = '0'.$min };
if( $sec < 10) { $sec = '0'.$sec };

$year = $yr + 1900 ;

  $timestamp = $year."-".$mon."-".$mday." ".$hour.":".$min.":".$sec;
 $filestamp = $mon."-".$mday."-".$hour.":".$min;

my $outfile = "/star/u/starreco/failjobs.".$filestamp.".csh";

 print $timestamp, "\n";

  foreach my $line (@statlist) {
     chop $line ;
#   print  $line, "\n";
    @prt = ();
    @prt = split (" ", $line);
     if ($prt[0] eq "ERROR") {
         $Nerror =  $prt[1]; 
    } elsif($prt[0] eq "FATAL") {
        $Nfatal =  $prt[1]; 
  }
 }

      @jobsloop = `crs_job -stat_show_problem | grep looping` ;

##################################################### remove looping jobs

  $Nloop = scalar(@jobsloop);

   &StDbProdConnect(); 

           if(scalar(@jobsloop) >= 1) {
  
     print $Nloop,"  ", "jobs looping:", "\n";

    foreach my $lline (@jobsloop) {
     chop $lline ;
#      print $lline, "\n";       

      @wrd = ();
      @prt = ();
      @jobrr = ();
      @wrd = split ("-", $lline);
      $jobname = $wrd[0];
#      @jobrr = split ("_", $wrd[0]);
#      $prodSer = $jobrr[2];
      @prt = split (" ", $lline);
      $crsjobname = $prt[0];

   $loopdir = "/home/starreco/newcrs/" . $prodSer ."/requests/daq/jobs_looping"; 


  $jobdir = "/home/starreco/newcrs/" . $prodSer ."/requests/daq/jobfiles";  
  $archdir = "/home/starreco/newcrs/" . $prodSer ."/requests/daq/archive";

     $fullname = $archdir ."/". $jobname;

############## uncomment next lines

      `crs_job -kill $crsjobname`;
     next if( $jobname =~ /dev/);

      `mv $fullname $loopdir \n`;
        print "Looping job killed and moved to jobs_looping dir: ", $jobname,"   ", $prt[1],  "\n";
#         print "Found looping jobs: ", $jobname,"   ", $prt[1],  "\n";

      $sql="update $JobStatusT set jobStatus = 'hunging'  where jobfileName = '$jobname' ";
      $rv = $dbh->do($sql) || die $dbh->errstr;   

    }
  }


   if($Nerror >= 1 || $Nfatal >= 1 ) {

       $Nfail = $Nerror + $Nfatal;

    print $Nfail,"  ", "jobs failed, below is a list:", "\n";

    @joblist = `crs_job -stat_show_problem | grep ERROR` ;
     @joblistf = `crs_job -stat_show_problem | grep FATAL` ;

    if(scalar(@joblistf) >= 1) {
	for($k=0; $k< scalar(@joblistf); $k++) {
        push @joblist, $joblistf[$k]; 

      }
    }

    foreach my $erline (@joblist) {
     chop $erline ;
#      print $erline, "\n";       

      @wrd = ();
      @prt = ();
      @jobrr = ();
      @wrd = split ("-", $erline);
      $jobname = $wrd[0];
#      @jobrr = split ("_", $wrd[0]);
#      $prodSer = $jobrr[2];
      @prt = split (" ", $erline);
      $crsjobname = $prt[0];

#      print $jobname,"   ", $prt[1], "\n";

  $jobdir = "/home/starreco/newcrs/" . $prodSer ."/requests/daq/jobfiles";  
  $archdir = "/home/starreco/newcrs/" . $prodSer ."/requests/daq/archive";

  $lostdir = "/home/starreco/newcrs/" . $prodSer ."/requests/daq/jobs_lostfiles"; 

     $fullname = $archdir ."/". $jobname;

    if ( $prt[1] eq "hpss_export_failed" ) {
    `crs_job -kill $crsjobname`;
     print "Job killed:  ", $jobname,"   ", $prt[1], "\n";

   }elsif($prt[1] eq "hpss_error_-2" or $prt[1] eq "hpss_error_-5" or $prt[1] eq "hpss_error_-150" ) {
    `crs_job -kill $crsjobname`;
     print "Job killed:  ", $jobname,"   ", $prt[1], "\n";
     `mv $fullname $lostdir \n`;
 
      $sql="update $JobStatusT set inputHpssStatus = '$prt[1]' where jobfileName = '$jobname' ";
      $rv = $dbh->do($sql) || die $dbh->errstr;
  

    }elsif($prt[1] eq "hpss_error_-153" or $prt[1] eq "hpss_error_-154" ) {
 
    `crs_job -kill $crsjobname`;
     print "Job killed:  ", $jobname,"   ", $prt[1], "\n";
     `mv $fullname $lostdir \n`;

      $sql="update $JobStatusT set inputHpssStatus = '$prt[1]' where jobfileName = '$jobname' ";
      $rv = $dbh->do($sql) || die $dbh->errstr;

    }elsif($prt[1] =~ /hpss_error/ ) {
 
    `crs_job -kill $crsjobname`;
     print "Job killed:  ", $jobname,"   ", $prt[1], "\n";
     `mv $fullname $lostdir \n`;

      $sql="update $JobStatusT set inputHpssStatus = '$prt[1]' where jobfileName = '$jobname' ";
      $rv = $dbh->do($sql) || die $dbh->errstr;

   }elsif($prt[1] eq "no_response_from_hpss_server") {
    `crs_job -reset $crsjobname`; 
     print "Job was reset:  ", $jobname,"   ", $prt[1], "\n";

   }elsif($prt[1] eq "hpss_request_submission_timed_out") {
    `crs_job -reset $crsjobname`; 
     print "Job was reset:  ", $jobname,"   ", $prt[1], "\n";

   }elsif($prt[1] eq "hpss_stage_request_timed_out") {
    `crs_job -reset $crsjobname`; 
     print "Job was reset:  ", $jobname,"   ", $prt[1], "\n";

   }elsif($prt[1] eq "hpss_busy") {
    `crs_job -reset $crsjobname`; 
     print "Job was reset:  ", $jobname,"   ", $prt[1], "\n";

   }elsif($prt[1] eq "pftp_get_failed") {
    `crs_job -reset $crsjobname`; 
     print "Job was reset:  ", $jobname,"   ", $prt[1], "\n";  

   }elsif($prt[1] eq "evicted_by_condor") {
    `crs_job -reset $crsjobname`; 
     print "Job was reset:  ", $jobname,"   ", $prt[1], "\n";  

   }else{

    `crs_job -kill $crsjobname`;
    next if( $jobname =~ /dev/);   

    `mv $fullname $lostdir \n`;
     print "Job killed:  ", $jobname,"   ", $prt[1],  "\n";
     
#    $jobfilelist[$ii] = $jobname;
#    $ii++;
    
  }
 }

 }else {

     print "No failed jobs", "\n";
 }

     &StDbProdDisconnect();

exit;


######################
sub StDbProdConnect {
    $dbh = DBI->connect("dbi:mysql:$dbname:$dbhost", $dbuser, $dbpass)
        || die "Cannot connect to db server $DBI::errstr\n";
}


######################
sub StDbProdDisconnect {
    $dbh = $dbh->disconnect() || die "Disconnect failure $DBI::errstr\n";
}
