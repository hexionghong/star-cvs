#!/usr/local/bin/perl
#
# 
#
# L.Didenko
#
# RetriveGridJobs.pl
#
# Retrive failed GRID production jobs
# 
################################################################################################

BEGIN {
 use CGI::Carp qw(fatalsToBrowser carpout);
}


use DBI;
use CGI qw(:standard);
#use Mysql;
use Class::Struct;


($sec,$min,$hour,$mday,$mon,$year) = localtime();

 $mon++;

if( $mon < 10) { $mon = '0'.$mon };
if( $mday < 10) { $mday = '0'.$mday };
if( $hour < 10) { $hour = '0'.$hour };
if( $min < 10) { $min = '0'.$min };
if( $sec < 10) { $sec = '0'.$sec };

my $todate = ($year+1900)."-".$mon."-".$mday." ".$hour.":".$min.":".$sec;

$dbhost="duvall.star.bnl.gov";
$dbuser="starreco";
$dbpass="";
$dbname="Embedding_job_stats";

my $JobStatusT = "jobs_prod_2013";

my $query = new CGI;

    if ( exists($ENV{'QUERY_STRING'}) ) { print $query->header };


my $qtrg = $query->param('trigs');
my $qprod = $query->param('prod');
my $qflag = $query->param('jflag');

my @jbfName = ();
my @jbEvent = ();
my @jbStatus  = ();
my @jbftime = ();
my @jbsubdate = ();
my @jbstate = ();
my @jbmudst = ();
my @jbdaq = ();
my @crsubdate = ();
my @globuserr = ();
my @jbprogress = ();
my @jbdaqsize = ();
my @dqsizeOnsite = ();
my @jblogstat = ();
my @jbcondorid = ();

my $nn = 0;
my $ii = 0;


  &StDbProdConnect();

   if( $qflag eq "jstat") {
 
  &beginJbHtml(); 

 $nn = 0; 

 $sql="SELECT inputFileName, recoStatus, nEvents, submissionTime, muDstCreateTime from $JobStatusT where recoStatus <> 'completed' and recoStatus <> 'unknown' and prodTag = '$qprod' and datasetName = '$qtrg' ";

          $cursor =$dbh->prepare($sql)
              || die "Cannot prepare statement: $DBI::errstr\n";
          $cursor->execute();

        while(@fields = $cursor->fetchrow) {

          $jbfName [$nn]  = $fields[0];
          $jbStatus[$nn]  = $fields[1];
          $jbEvent[$nn]   = $fields[2];
          $jbsubdate[$nn] = $fields[3];
          $jbftime[$nn]   = $fields[4];
	  $nn++;

         }

   $cursor->finish();

   
 }elsif($qflag eq "unknown") {

   &beginUnHtml();

  $nn = 0; 

 $sql="SELECT inputFileName, inputFileSize, daqSizeOnSite, jobProgress, submissionTime, condorJobID, logFileState, muDstStatus, globusError from $JobStatusT where jobState = 'done' and recoStatus = 'unknown' and prodTag = '$qprod' and datasetName = '$qtrg' ";

          $cursor =$dbh->prepare($sql)
              || die "Cannot prepare statement: $DBI::errstr\n";
          $cursor->execute();

        while(@fields = $cursor->fetchrow) {

          $jbfName[$nn]  = $fields[0];
          $jbdaqsize[$nn]  = $fields[1];
          $dqsizeOnsite[$nn]  = $fields[2]; 
          $jbprogress[$nn]  = $fields[3];
          $jbsubdate[$nn] = $fields[4];
          $jbcondorid[$nn] = $fields[5];
          $jblogstat[$nn]   = $fields[6];
          $jbmudst[$nn]   = $fields[7];
          $globuserr[$nn] = $fields[8];
	  $nn++;

         }

   $cursor->finish();

 
 }elsif($qflag eq "mudst") {

   &beginMuHtml();

  $nn = 0; 

  $sql="SELECT inputFileName, jobProgress, recoStatus, nEvents, submissionTime, globusError from $JobStatusT where prodTag = '$qprod' and datasetName = '$qtrg' and jobState = 'done'  and ( muDstStatus = 'missing' or  muDstStatus = 'corrupted')" ;

          $cursor =$dbh->prepare($sql)
              || die "Cannot prepare statement: $DBI::errstr\n";
          $cursor->execute();

        while(@fields = $cursor->fetchrow) {

          $jbfName [$nn]  = $fields[0];
          $jbstate[$nn]  = $fields[1];
          $jbStatus[$nn]  = $fields[2];
          $jbEvent[$nn]   = $fields[3];
          $jbsubdate[$nn] = $fields[4];
          $globuserr[$nn] = $fields[5];
	  $nn++;

         }

   $cursor->finish();


 }elsif($qflag eq "mlog") {

   &beginLogHtml();

  $nn = 0; 

 $sql="SELECT inputFileName, jobProgress, jobState, submissionTime, logFileState, muDstStatus, globusError  from $JobStatusT where  prodTag = '$qprod' and datasetName = '$qtrg' and  jobState = 'done' and (logFileState = 'missing' or logFileState = 'truncated' )  ";


          $cursor =$dbh->prepare($sql)
              || die "Cannot prepare statement: $DBI::errstr\n";
          $cursor->execute();

        while(@fields = $cursor->fetchrow) {

          $jbfName [$nn]  = $fields[0];
          $jbprogess[$nn]  = $fields[1];
          $jbstate[$nn]  = $fields[2];
          $jbsubdate[$nn] = $fields[3];
          $jblogstate[$nn] = $fields[4];
          $jbmudst[$nn]   = $fields[5];
          $globuserr[$nn] = $fields[6];

	  $nn++;
         }

   $cursor->finish();

 }elsif($qflag eq "mdaq") {

   &beginMDHtml();

  $nn = 0; 

 $sql="SELECT inputFileName, carouselSubTime, jobState from $JobStatusT where prodTag = '$qprod' and datasetName = '$qtrg' and inputFileExists = 'no' ";

          $cursor =$dbh->prepare($sql)
              || die "Cannot prepare statement: $DBI::errstr\n";
          $cursor->execute();

        while(@fields = $cursor->fetchrow) {

          $jbfName [$nn]  = $fields[0];
          $crsubdate[$nn] = $fields[1];
          $jbstate[$nn]  = $fields[2];
	  $nn++;

         }

   $cursor->finish();

 }elsif($qflag eq "mglob") {

   &beginGLHtml();

  $nn = 0; 


 $sql="SELECT inputFileName, globusError, submissionTime, jobProgress, jobState, muDstStatus from $JobStatusT where prodTag = '$qprod' and datasetName = '$qtrg' and globusError <> ' ' ";

          $cursor =$dbh->prepare($sql)
              || die "Cannot prepare statement: $DBI::errstr\n";
          $cursor->execute();

        while(@fields = $cursor->fetchrow) {

          $jbfName [$nn]  = $fields[0];
          $globuserr[$nn]  = $fields[1];
          $jbsubdate[$nn] = $fields[2];
          $jbprogress[$nn] = $fields[3];
          $jbstate[$nn]  = $fields[4];
          $jbmudst[$nn]  = $fields[5];
	  $nn++;

         }

   $cursor->finish();



  }else{

   &beginHtml();
 }

&StDbProdDisconnect(); 


    if( $qflag eq "jstat" ) {

   for ( $ii=0; $ii<$nn; $ii++ ) {

 print <<END;

<TR ALIGN=CENTER HEIGHT=10 bgcolor=\"cornsilk\">
<td HEIGHT=10><h3>$jbfName[$ii]</h3></td>
<td HEIGHT=10><h3>$jbStatus[$ii]</h3></td>
<td HEIGHT=10><h3>$jbEvent[$ii]</h3></td>
<td HEIGHT=10><h3>$jbsubdate[$ii]</h3></td>
<td HEIGHT=10><h3>$jbftime[$ii]</h3></td>
</TR>
END
   }

 }elsif($qflag eq "mudst") {

 for ( $ii=0; $ii<$nn; $ii++ ) {

print <<END;

<TR ALIGN=CENTER HEIGHT=10 bgcolor=\"cornsilk\">
<td HEIGHT=10><h3>$jbfName[$ii]</h3></td>
<td HEIGHT=10><h3>$jbstate[$ii]</h3></td>
<td HEIGHT=10><h3>$jbStatus[$ii]</h3></td>
<td HEIGHT=10><h3>$jbEvent[$ii]</h3></td>
<td HEIGHT=10><h3>$jbsubdate[$ii]</h3></td>
<td HEIGHT=10><h3>$jbftime[$ii]</h3></td>
</TR>
END
 }


 }elsif($qflag eq "unknown") {

 for ( $ii=0; $ii<$nn; $ii++ ) {

print <<END;

<TR ALIGN=CENTER HEIGHT=10 bgcolor=\"cornsilk\">
<td HEIGHT=10><h3>$jbfName[$ii]</h3></td>
<td HEIGHT=10><h3>$jbdaqsize[$ii]</h3></td>
<td HEIGHT=10><h3>$dqsizeOnsite[$ii]</h3></td>
<td HEIGHT=10><h3>$jbprogress[$ii]</h3></td>
<td HEIGHT=10><h3>$jbsubdate[$ii]</h3></td>
<td HEIGHT=10><h3>$jbcondorid[$ii]</h3></td>
<td HEIGHT=10><h3>$jblogstat[$ii]</h3></td>
<td HEIGHT=10><h3>$jbmudst[$ii]</h3></td>
<td HEIGHT=10><h3>$globuserr[$ii]</h3></td>
</TR>
END
 }

 }elsif($qflag eq "mlog") {

for ($ii=0; $ii<$nn; $ii++ ) {

print <<END;

<TR ALIGN=CENTER HEIGHT=10 bgcolor=\"cornsilk\">
<td HEIGHT=10><h3>$jbfName[$ii]</h3></td>
<td HEIGHT=10><h3>$jbprogess[$ii]</h3></td>
<td HEIGHT=10><h3>$jbstate[$ii]</h3></td>
<td HEIGHT=10><h3>$jbsubdate[$ii]</h3></td>
<td HEIGHT=10><h3>$jblogstate[$ii]</h3></td>
<td HEIGHT=10><h3>$jbmudst[$ii]</h3></td>
<td HEIGHT=10><h3>$globuserr[$ii]</h3></td>
</TR>
END
   }

 }elsif($qflag eq "mdaq") {

for ($ii=0; $ii<$nn; $ii++ ) {

print <<END;

<TR ALIGN=CENTER HEIGHT=10 bgcolor=\"cornsilk\">
<td HEIGHT=10><h3>$jbfName[$ii]</h3></td>
<td HEIGHT=10><h3>$crsubdate[$ii]</h3></td>
<td HEIGHT=10><h3>$jbstate[$ii]</h3></td>
</TR>
END
   }

 }elsif($qflag eq "mglob") {

for ($ii=0; $ii<$nn; $ii++ ) {

print <<END;

<TR ALIGN=CENTER HEIGHT=10 bgcolor=\"cornsilk\">
<td HEIGHT=10><h3>$jbfName[$ii]</h3></td>
<td HEIGHT=10><h3>$globuserr[$ii]</h3></td>
<td HEIGHT=10><h3>$jbsubdate[$ii]</h3></td>
<td HEIGHT=10><h3>$jbprogress[$ii]</h3></td>
<td HEIGHT=10><h3>$jbstate[$ii]</h3></td>
<td HEIGHT=10><h3>$jbmudst[$ii]</h3></td>

</TR>
END
   }

 }

 &endHtml();

######################

sub beginHtml {

print <<END;

  <html>
   <body BGCOLOR=\"cornsilk\"> 
 <h2 ALIGN=CENTER> <B>List of jobs failed for<font color="blue"> $qprod </font> production <br> and <font color="blue">$qtrg </font> dataset  </B></h2>
 <h3 ALIGN=CENTER> Generated on $todate</h3>
<TABLE ALIGN=CENTER BORDER=5 CELLSPACING=1 CELLPADDING=2 bgcolor=\"#ffdc9f\">
<TR>
<TD ALIGN=CENTER WIDTH=\"50%\" HEIGHT=60><B><h3>No failed jobs</h3></B></TD>
   </TR>
    </body>
END
}

#####################################

sub beginJbHtml {

print <<END;

  <html>
   <body BGCOLOR=\"cornsilk\">
 <h2 ALIGN=CENTER> <B>List of jobs crashed in<font color="blue"> $qprod </font> production <br> and <font color="blue">$qtrg </font> dataset  </B></h2>
 <h3 ALIGN=CENTER> Generated on $todate</h3>
<br>
<TABLE ALIGN=CENTER BORDER=5 CELLSPACING=1 CELLPADDING=2 bgcolor=\"#ffdc9f\">
<TR>
<TD ALIGN=CENTER WIDTH=\"30%\" HEIGHT=60><B><h3>Input filename</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>Reco status</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>No.events</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>Date of submission</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"20%\" HEIGHT=60><B><h3>Finish time</h3></B></TD>
</TR>
    </body>
END
}

#####################################

sub beginUnHtml {

print <<END;

  <html>
   <body BGCOLOR=\"cornsilk\">
 <h2 ALIGN=CENTER> <B>List of jobs wth reco status <font color="red">unknown</font> and job status <font color="red">done</font> in<font color="blue"> $qprod </font> production <br> and <font color="blue">$qtrg </font> dataset  </B></h2>
 <h3 ALIGN=CENTER> Generated on $todate</h3>
<br>
<TABLE ALIGN=CENTER BORDER=5 CELLSPACING=1 CELLPADDING=2 bgcolor=\"#ffdc9f\">
<TR>
<TD ALIGN=CENTER WIDTH=\"20%\" HEIGHT=60><B><h3>Input filename</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>Input file size</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>Input file size on site</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>Last job progress state</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"20%\" HEIGHT=60><B><h3>Date of submission</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>Condor job ID</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>Log file status</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>MuDst status</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>Globus error</h3></B></TD>
</TR>
    </body>
END
}


#####################################

sub beginMuHtml {

print <<END;

  <html>
   <body BGCOLOR=\"cornsilk\">
 <h2 ALIGN=CENTER> <B>List of jobs failed to deliver MuDst files for<font color="blue"> $qprod </font> production <br> and <font color="blue">$qtrg </font> dataset  </B></h2>
 <h3 ALIGN=CENTER> Generated on $todate</h3>
<br>
<TABLE ALIGN=CENTER BORDER=5 CELLSPACING=1 CELLPADDING=2 bgcolor=\"#ffdc9f\">
<TR>
<TD ALIGN=CENTER WIDTH=\"30%\" HEIGHT=60><B><h3>Input filename</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>Last job state</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>Reco status</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>No.events</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"20%\" HEIGHT=60><B><h3>Date of submission</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>Globus error</h3></B></TD>
</TR>
    </body>
END
}

#####################################

sub beginLogHtml {

print <<END;

  <html>
   <body BGCOLOR=\"cornsilk\">
 <h2 ALIGN=CENTER> <B>List of jobs failed to deliver log files for<font color="blue"> $qprod </font> production <br> and <font color="blue">$qtrg </font> dataset  </B></h2>
 <h3 ALIGN=CENTER> Generated on $todate</h3>
<br>
<TABLE ALIGN=CENTER BORDER=5 CELLSPACING=1 CELLPADDING=2 bgcolor=\"#ffdc9f\">
<TR>
<TD ALIGN=CENTER WIDTH=\"30%\" HEIGHT=60><B><h3>Input filename</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>Last job progess state</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>Last job state</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"20%\" HEIGHT=60><B><h3>Date of submission</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>Log file status</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>MuDst status</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>Globus error</h3></B></TD>

</TR>
    </body>
END
}

#####################################

sub beginMDHtml {

print <<END;

  <html>
   <body BGCOLOR=\"cornsilk\">
 <h2 ALIGN=CENTER> <B>List of daq files not restored on disk for <font color="blue"> $qprod </font> production <br> and <font color="blue">$qtrg </font> dataset  </B></h2>
 <h3 ALIGN=CENTER> Generated on $todate</h3>
<br>
<TABLE ALIGN=CENTER BORDER=5 CELLSPACING=1 CELLPADDING=2 bgcolor=\"#ffdc9f\">
<TR>
<TD ALIGN=CENTER WIDTH=\"30%\" HEIGHT=60><B><h3>Input filename</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"20%\" HEIGHT=60><B><h3>Date of submission to DC</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>Last job state</h3></B></TD>
</TR>
    </body>
END
}


#####################################

sub beginGLHtml {

print <<END;

  <html>
   <body BGCOLOR=\"cornsilk\">
 <h2 ALIGN=CENTER> <B>List of jobs with globus error<font color="blue"> $qprod </font> production <br> and <font color="blue">$qtrg </font> dataset  </B></h2>
 <h3 ALIGN=CENTER> Generated on $todate</h3>
<br>
<TABLE ALIGN=CENTER BORDER=5 CELLSPACING=1 CELLPADDING=2 bgcolor=\"#ffdc9f\">
<TR>
<TD ALIGN=CENTER WIDTH=\"30%\" HEIGHT=60><B><h3>Input filename</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>Globus error </h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>Date of submission</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>Job progress statement</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>Last job state</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>MuDst status</h3></B></TD>
</TR>
    </body>
END
}


#####################################


sub endHtml {
my $Date = `date`;

print <<END;
</TABLE>
      <h5>
      <address><a href=\"mailto:didenko\@bnl.gov\">Lidia Didenko</a></address>
<!-- Created:  June 10 2014 -->
<!-- hhmts start -->
Last modified: 2014-06-23
<!-- hhmts end -->
  </body>
</html>
END

}



######################
sub StDbProdConnect {
    $dbh = DBI->connect("dbi:mysql:$dbname:$dbhost", $dbuser, $dbpass)
        || die "Cannot connect to db server $DBI::errstr\n";
}

######################
sub StDbProdDisconnect {
    $dbh = $dbh->disconnect() || die "Disconnect failure $DBI::errstr\n";
}

#####################################

sub cgiSetup {
    $q=new CGI;
    if ( exists($ENV{'QUERY_STRING'}) ) { print $q->header };
}












