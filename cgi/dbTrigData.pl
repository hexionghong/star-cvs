#! /opt/star/bin/perl -w
#
# 
#
#   
#
# dbTrigData.pl
#
# L.Didenko
#
# Interactive box for production plots query
# 
#############################################################################

require "/afs/rhic/star/packages/scripts/dbCpProdSetup.pl";

use Class::Struct;
use CGI;
use CGI::Carp qw(fatalsToBrowser);

#&cgiSetup();

my $debugOn = 0;

my @prodPer;
my $nprodPer = 0;
my $myprod;

my @detSet = ("all","tpc","svt","rich","tof","ftpc","emc","fpd","pmd");
my @trigSet  = ("all","central","minbias","medium","peripheral","mixed","physics");
my @mfield = ("all","FullField","HalfField","FieldOff");

&StDbProdConnect();

$sql="SELECT DISTINCT prodSeries FROM JobStatus WHERE prodSeries like 'P0%'";

   $cursor =$dbh->prepare($sql)
      || die "Cannot prepare statement: $DBI::errstr\n";
   $cursor->execute;

    while(@fields = $cursor->fetchrow) {
      my $cols=$cursor->{NUM_OF_FIELDS};

    for($i=0;$i<$cols;$i++) {
       my $fvalue=$fields[$i];
       my $fname=$cursor->{NAME}->[$i];
       print "$fname = $fvalue\n" if $debugOn;

     $myprod = $fvalue  if($fname eq 'prodSeries'); 
      }
        $prodPer[$nprodPer] = $myprod;
        $nprodPer++;
      }

&StDbProdDisconnect();

$query = new CGI;

print $query->header;
print $query->start_html('dbTirgData');
print $query->startform(-action=>"dbProdSum.pl");  

  print "<html>\n";
  print " <head>\n";

print <<END;
<META Name="Production plotes" CONTENT="This site demonstrates plots for production operation">
<META HTTP-EQUIV="Expires" CONTENT="0">
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<META HTTP-EQUIV="Cache-Control" CONTENT="no-cache">
END
 
  
  print " <title>Production Query</title>";
  print "  </head>\n";
  print "  <body bgcolor=\"#ccffff\"> \n";
  print "<a href=\"http://www.star.bnl.gov/STARAFS/comp/prod\"><h5>Production </h5></a>\n";
  print "  <h1 align=center>Production Query </h1>\n";
  print " </head>\n";
  print " <body>";


print <<END;
<hr>
<table BORDER=0 align=center width=99% cellspacing=3>
<tr ALIGN=center VALIGN=CENTER NOSAVE>
<td>
END

print "</td><td>";
print "<h3 align=center>Production series:</h3>";
print "<h3 align=center>";
print $query->popup_menu(-name=>'SetP',  
                   -values=>\@prodPer,
                   -default=>'P01hf',                   
                   -size=>4                              
                   );                                  
 
 print "</h3>";
 print "</td><td>";
 print "<h3 align=center>Trigger:</h3>";
 print "<h3 align=center>";
 print $query->popup_menu(-name=>'SetT',
                    -values=>\@trigSet,
                    -default=>'all', 
                    -size=>4
                    ); 

 print "</h3>";
 print "</td><td>";
 print "<h3 align=center>Magnetic Field:</h3>";
 print "<h3 align=center>";
 print $query->popup_menu(-name=>'SetF',
                    -values=>\@mfield,
                    -default=>'all', 
                    -size=>4
                    ); 
 print "</h3>";
 print "</td><td>";
 print "<h3 align=center>Detectors:</h3>";
 print "<h3 align=center>";
 print $query->popup_menu(-name=>'SetD',
                    -values=>\@detSet,
                    -default=>'all', 
                    -size=>4
                    ); 

print "</h3>";
print "</td> </table><hr><center>";

 print "<p>";
 print "<p><br><br>"; 
 print $query->submit;
 print "<P><br>", $query->reset;
 print $query->endform;
 print "  <address><a href=\"mailto:didenko\@bnl.gov\">Lidia Didenko</a></address>\n";

 print "</body>";
 print "</html>";
  

#=======================================================================

if($query->param) {
  dbProdSum($query);
}
print $query->end_html; 







