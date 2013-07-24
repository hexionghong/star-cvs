#!/usr/local/bin/perl
#
# 
#
# L.Didenko
#
# RetriveNodes.pl
#
# Retrive list of nodes and size of  data productions stored on these nodes from FileCatalog.
# 
################################################################################################

BEGIN {
 use CGI::Carp qw(fatalsToBrowser carpout);
}

use CGI qw(:standard);
use DBI;
use lib "/afs/rhic.bnl.gov/star/packages/scripts";
use FileCatalog;


($sec,$min,$hour,$mday,$mon,$year) = localtime();

my $mon =  $mon + 1;

if( $mon < 10) { $mon = '0'.$mon };
if( $mday < 10) { $mday = '0'.$mday };
if( $hour < 10) { $hour = '0'.$hour };
if( $min < 10) { $min = '0'.$min };
if( $sec < 10) { $sec = '0'.$sec };

my $todate = ($year+1900)."-".$mon."-".$mday." ".$hour.":".$min.":".$sec;

my $SITE         = "BNL";
my $status       = (0==1);

my $fileC = new FileCatalog();

    $fileC->connect_as($SITE."::User","FC_user") || die "Connection failed for FC_user\n";

my @sumsize = ();
my @datasize = ();
my @nodelst = (); 
my @arnodes = ();
my @sumevents = ();
my @filelst = ();
my @numfiles = ();
my $strline = 'none';

my $nlist = 0;
my $ssize = 0;
my $dsize = 0;
my @prt = ();


my $query = new CGI;

 if ( exists($ENV{'QUERY_STRING'}) ) { print $query->header };

my $qtrg = $query->param('trigs');
my $qprod = $query->param('prod');

&beginHtml();

 $fileC->set_context("trgsetupname=$qtrg","production=$qprod","filetype=daq_reco_MuDst","storage=local","limit=0");

 @arnodes = $fileC->run_query("grp(node)","sum(size)","sum(events)");

 $fileC->clear_context( );

     foreach $strline (@arnodes){

     @prt = ();
     @prt = split("::",$strline);
     $nodelst[$nlist] = $prt[0]; 
     $datasize[$nlist] = $prt[1]; 
     $dsize = $datasize[$nlist]/1000000000. ;
     $sumsize[$nlist] = sprintf("%.2f", $dsize);
     $sumevents[$nlist] = $prt[2]; 

 $fileC->set_context("trgsetupname=$qtrg","production=$qprod","node=$nodelst[$nlist]","filetype=daq_reco_MuDst","storage=local","limit=0");

   @filelst = $fileC->run_query(filename);

   $fileC->clear_context( );

   $numfiles[$nlist] = scalar(@filelst);

 print <<END;

<TR ALIGN=CENTER HEIGHT=10 bgcolor=\"#ffdc9f\">
<td HEIGHT=5><h3>$nodelst[$nlist]</h3></td>
<td HEIGHT=5><h3>$sumsize[$nlist]</h3></td>
<td HEIGHT=5><h3>$sumevents[$nlist]</h3></td>
<td HEIGHT=5><h3>$numfiles[$nlist]</h3></td>
</TR>
END
      $nlist++;

 }
 
   $fileC->destroy();

 &endHtml();

######################

sub beginHtml {

print <<END;

  <html>
   <body BGCOLOR=\"cornsilk\"> 
 <h2 ALIGN=CENTER> <B> List of nodes and <font color="blue">$qprod </font> production summary on distributed disk <br>for <font color="blue">$qtrg </font> dataset </B></h2>
 <h3 ALIGN=CENTER> Created on $todate</h3>
<br>
<TABLE ALIGN=CENTER BORDER=2 CELLSPACING=1 CELLPADDING=1 >
<TR>
<TD ALIGN=CENTER WIDTH=\"40%\" HEIGHT=40><B><h3>Node name</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"20%\" HEIGHT=40><B><h3>Size of MuDst (GB)</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"20%\" HEIGHT=40><B><h3>Number of events </h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"20%\" HEIGHT=40><B><h3>Number of MuDst files </h3></B></TD>
</TR> 
    </body>
</html>
END
}

#####################
sub endHtml {
my $Date = `date`;

print <<END;
</TABLE>
      <h5>
      <address><a href=\"mailto:didenko\@bnl.gov\">Lidia Didenko</a></address>
<!-- Created: Wed July 26  05:29:25 MET 2000 -->
<!-- hhmts start -->
Last modified: $Date
<!-- hhmts end -->
  </body>
</html>
END

}













