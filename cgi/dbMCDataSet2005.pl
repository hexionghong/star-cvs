#!/usr/local/bin/perl -w
#
# dbMCDatSet2004.pl
#
################################################################

use CGI;
use CGI::Carp qw(fatalsToBrowser);

my $debugOn = 0;

my @collision = ( "all","CuCu62","CuCu200");
my @evtGen  = ("all","hijing", "pythia");

my @geoYear = ("all", "y2005x","y2006");

my @ftype = ("fzd","root");

$query = new CGI;

print $query->header;
print $query->start_html('dbMCDataSet2005');
print <<END;
<META HTTP-EQUIV="Expires" CONTENT="0">
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<META HTTP-EQUIV="Cache-Control" CONTENT="no-cache">
END
print $query->startform(-action=>"dbMCList2005.pl");  

print "<body bgcolor=\"#ffdc9f\">\n";
print "<h1 align=center>Query for MC Datasets</h1>\n";

print <<END;
<hr>
<table BORDER=0 align=center width=99% cellspacing=3>
<tr ALIGN=center VALIGN=CENTER NOSAVE>
<td>
END

print "<p>";
print "<h3 align=center>Select Collision:</h3>";
print "<h4 align=center>";
print $query->scrolling_list(-name=>'setC',
			     -values=>\@collision,
                             -default=>'all', 
			     -size=>2);
                              
print "</td><td>";
print "<h3 align=center> Select Event Generator:</h3>";
print "<h4 align=center>";
print $query->scrolling_list(-name=>'evGen',
			     -values=>\@evtGen,
                             -default=>'all',   
			     -size =>2); 
print "</td><td>";
print "<h3 align=center> Select Geometry:</h3>";
print "<h4 align=center>";
print $query->scrolling_list(-name=>'gYear',
			     -values=>\@geoYear,
                             -default=>'all', 
			     -size =>2); 

print "</td> </tr> </table><hr><center>";

print "<p>";
print "<br>";
print "<br>";
print "<br>";
print "<br>";
print $query->submit,"<p>";
print $query->reset;
print $query->endform;
print "<address><a href=\"mailto:didenko\@bnl.gov\">Lidia Didenko</a></address>\n";

#print $query->delete_all;
print $query->end_html;
