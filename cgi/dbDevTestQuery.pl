#!/opt/star/bin/perl -w
#
# $Id: dbDevTestQuery.pl,v 1.8 2001/08/21 17:03:34 didenko Exp $
#
# $Log: dbDevTestQuery.pl,v $
# Revision 1.8  2001/08/21 17:03:34  didenko
# update daq directory
#
# Revision 1.7  2001/07/10 20:14:36  didenko
# updated dataset
#
# Revision 1.6  2001/04/02 18:02:02  didenko
# update directories
#
# Revision 1.5  2001/02/23 00:37:48  liuzx
# .Add a random number as the action's parameter!
#
# Revision 1.4  2001/02/16 15:37:54  liuzx
# .Add select for weeks,(default 1, max 4)
#
# Revision 1.3  2001/02/15 18:13:15  liuzx
# Header Error modified!
#
# Revision 1.2  2001/02/14 17:02:58  liuzx
# Form->action error modified!
#
# Revision 1.1  2001/02/14 16:59:37  liuzx
# Initial Version: query for nightly test in DEV library.
#                   (currently only last five days)
#
#
################################################################

use CGI;
use CGI::Carp qw(fatalsToBrowser);

my $debugOn = 0;

my $rand = rand(100);
my @prod_set = (
		"trs_redhat61/year_1h/hc_lowdensity",
		"trs_redhat61/year_1h/hc_standard",
		"trs_redhat61/year_1h/hc_highdensity",
		"trs_redhat61/year_1h/peripheral", 
		"trs_redhat61/year_2001/hc_lowdensity",
		"trs_redhat61/year_2001/hc_standard",
		"trs_redhat61/year_2001/hc_highdensity",
                "trs_redhat61/year_2001/pp_minbias",
                "trs_redhat61/year_2001/ppl_minbias",
		"daq_redhat61/year_2001/minbias",
#		"daq_redhat61/year_1h/central",
		);

my @sets_name;

my $kk = 0;
$sets_name[0] = "all";
for( $ll=0; $ll<scalar(@prod_set); $ll++) {
    $sets_name[$kk] = $prod_set[$ll] ;
    $kk++;
}  

my @myplot =   (
		"MemUsage",
                "CPU_per_Event",
		"RealTime_per_Event",
                "Average_NoTracks",
		"Average_NoPrimaryT",
                "Average_NoV0Vrt",
		"Average_NoXiVrt",
		"Average_NoKinKVrt"
                );   

$query = new CGI;

print $query->header;
print $query->start_html('Query for Nightly Test in DEV Library');
print <<END;
<META HTTP-EQUIV="Expires" CONTENT="0">
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<META HTTP-EQUIV="Cache-Control" CONTENT="no-cache">
END
print $query->startform(-action=>"dbDevTestQueryPlot.pl?rand=$rand");  

print "<body bgcolor=\"#ffdc9f\">\n";
print "<h1 align=center><u>Query for Nightly Test in DEV Library</u></h1>\n";

print <<END;
<hr>
<table BORDER=0 align=center width=99% cellspacing=3>
<tr ALIGN=center VALIGN=CENTER NOSAVE>
<td>
END

print "<p>";
print "<h3 align=center>Select Test</h3>";
print "<h4 align=center>";
print $query->scrolling_list(-name=>'set1',
			     -values=>\@sets_name,
			     -size=>9);
print "</td><td>";
print "<h3 align=center> Select plot:</h3>";
print "<h4 align=center>";
print $query->scrolling_list(-name=>'plotVal',
			     -values=>\@myplot,
			     -size =>8); 
print "</td> </tr> </table><hr><center>";

print "<h4 align=center> How many weeks do you want to show: ";
print $query->popup_menu(-name=>'weeks',
			 -values=>['1','2','3','4'],
			 -defaults=>1);
print "</h4>";

print $query->submit,"<p>";
print $query->reset;
print $query->endform;
print "<address><a href=\"mailto:liuzx\@bnl.gov\">Zhixu Liu</a></address>\n";

#print $query->delete_all;
print $query->end_html;
exit 0;
