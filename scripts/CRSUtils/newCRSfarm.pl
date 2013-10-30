#!/usr/local/bin/perl -w
#
#  
#
#  L. Didenko
#
#   Script to monitor jobs status and errors using new CRS software interface
#
###############################################################################

 use DBI;

$dbhost="duvall.star.bnl.gov";
$dbuser="starreco";
$dbpass="";
$dbname="operation";


# Tables
$crsJobStatusT = "newcrsJobState";

 my @statlist = ();
 my @joblist  = ();

 @statlist = `/usr/bin/crs_summary`;
 
 my $year;
 my $mon = 0;
 my $mday = 0;
 my $hour = 0;
 my $min = 0;
 my $sec = 0;
 my $thisday ;

my $Ncreate = 0;
my $Nqueued = 0;
my $Nstage = 0;
my $Nsubm = 0;
my $Nimport = 0;
my $Nrun = 0;
my $Nexport = 0;
my $Ndone = 0;
my $Nerror = 0;
my $Nkill = 0;
my $Nheld = 0;

my $NFprestage = 0;
my $NFexport = 0;
my $NFimport = 0;
my $NFretry = 0;
my $Nioerror = 0;
my $NFcondor = 0;
my $NFjexec = 0;
my $Tperror = 0;

my @joberr = ();

my $jid = 0;

my @prt = ();
my @wrd = ();
my @pt = ();

 ($sec,$min,$hour,$mday,$mon,$yr) = localtime;


    $mon++;
if( $mon < 10) { $mon = '0'.$mon };
if( $mday < 10) { $mday = '0'.$mday };
if( $hour < 10) { $hour = '0'.$hour };
if( $min < 10) { $min = '0'.$min };
if( $sec < 10) { $sec = '0'.$sec };

 $year = $yr + 1900;

 $thisday = $year."-".$mon."-".$mday." ".$hour.":".$min.":".$sec;

 print $thisday, "\n";

   &StcrsdbConnect();

  foreach my $line (@statlist) {
     chop $line ;

    next if($line =~ /---/ or $line =~ /Status/ );
   print  $line, "\n";
   @prt = ();
   @prt = split (" ", $line);

#   print "Check parts  ", ,$prt[1],"  ",$prt[3],"\n";

     if ($prt[3] eq "SUBMITTED") {
	 $Nsubm =  $prt[1];
	} elsif ($prt[3] eq "CREATED") {
        $Ncreate =  $prt[1]; 
	} elsif ($prt[3] eq "QUEUED") {         
        $Nqueued =  $prt[1]; 
	} elsif ($prt[3] eq "STAGING") {       
        $Nstage =  $prt[1];
	} elsif ($prt[3] eq "IMPORTING") {       
        $Nimport =  $prt[1];
	} elsif ($prt[3] eq "RUNNING") {       
        $Nrun =  $prt[1];
	} elsif ($prt[3] eq "EXPORTING") {       
        $Nexport =  $prt[1];
	} elsif ($prt[3] eq "DONE") { 
        $Ndone =  $prt[1];
	} elsif ($prt[3] eq "ERROR") {        
        $Nerror =  $prt[1];
 	} elsif ($prt[3] eq "KILLED") {        
        $Nkill =  $prt[1];
	} elsif ($prt[3] eq "HELD") {        
        $Nheld =  $prt[1];
        }
    }

  @joblist = `crs_job -stat | grep ERROR` ;

   
    foreach my $jline (@joblist) {
     chop $jline ;
#     print $jline, "\n";
     @wrd = ();
     @wrd = split (" ", $jline);
     print $wrd[0],"   ",$wrd[1], "\n";

     $jid = $wrd[0];
     $jid = substr($wrd[0],0,-1) + 0;

    print "Job id = ",$jid, "\n";

    @joberr = ();
    
    @joberr = `crs_job -long $jid | grep Error`;

    foreach my $erline (@joberr) {
     chop $erline ;
     print $erline, "\n";
     if ( $erline =~ /Error/ ) {

     @pt = ();
     @pt = split (" ", $erline);

#  print "Error line : ", $pt[1],"  ", $pt[2],"  ",$pt[3], "\n";
     
     $Tperror = $pt[2];
     $Tperror =~ s/://g;

   print ""Job id and error number =  ", $jid,"   ",$Tperror,"\n";

      if($Tperror == 10) {
	 $NFcondor++;
    }elsif($Tperror == 20) {
        $NFprestage++;
    }elsif($Tperror == 30) {
        $NFretry++;  
    }elsif($Tperror == 40) {
        $NFimport++;  
    }elsif($Tperror == 50) {
        $NFjexec++;  
    }elsif($Tperror == 60) {
        $NFimport++;  
    }elsif($Tperror == 70) {
        $Nioerror++;  
     }
######
     }
    }

   `crs_job -kill -f $jid`; 

   print "Job   ",$jid,"   was killed","\n";
 
   `crs_job -destroy -f $jid`; 

   print "Job   ",$jid,"   was destroied","\n";

#####

   }

     &fillTable();

    print "Ncreate = ", $Ncreate,"   ","Nqueued = ",$Nqueued,"   ","Nstage = ",$Nstage,"   ","Nsubm = ", $Nsubm,"   ","Nimport = ",$Nimport,"   ","Nrun = ",$Nrun,"   ","Nexport = ",$Nexport,"   ","Ndone = ",$Ndone,"   ","Nerror = ",$Nerror,"   ","Nkill = ",$Nkill,"   ","Nheld = ",$Nheld, "\n";

   &StcrsdbDisconnect();

exit;


#################################################################################################

  sub fillTable {

 $sql="insert into $crsJobStatusT set ";
 $sql.="created='$Ncreate',"; 
 $sql.="submitted='$Nsubm',";
 $sql.="staging='$Nstage',";
 $sql.="queued='$Nqueued',";
 $sql.="importing='$Nimport',"; 
 $sql.="running='$Nrun',";
 $sql.="exporting='$Nexport',";
 $sql.="killed='$Nkill',";
 $sql.="held='$Nheld',";
 $sql.="done='$Ndone',";
 $sql.="error='$Nerror',";
 $sql.="prestaging_failed='$NFprestage',";
 $sql.="hpss_export_failed='$NFexport',";
 $sql.="hpss_import_failed='$NFimport',";
 $sql.="hpss_retry_failed='$NFretry',";
 $sql.="job_exec_failed='$NFjexec',";
 $sql.="io_error='$Nioerror',";
 $sql.="condor_failed='$NFcondor',";
 $sql.="sdate='$thisday' "; 
#   print "$sql\n" if $debugOn;
    # $rv = $dbh->do($sql) || die $dbh->errstr;
    $dbh->do($sql) || die $dbh->errstr;
   }

##################################################################################################
sub StcrsdbConnect {
    $dbh = DBI->connect("dbi:mysql:$dbname:$dbhost", $dbuser, $dbpass)
        || die "Cannot connect to db server $DBI::errstr\n";
}

######################
sub StcrsdbDisconnect {
    $dbh = $dbh->disconnect() || die "Disconnect failure $DBI::errstr\n";
}
