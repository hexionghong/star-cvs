#! /opt/star/bin/perl -w
#
# 
#
#  mdc3PlotReq.pl 
#
#
# Interactive box for production plots query
# 
#
#############################################################################


#require "/afs/rhic/star/packages/DEV00/mgr/dbCpProdSetup.pl";
use CGI;
use CGI::Carp qw(fatalsToBrowser);

my $debugOn = 0;

my @prod_set = (
            "auau200/nexus/default/b0_3/year_1h/hadronic_on",
            "auau200/nexus/default/minbias/year_1h/hadronic_on", 
             "auau200/mevsim/vanilla/central/year_1h/hadronic_on",
             "auau200/mevsim/vanilla/flow/year_1h/hadronic_on", 
             "auau200/mevsim/vanilla/resonance/year_1h/hadronic_on", 
             "auau200/mevsim/vanilla/trigger/year_1h/hadronic_on", 
             "auau200/vni/default/b0_3/year_1h/hadronic_on",
             "auau200/hijing/b0_3_jetq_off/jet05/year_1h/hadronic_on",
             "auau200/hijing/b0_3_jetq_on/jet05/year_1h/hadronic_on",
             "auau200/hijing/b8_15_jetq_off/jet05/year_1h/hadronic_on",
             "auau200/hijing/b8_15_jetq_on/jet05/year_1h/hadronic_on", 
             "auau200/hbt/default/peripheral/year_1h/hadronic_on",
             "auau200/hbt/default/midperipheral/year_1h/hadronic_on",
             "auau200/hbt/default/middle/year_1h/hadronic_on",
             "auau200/hbt/default/central/year_1h/hadronic_on",
             "auau200/hbt/default/midcentral/year_1h/hadronic_on",
             "auau200/starlight/2gamma/halffield/year_1h/hadronic_on",
             "auau200/starlight/2gamma/none/year_1h/hadronic_on",
             "auau200/starlight/vmeson/halffield/year_1h/hadronic_on",
             "auau200/starlight/vmeson/none/year_1h/hadronic_on",
             "auau200/dtunuc/two_photon/halffield/year_1h/hadronic_on",
             "auau200/dtunuc/two_photon/none/year_1h/hadronic_on",
             "auau200/hemicosm/default/none/year_1h/hadronic_on",
             "pau200/hijing/b0_7/gam15/year_1h/hadronic_on",
             "pau200/hijing/b0_7/jet15/year_1h/hadronic_on", 
             "auau200/mevsim/vcascade/central/year_1h/hadronic_on",       
             "auau200/mevsim/vcascade/flow/year_1h/hadronic_on", 
             "auau200/mevsim/vcascade/fluct/year_1h/hadronic_on",
             "auau200/mevsim/vcascade/resonance/year_1h/hadronic_on", 
             "auau200/mevsim/vcascade/trigger/year_1h/hadronic_on",
             "auau200/mevsim/cascade/central/year_1h/hadronic_on",
             "auau200/mevsim/vanilla/fluct/year_1h/hadronic_on",
             "auau200/hijing_quark/b0_3_jetq_off/jet05/year_1h/hadronic_on",
             "auau200/hijing_antinuc/b0_3_jetq_off/jet05/year_1h/hadronic_on",
             "auau200/hijing_antinuc/b0_3_jetq_on/jet05/year_1h/hadronic_on",
             "auau200/hijing135/default/b0_3/year_1h/hadronic_on",
             "auau200/strongcp/broken/eb-400_90/year_1h/hadronic_on",
             "auau200/strongcp/broken/eb-400_00/year_1h/hadronic_on",
             "auau200/strongcp/broken/lr_eb-400_90/year_1h/hadronic_on",
             "auau200/venus/default/b3_6/year_1h/hadronic_on",
             "auau200/venus/default/b6_9/year_1h/hadronic_on", 
             "auau200/venus412/default/b0_3/year_1h/hadronic_on",

   );
 
my @sets_name;

my $kk = 1;
 $sets_name[0] = "all";
for( $ll=0; $ll<scalar(@prod_set); $ll++) {
 $sets_name[$kk] = $prod_set[$ll] ;
   $kk++;
}  

                      
my @myplot =   (
                 "Memory_size",
                 "CPU_per_Event",
                 "RealTime_per_Event",
                 "Average_NoTracks",
                 "Average_NoVertices"
                );   

$query = new CGI;

print $query->header;
print $query->start_html('mdc3PlotReq');
print $query->startform(-action=>"GifMdc3Plots.pl");  

  print "<html>\n";
  print " <head>\n";

print <<END;
<META Name="Production plotes" CONTENT="This site demonstrates plots for production operation">
<META HTTP-EQUIV="Expires" CONTENT="0">
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<META HTTP-EQUIV="Cache-Control" CONTENT="no-cache">
<META HTTP-equiv="Refresh" content="600; URL-HTTP://www.star.bnl.gov/devcgi/GifMdc3Plots.pl">
END

  print " <title>Select Query for Production Plots </title>";
  print "  </head>\n";
  print "  <body bgcolor=\"#ffdc9f\"> \n";
  print "  <h1 align=center>Select Query for MDC3 Production Plots </h1>\n";
  print " </head>\n";
  print " <body>";


print <<END;
</SELECT><br>
<p>
<br>
END

print "<p>";
print "<h3 align=center>Select name of dataset:</h3>";
print "<h4 align=center>";
print $query->scrolling_list(-name=>'set1',
                   -values=>\@sets_name,
                   -size=>10); 

 
print <<END;
</SELECT><br>
<p>
END

print "<h3 align=center> Select plot:</h3>";
print "<h4 align=center>";
print $query->scrolling_list(-name=>'plotVal',
                   -values=>\@myplot,
                   -size =>4); 

print <<END;
</SELECT><br>
<p>
<br>
<h3>Enter date of production yyyy-mm-dd:</h3> <input type="text" size=12 name="datProd"><br>
<p>
END


 print "<p>";
 print "<p><br>"; 
 print $query->submit;
 print "<P><br>", $query->reset;
 print $query->endform;
 print "  <address><a href=\"mailto:didenko\@bnl.gov\">Lidia Didenko</a></address>\n";

 print "</body>";
 print "</html>";
  

#=======================================================================

if($query->param) {
  GifMdc3Plots($query);
}
#print $query->delete_all;
print $query->end_html; 






