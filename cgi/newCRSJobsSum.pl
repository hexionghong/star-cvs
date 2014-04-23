#!/usr/bin/env perl 
#
#  newCRSJobSum.pl
#
# L. Didenko 
#
# newCRSJobSum.pl - script to get summary for CRS jobs id, trigset, production tag, runnumbers,
# input file names and streams
# 
#########################################################################################################


BEGIN {
 use CGI::Carp qw(fatalsToBrowser carpout);
}

use DBI;
use CGI qw(:standard);
use Time::Local;


$dbhost="duvall.star.bnl.gov";
$dbuser="starreco";
$dbpass="";
$dbname="operation";


my ($sec,$min,$hour,$mday,$mon,$year) = localtime();

$mon++;
if( $mon < 10) { $mon = '0'.$mon };
if( $mday < 10) { $mday = '0'.$mday };
if( $hour < 10) { $hour = '0'.$hour };
if( $min < 10) { $min = '0'.$min };
if( $sec < 10) { $sec = '0'.$sec };


my $date_hour = ($year+1900)."-".$mon."-".$mday ." ".$hour;

my $nowdate = ($year+1900)."-".$mon."-".$mday ." ".$hour.":".$min.":".$sec ;

my $JobStatusT = "CRSJobsInfo";  

 my $maxdate = "0000-00-00 00";

 my @jbstate  = ();
 my @prodtags = ();
 my @jbtrigs = ();
 my @jbId = ();
 my @runId = ();
 my $nj = 0;
 my @njbfile = ();
 my @nstream = ();


 &cgiSetup();

my $query = new CGI;

 &StDbConnect();

    $sql="SELECT DISTINCT date_format(max(runDate), '%Y-%m-%d %H') as PDATE FROM $JobStatusT  ";

    $cursor =$dbh->prepare($sql)
      || die "Cannot prepare statement: $DBI::errstr\n";
    $cursor->execute();

    while( $mpr = $cursor->fetchrow() ) {
          $maxdate = $mpr;
       }
    $cursor->finish();


    $sql="SELECT DISTINCT status, prodtag, trigset, runnumber  FROM $JobStatusT where runDate like '$maxdate%' order by runnumber ";

    $cursor =$dbh->prepare($sql)
      || die "Cannot prepare statement: $DBI::errstr\n";
    $cursor->execute();

    while( @fields = $cursor->fetchrow() ) {

       $jbstate[$nj]  =  $fields[0];
       $prodtags[$nj] =  $fields[1];
       $jbtrigs[$nj]  =  $fields[2];
       $runId[$nj]    =  $fields[3];
       $nj++;

       }
    $cursor->finish();

  for ( my $ik = 0; $ik < $nj; $ik++ ) {  

##############   find N files


   $sql="SELECT count(filename)  FROM $JobStatusT where runnumber = '$runId[$ik]' and runDate like '$maxdate%' ";

      $cursor =$dbh->prepare($sql)
          || die "Cannot prepare statement: $DBI::errstr\n";
       $cursor->execute();

       while( my $mpr = $cursor->fetchrow() ) {
          $njbfile[$ik] = $mpr;
       }
    $cursor->finish();

###############   find N streams

 
  $sql="SELECT count(distinct stream)  FROM $JobStatusT where runnumber = '$runId[$ik]'and runDate like '$maxdate%' ";

      $cursor =$dbh->prepare($sql)
          || die "Cannot prepare statement: $DBI::errstr\n";
       $cursor->execute();

       while( my $mpr = $cursor->fetchrow() ) {
          $nstream[$ik] = $mpr;
       }
    $cursor->finish();

 }

  for ( my $ii = 0; $ii < $nj; $ii++ ) {  

      if($jbstate[$ii] eq "QUEUED" )  {

print <<END;

<TR ALIGN=CENTER HEIGHT=10 bgcolor=\"lightblue\">
<td HEIGHT=10>$jbId[$ii]</td>
<td HEIGHT=10>$jbstate[$ii]</td>
<td HEIGHT=10>$jbtrigs[$ii]</td>
<td HEIGHT=10>$prodtags[$ii]</td>
<td HEIGHT=10>$runId[$ii]</td>
<td HEIGHT=10>$njbfile[$ii]</td>
<td HEIGHT=10>$nstream[$ii]</td>
<td HEIGHT=10>$maxdate</td>
</TR>
END

      }elsif($jbstate[$ii] eq "STAGING" ) {

print <<END;

<TR ALIGN=CENTER HEIGHT=10 bgcolor=\"lightgreen\">
<td HEIGHT=10>$jbId[$ii]</td>
<td HEIGHT=10>$jbstate[$ii]</td>
<td HEIGHT=10>$jbtrigs[$ii]</td>
<td HEIGHT=10>$prodtags[$ii]</td>
<td HEIGHT=10>$runId[$ii]</td>
<td HEIGHT=10>$njbfile[$ii]</td>
<td HEIGHT=10>$nstream[$ii]</td>
<td HEIGHT=10>$maxdate</td>
</TR>
END

      }elsif($jbstate[$ii] eq "SUBMITTED" ) {

print <<END;

<TR ALIGN=CENTER HEIGHT=10 bgcolor=\"#ffdc9f\">
<td HEIGHT=10>$jbId[$ii]</td>
<td HEIGHT=10>$jbstate[$ii]</td>
<td HEIGHT=10>$jbtrigs[$ii]</td>
<td HEIGHT=10>$prodtags[$ii]</td>
<td HEIGHT=10>$runId[$ii]</td>
<td HEIGHT=10>$njbfile[$ii]</td>
<td HEIGHT=10>$nstream[$ii]</td>
<td HEIGHT=10>$maxdate</td>
</TR>
END

      }elsif($jbstate[$ii] eq "IMPORTING" ) {

print <<END;

<TR ALIGN=CENTER HEIGHT=10 bgcolor=\"pink\">
<td HEIGHT=10>$jbId[$ii]</td>
<td HEIGHT=10>$jbstate[$ii]</td>
<td HEIGHT=10>$jbtrigs[$ii]</td>
<td HEIGHT=10>$prodtags[$ii]</td>
<td HEIGHT=10>$runId[$ii]</td>
<td HEIGHT=10>$njbfile[$ii]</td>
<td HEIGHT=10>$nstream[$ii]</td>
<td HEIGHT=10>$maxdate</td>
</TR>
END


      }elsif($jbstate[$ii] eq "RUNNING" ) {

print <<END;

<TR ALIGN=CENTER HEIGHT=10 bgcolor=\"cornsilk\">
<td HEIGHT=10>$jbId[$ii]</td>
<td HEIGHT=10>$jbstate[$ii]</td>
<td HEIGHT=10>$jbtrigs[$ii]</td>
<td HEIGHT=10>$prodtags[$ii]</td>
<td HEIGHT=10>$runId[$ii]</td>
<td HEIGHT=10>$njbfile[$ii]</td>
<td HEIGHT=10>$nstream[$ii]</td>
<td HEIGHT=10>$maxdate</td>
</TR>
END

      }elsif($jbstate[$ii] eq "ERROR" or $jbstate[$ii] eq "HELD" ) {

print <<END;

<TR ALIGN=CENTER HEIGHT=10 bgcolor=\"#D8BFD8\">
<td HEIGHT=10>$jbId[$ii]</td>
<td HEIGHT=10>$jbstate[$ii]</td>
<td HEIGHT=10>$jbtrigs[$ii]</td>
<td HEIGHT=10>$prodtags[$ii]</td>
<td HEIGHT=10>$runId[$ii]</td>
<td HEIGHT=10>$njbfile[$ii]</td>
<td HEIGHT=10>$nstream[$ii]</td>
<td HEIGHT=10>$maxdate</td>
</TR>
END

  }
 }

 &StDbDisconnect();


 &endHtml();


######################
sub StDbConnect {
    $dbh = DBI->connect("dbi:mysql:$dbname:$dbhost", $dbuser, $dbpass)
        || die "Cannot connect to db server $DBI::errstr\n";
}

######################
sub StDbDisconnect {
    $dbh = $dbh->disconnect() || die "Disconnect failure $DBI::errstr\n";
}

#####################################

sub beginHtml {

print <<END;

  <html>

    <head>
          <title>CRS Jobs Info</title>
    </head>

   <body BGCOLOR=\"cornsilk\">
 <h2 ALIGN=CENTER> <B>Jobs Info currently running on the CRS farm</h2>
 <h3 ALIGN=CENTER> Generated on $nowdate</h3>
<br>
<TABLE ALIGN=CENTER BORDER=4 CELLSPACING=1 CELLPADDING=1 bgcolor=\"#ffdc9f\">
<TR>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>CRS JobID</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>Status</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"30%\" HEIGHT=60><B><h3>Trigger set name</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>Prod Tag</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>Run number</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>Number of files</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>Number of streams</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=60><B><h3>Date of scan</h3></B></TD>
</TR>
    </body>
END
}


#####################

sub endHtml {
my $Date = `date`;

print <<END;
</TABLE>
      <h5>
      <address><a href=\"mailto:didenko\@bnl.gov\">Lidia Didenko</a></address>
<!-- Created: April 24 2014 -->
<!-- hhmts start -->
Last modified: 2014-04-24
<!-- hhmts end -->
  </body>
</html>
END

}

##############
sub cgiSetup {
    $q=new CGI;
    if ( exists($ENV{'QUERY_STRING'}) ) { print $q->header };
}

