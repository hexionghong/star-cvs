#!/usr/bin/env perl 
#
# $Id: dbDevTestQuery.pl,v 1.14 2004/02/20 15:27:26 didenko Exp $
#
# $Log: dbDevTestQuery.pl,v $
# Revision 1.14  2004/02/20 15:27:26  didenko
# updated for 2004 run test
#
# Revision 1.13  2004/02/16 04:13:49  jeromel
# Small modifs (modules would need to be also installed in OPTSTAR)
#
# Revision 1.12  2003/05/22 18:59:18  didenko
# updated test directories for year2003
#
# Revision 1.11  2002/04/12 19:32:39  didenko
# updated redhat72
#
# Revision 1.10  2002/01/30 15:08:54  didenko
# add new daq test
#
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

BEGIN {
 use CGI::Carp qw(fatalsToBrowser carpout);
}

my $debugOn = 0;

my $rand = rand(100);
my @prod_set = (
#		"trs_redhat72/year_1h/hc_lowdensity",
		"trs_redhat72/year_1h/hc_standard",
#		"trs_redhat72/year_1h/hc_highdensity",
#		"trs_redhat72/year_1h/peripheral", 
		"trs_redhat72/year_2001/hc_lowdensity",
		"trs_redhat72/year_2001/hc_standard",
		"trs_redhat72/year_2001/hc_highdensity",
                "trs_redhat72/year_2001/pp_minbias",
                "trs_redhat72/year_2003/dau_minbias",
		"daq_redhat72/year_2001/minbias",
                "daq_redhat72/year_2001/central",
                "daq_redhat72/year_2001/ppMinBias",
                "daq_redhat72/year_2003/ppMinBias",
                "daq_redhat72/year_2003/dAuMinBias",
                "daq_redhat72/year_2004/AuAuMinBias",
                "daq_redhat72/year_2004/AuAu_prodHigh",
                "daq_redhat72/year_2004/AuAu_prodLow",
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

print "<body bgcolor=\"cornsilk\">\n";
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
print "<address><a href=\"mailto:liuzx at bnl.gov\">Zhixu Liu</a></address>\n";

#print $query->delete_all;
print $query->end_html;
exit 0;
