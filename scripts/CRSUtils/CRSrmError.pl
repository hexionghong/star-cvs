#! /usr/local/bin/perl -w
#
# L. Didenko
###############################################################################

 my $prodSer; 
my $jobdir;
my $archdir;

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
  }
 }

   if($Nerror >= 1 ) {

    print $Nerror,"  ", "jobs failed, below is a list:", "\n";

    @joblist = `crs_job -stat_show_problem | grep ERROR` ;
     @joblistf = `crs_job -stat_show_problem | grep FATAL` ;
    @jobsloop = `crs_job -stat_show_problem | grep looping` ;


    if(scalar(@joblistf) >= 1) {
	for($k=0; $k< scalar(@joblistf); $k++) {
        push @joblist, $joblistf[$k]; 

      }
    }

     if(scalar(@jobsloop) >= 1) {
	for($l=0; $l< scalar(@jobsloop); $l++) {
        push @joblist, $jobsloop[$l]; 

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
      @jobrr = split ("_", $wrd[0]);
      $prodSer = $jobrr[2];
      @prt = split (" ", $erline);
      $crsjobname = $prt[0];

#      print $jobname,"   ", $prt[1], "\n";

  $jobdir = "/home/starreco/newcrs/" . $prodSer ."/requests/daq/jobfiles";  
  $archdir = "/home/starreco/newcrs/" . $prodSer ."/requests/daq/archive";

     $fullname = $archdir ."/". $jobname;

    if ( $prt[1] eq "hpss_export_failed" ) {
    `crs_job -kill $crsjobname`;
     print "Job killed:  ", $jobname,"   ", $prt[1], "\n";
   }elsif($prt[1] eq "no_response_from_hpss_server") {
    `crs_job -reset $crsjobname`; 
     print "Job was reset:  ", $jobname,"   ", $prt[1], "\n";
   }elsif($prt[1] eq "pftp_get_failed") {
    `crs_job -reset $crsjobname`; 
     print "Job was reset:  ", $jobname,"   ", $prt[1], "\n";  

   }else{

    `mv $fullname $jobdir \n`;
    `crs_job -kill $crsjobname`;
        print "Job killed and resubmitted: ", $jobname,"   ", $prt[1],  "\n";

    next if($prt[1] eq "looping");

    $jobfilelist[$ii] = $jobname;
    $ii++;
    
  }
 }
#####################

    open (STDOUT, ">$outfile");

   print "#! /usr/local/bin/tcsh -f", "\n";

   foreach my $jfile (@jobfilelist) { 

    @wrd = split ("_",$jfile);
    $prod = $wrd[2]; 
    $trigset = $wrd[0]; 
    $field = $wrd[1]; 
    if( $jfile =~ /st_physics_adc_/ ) {
    $filebase = $wrd[3] ."_".$wrd[4]."_".$wrd[5]."_".$wrd[6]."_".$wrd[7]."_".$wrd[8];
  }else{
    $filebase = $wrd[3] ."_".$wrd[4]."_".$wrd[5]."_".$wrd[6]."_".$wrd[7];
  }   

    $pathname = "/star/data*/reco/".$trigset."/".$field."/".$prod."/"."*"."/"."*"."/".$filebase."*";

   print "rm ", $pathname, "\n";

 }
#####################
     close (STDOUT);

    `chmod +x $outfile`;

 }else {

     print "No failed jobs", "\n";
 }

exit;

