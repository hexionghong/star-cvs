#!/usr/local/bin/perl
#
# 
#
# L.Didenko
#
# RetriveStreams.pl
#
# Retrive stream data productions from FileCatalog.
# 
################################################################################################

BEGIN {
 use CGI::Carp qw(fatalsToBrowser carpout);
}

use CGI;
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

my @sumevt = ();
my @runevents = ();
my @sumsize = ();
my @datasize = ();
my @filelst = (); 
my $strline = 'none';
my @strname = ();
my @streamname = ();
my $strmain;

my $nlist = 0;
my $ssize = 0;
my $dsize = 0;
my @numfiles = ();

my @arstream = ();
my @prt = ();


my $query = new CGI;

my $qtrg = $query->param('trigs');
my $qprod = $query->param('prod');

$arstream[0] = 0;

&beginHtml();

 $fileC->set_context("trgsetupname=$qtrg","production=$qprod","filetype=daq_reco_MuDst","storage=hpss");

 @arstream = $fileC->run_query(sname2);

 $fileC->clear_context( );

     foreach $strline (@arstream){

$fileC->set_context("trgsetupname=$qtrg","production=$qprod","sname2=$strline","filetype=daq_reco_MuDst","sanity=1","storage=hpss","limit=0");

     $strname[$nlist] = $strline;
     @prt = ();
     @prt = split("_",$strline);
     $streamname[$nlist] = $prt[1]; 
    
     @runevents = ();
     $runevents[0] = 0;  
     @datasize = ();
     $datasize[0] = 0; 
     @filelst = ();  

   @runevents = $fileC->run_query("sum(events)");
   @datasize = $fileC->run_query("sum(size)");
   @filelst = $fileC->run_query(filename);

   $fileC->clear_context( );

   $sumevt[$nlist] = $runevents[0];
   $sumsize[$nlist] = int($datasize[0]/1000000000);
   $dsize = $sumsize[$nlist];

   if($sumsize[$nlist] < 1 ) {
   $ssize = int($datasize[0]/1000000);
   $sumsize[$nlist] = "0.".$ssize;
  }elsif($sumsize[$nlist] < 10 ) {
   $ssize = int($datasize[0]/1000000) - $dsize*1000;
   $sumsize[$nlist] = $dsize.".".$ssize;

    }else{
   $sumsize[$nlist] = int($datasize[0]/1000000000 + 0.5);
    }

   $numfiles[$nlist] = scalar(@filelst);


  if($strname[$nlist] =~ /_adc/ )  {

      $strmain = "st_".$streamname[$nlist];

 print <<END;

<TR ALIGN=CENTER HEIGHT=20 bgcolor=\"#ffdc9f\">
<td HEIGHT=10><h3>$strname[$nlist]</h3></td>
<td HEIGHT=10><h3>$sumevt[$nlist]</h3></td>
<td HEIGHT=10><h3>$sumsize[$nlist]</h3></td>
<td HEIGHT=10><h3>$numfiles[$nlist]</h3></td>
<td HEIGHT=10>added to <B>$strmain</B></td>
</TR>
END

    }else{

 print <<END;

<TR ALIGN=CENTER HEIGHT=20 bgcolor=\"#ffdc9f\">
<td HEIGHT=10><h3>$strname[$nlist]</h3></td>
<td HEIGHT=10><h3>$sumevt[$nlist]</h3></td>
<td HEIGHT=10><h3>$sumsize[$nlist]</h3></td>
<td HEIGHT=10><h3>$numfiles[$nlist]</h3></td>
<td HEIGHT=10><h3><a href="http://www.star.bnl.gov/devcgi/RetriveTrigID.pl?rtrig=$qtrg;rprod=$qprod;rstream=$streamname[$nlist]">triggerID</h3></td>
</TR>
END
#      $nlist++;

    }
      $nlist++;
 }
 
   $fileC->destroy();

 &endHtml();

######################

sub beginHtml {

print <<END;

  <html>
   <body BGCOLOR=\"cornsilk\"> 
 <h2 ALIGN=CENTER> <B> <font color="blue">$qprod </font>production summary for <font color="blue">$qtrg </font>stream data </B></h2>
 <h3 ALIGN=CENTER> Generated on $todate</h3>
<br>
<TABLE ALIGN=CENTER BORDER=5 CELLSPACING=1 CELLPADDING=2 >
<TR>
<TD ALIGN=CENTER WIDTH=\"40%\" HEIGHT=60><B><h3>Stream name</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"20%\" HEIGHT=60><B><h3>Number of Events</h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"20%\" HEIGHT=60><B><h3>Size (GB) of MuDst </h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"20%\" HEIGHT=60><B><h3>Number of MuDst files </h3></B></TD>
<TD ALIGN=CENTER WIDTH=\"20%\" HEIGHT=60><B><h3>TriggerID's summary </h3></B></TD>
</TR> 
   </head>
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
<!-- Created: Wed July 26  05:29:25 MET 2000 -->
<!-- hhmts start -->
Last modified: $Date
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












