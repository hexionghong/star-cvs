#!/usr/local/bin/perl
#!/usr/bin/env perl 
#
# dbNewLibQuery.pl
#
################################################################

use CGI;

BEGIN {
 use CGI::Carp qw(fatalsToBrowser carpout);
}


use DBI;
use CGI qw(:standard);
use GD;
use GD::Graph::linespoints;
use Mysql;

$dbhost="duvall.star.bnl.gov";
$dbuser="starreco";
$dbpass="";
$dbname="LibraryJobs";

# Tables

$JobStatusT = "JobStatus";

my $debugOn = 0;
my @data = ();
my @legend = ();
my $prepath = "new";



my @prod_set = (
	  	"trs_sl302/year_2000/hc_standard",
		"trs_sl302/year_2001/hc_standard",
                "trs_sl302/year_2001/pp_minbias",
                "trs_sl302/year_2003/dau_minbias",
                "trs_sl302/year_2004/auau_minbias",
                "trs_sl302/year_2004/auau_central", 
                "trs_sl302/year_2005/cucu200_minbias",
                "trs_sl302/year_2005/cucu62_minbias",
                "trs_sl302/year_2006/pp200_minbias",
                "trs_sl302/year_2007/auau200_central",
                "trs_sl302/year_2008/dau200_minbias",
                "trs_sl302/year_2008/pp200_minbias",
                "trs_sl302/year_2009/pp200_minbias",
                "trs_sl302/year_2009/pp500_minbias",
                "trs_sl302/year_2010/auau200_minbias",
                "daq_sl302/year_2000/minbias",
                "daq_sl302/year_2000/central",
		"daq_sl302/year_2001/minbias",
                "daq_sl302/year_2001/central",
                "daq_sl302/year_2001/ppMinBias",
                "daq_sl302/year_2003/ppMinBias",
                "daq_sl302/year_2003/dAuMinBias",
                "daq_sl302/year_2004/AuAuMinBias",
                "daq_sl302/year_2004/AuAu_prodHigh",
                "daq_sl302/year_2004/AuAu_prodLow",
                "daq_sl302/year_2004/prodPP",
                "daq_sl302/year_2005/CuCu200_HighTower",
                "daq_sl302/year_2005/CuCu200_MinBias",
                "daq_sl302/year_2005/CuCu62_MinBias",
                "daq_sl302/year_2005/CuCu22_MinBias",
                "daq_sl302/year_2005/ppProduction",
                "daq_sl302/year_2006/ppProdLong",
                "daq_sl302/year_2006/ppProdTrans",
                "daq_sl302/year_2007/2007ProductionMinBias",
                "daq_sl302/year_2008/production_dAu2008",
                "daq_sl302/year_2008/ppProduction2008", 
                "daq_sl302/year_2009/production2009_500GeV", 
                "daq_sl302/year_2009/production2009_200Gev_Hi", 
                "daq_sl302/year_2010/auau200_production",
                "daq_sl302/year_2010/auau62_production",
                "daq_sl302/year_2010/auau39_production",
                "daq_sl302/year_2010/auau11_production", 
                "daq_sl302/year_2010/auau7_production"
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
                "Average_NoTracksNfit15",
		"Average_NoPrimaryTNfit15",
                "Average_NoPrimVertex",
                "NoEvent_vertex",                 
                "Average_NoV0Vrt",
		"Average_NoXiVrt",
		"Average_NoKinKVrt",
                "Percent_of_usableEvents",
                "Average_NoTracks_per_usableEvent",
		"Average_NoPrimTrack_per_usableEvent",
                "Average_NoTracksNfit15_per_usableEvent",
		"Average_NoPrimTrackNfit15_per_usableEvent",                 
                "Average_NoV0_per_usableEvent",
		"Average_NoXi_uper_sableEvent",
		"Average_NoKink_per_usableEvent",                 
                
                );   


my %plotHash = (
                MemUsage => 'memUsageF, memUsageL',
                CPU_per_Event => 'CPU_per_evt_sec',
                RealTime_per_Event => 'RealTime_per_evt',
                Average_NoTracks => 'avg_no_tracks',
                Average_NoPrimaryT => 'avg_no_primaryT',
                Average_NoTracksNfit15 => 'avg_no_tracksnfit15',
                Average_NoPrimaryTNfit15  => 'avg_no_primaryTnfit15',
                Average_NoPrimVertex => 'avgNoVtx_evt',
                NoEvent_vertex => 'NoEventVtx',
                Average_NoV0Vrt => 'avg_no_V0Vrt',
                Average_NoXiVrt => 'avg_no_XiVrt',
                Average_NoKinKVrt => 'avg_no_KinKVrt',
                Percent_of_usableEvents => 'percent_of_usable_evt',
                Average_NoTracks_per_usableEvent => 'avgNoTrack_usbevt',
                Average_NoPrimTrack_per_usableEvent => 'avgNoPrTrack_1vtx_usbevt',
                Average_NoTracksNfit15_per_usableEvent => 'avgNoTrackNfit15_usbevt',
                Average_NoPrimTrackNfit15_per_usableEvent => 'avgNoPrTrackNfit15_1vtx_usbevt',
                Average_NoV0_per_usableEvent => 'avgNoV0_usbevt',
                Average_NoXi_per_sableEvent => 'avgNoXi_usbevt',
                Average_NoKink_per_usableEvent => 'avgNoKink_usbevt'
               );


my @plotvaldg = ();
my @plotvalop = ();
my @libtag = ();
my $npt = 0;
my $npk = 0;
my @plotmemfst = ();
my @plotmemlst = ();
my $min_y = 0;
my $max_y = 5000;
my $maxval = 0;
my $minVal = 0;
 

my $query = new CGI;

my $scriptname = $query->url(-relative=>1);

my $tset    =  $query->param('sets');
my $plotVal = $query->param('plotVal');

  if( $tset eq "" and $plotVal eq "" ) {

print $query->header();
print $query->start_html('Plots for Nightly Test in NEW Library');
print <<END;
<META HTTP-EQUIV="Expires" CONTENT="0">
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<META HTTP-EQUIV="Cache-Control" CONTENT="no-cache">
END
print $query->startform(-action=>"$scriptname");  

print "<body bgcolor=\"cornsilk\">\n";
print "<h1 align=center><u>Plots for Nightly Test in NEW Librares</u></h1>\n";

print "<br>";
print "<br>";
print <<END;
<hr>
<table BORDER=0 align=center width=99% cellspacing=3>
<tr ALIGN=center VALIGN=CENTER NOSAVE>
<td>
END

print "<p>";
print "<h3 align=center>Select Test</h3>";
print "<h4 align=center>";
print $query->scrolling_list(-name=>'sets',
			     -values=>\@sets_name,
			     -size=>8);
print "</td><td>";
print "<h3 align=center> Select plot:</h3>";
print "<h4 align=center>";
print $query->scrolling_list(-name=>'plotVal',
			     -values=>\@myplot,
			     -size =>8); 
print "</td> </tr> </table><hr><center>";

print "</h4>";
print "<br>";
print "<br>";
print "<br>";
print $query->submit(),"<p>";
print $query->reset();
print $query->endform();
print "<address><a href=\"mailto:didenko\@bnl.gov\">Lidia Didenko</a></address>\n";

print $query->end_html();

  }else{

my $qqr = new CGI;

my $tset    =  $qqr->param('sets');
my $plotVal =  $qqr->param('plotVal');

$JobStatusT = "JobStatus";

my @spl = ();
@spl = split(" ",$plotVal);
my $plotVl = $spl[0];

my $mplotVal = $plotHash{$plotVl};

  &StDbTJobsConnect();

my $path;
my $pth;
my $tyear  = "2010";

 @spl = split(" ", $tset);
 $pth = $spl[0];

 @spl = ();
 @spl = split("/", $pth);
 $path = $spl[1]."/". $spl[2];

$prepath = "%new/".$spl[0].".ittf";

 my $qupath = "%$prepath%$path%";

@plotvaldg = ();
@plotvalop = ();
@libtag = ();
$npt = 0;
$npk = 0;
@plotmemfsto = ();
@plotmemlsto = ();
@plotmemfstd = ();
@plotmemlstd = ();

$min_y = 0;
$max_y = 5000;
$maxval = 0;
$minVal = 0;


    $sql="SELECT path, $mplotVal, LibTag FROM JobStatus WHERE path LIKE ?  AND jobStatus= 'Done' and LibTag like 'SL10%' and createTime like '$tyear%' ORDER by createTime";

        $cursor = $dbh->prepare($sql) || die "Cannot prepare statement: $dbh->errstr\n";
        $cursor->execute($qupath);

        while(@fields = $cursor->fetchrow_array) {

            if ($fields[0] =~ /sl302.ittf_opt/) {
              if ($plotVal eq "MemUsage") {
                $plotmemfsto[$npt] = $fields[1];
                $plotmemlsto[$npt] = $fields[2];
                if( $plotmemlsto[$npt] >= $maxval) {
		    $maxval =  $plotmemlsto[$npt];
                  }
	        if( $plotmemfsto[$npt] <= $minval ) {
		  $minval =  $plotmemfsto[$npt];
	          }
                $libtag[$npt] = $fields[3];               
                 $npt++;  
	   }else{
		$plotvalop[$npt] = $fields[1];
		if( $plotvalop[$npt] >= $maxval) {
		    $maxval =  $plotvalop[$npt];
                  }
	        if( $plotvalop[$npt] <= $minval ) {
		  $minval =  $plotvalop[$npt];
	          }
                $libtag[$npt] = $fields[2];
                 $npt++;
	       }
	    }else{                
              if ($plotVal eq "MemUsage") {
                $plotmemfstd[$npk] = $fields[1];
                $plotmemlstd[$npk] = $fields[2];
		if( $plotmemlstd[$npk] >= $maxval) {
		    $maxval =  $plotmemlstd[$npk];
                  }
	        if( $plotmemfstd[$npk] <= $minval ) {
		  $minval =  $plotmemfstd[$npk];
	          }
                $libtag[$npk] = $fields[3];
                 $npk++;                 
	   }else{
 		$plotvaldg[$npk] = $fields[1];
                if( $plotvaldg[$npk] >= $maxval) {
		    $maxval =  $plotvaldg[$npk];
                  }
	        if( $plotvaldg[$npk] <= $minval ) {
		  $minval =  $plotvaldg[$npk];
	          }
                $libtag[$npk] = $fields[2];
                $npk++;            
            }
	  }

	}

&StDbTJobsDisconnect();

 $min_y = $minval;
 $max_y = $maxval;

 my $ylabel;
 my $gtitle;

@data = ();


my $graph = new GD::Graph::linespoints(650,500);

 if ( ! $graph){
    print STDOUT $qqr->header(-type => 'text/plain');
    print STDOUT "Failed\n";

 } else {


if ($plotVal eq "MemUsage") {
    @data = (\@libtag, \@plotmemfsto, \@plotmemlsto, \@plotmemfstd, \@plotmemlstd );
    $legend[0] = "MemUsageFirst(optimized)";
    $legend[1] = "MemUsageLast(optimized)";
    $legend[2] = "MemUsageFirst(nonoptimized)";
    $legend[3] = "MemUsageLast(nonoptimized)";

    $mplotVal="MemUsageFirstEvent,MemUsageLastEvent";
} else {
    @data = (\@libtag, \@plotvalop, \@plotvaldg );
    
    $legend[0] = "$plotVal"."(optimized)";
    $legend[1] = "$plotVal"."(nonoptimized)";

}

 my $xLabelsVertical = 1;
 my $xLabelPosition = 0.5;
 my $xLabelSkip = 1;


    if( $min_y == 0) {
        $graph->set(x_label => "(0 value means job failed or data not available)");
    } else {
        $min_y = $min_y - ($max_y-$min_y)*50.0;
    }

   $max_y = $max_y + ($max_y - $min_y)/10.0;

    if($max_y eq $min_y) {
        $max_y += 1;
        $min_y -= 1;
    }

    if($min_y < 0) {
        $min_y = 0;
    }


    $graph->set(#x_label => "$xlabel",
                #y_label => "$mplotVal",
                x_label_position => 0.5,
                title   => "$tset"." ($mplotVal)",
                y_tick_number => 10,
                y_min_value => $min_y,
                y_max_value => $max_y,
                y_number_format => \&y_format,
                labelclr => "lblack",
                dclrs => [ qw(lblack lred lgreen lpink lblue lpurple lorange lyellow ) ],
                line_width => 2,
                markers => [ 2,3,4,5,6,7,8,9],
                marker_size => 2,
                x_label_skip => $xLabelSkip,
                x_labels_vertical =>$xLabelsVertical,
                #long_ticks => 1
                );

   $graph->set_legend(@legend);
    $graph->set_legend_font(gdMediumBoldFont);
    $graph->set_title_font(gdMediumBoldFont);
    $graph->set_x_label_font(gdMediumBoldFont);
    $graph->set_y_label_font(gdMediumBoldFont);
    $graph->set_x_axis_font(gdMediumBoldFont);
    $graph->set_y_axis_font(gdMediumBoldFont);

         if ( scalar(@libtag) <= 1 ) {
            print $qqr->header(-type => 'text/html')."\n";
            &beginHtml();
        } else {
            my $format = $graph->export_format;
            print header("image/$format");
            binmode STDOUT;
    
    print STDOUT $graph->plot(\@data)->$format();

       }
    }
  }

exit 0;

######################
sub StDbTJobsConnect {
    $dbh = DBI->connect("dbi:mysql:$dbname:$dbhost", $dbuser, $dbpass)
        || die "Cannot connect to db server $DBI::errstr\n";
}

######################
sub StDbTJobsDisconnect {
    $dbh = $dbh->disconnect() || die "Disconnect failure $DBI::errstr\n";
}


##########################################################

sub y_format
{
    my $value = shift;
    my $ret;

    $ret = sprintf("%8.2f", $value);
}

#####################################

sub beginHtml {

print <<END;
  <html>
   <head>
          <title>Plots for Nightly Test in NEW Library</title>
   </head>
   <body BGCOLOR=\"#ccffff\">
     <h1 align=center>No Data for $tset and $mplotVal </h1>


    </body>
   </html>
END
}
