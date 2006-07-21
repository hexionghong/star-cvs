#!/usr/local/bin/perl
#!/usr/bin/env perl 
#
# $Id: CRSqueueStatus.pl,v 1.5 2006/07/21 17:45:32 didenko Exp $
#
# $Log: CRSqueueStatus.pl,v $
# Revision 1.5  2006/07/21 17:45:32  didenko
# more fixes for injection protection
#
# Revision 1.4  2006/07/06 16:02:46  didenko
# extend period for monitoring
#
# Revision 1.3  2006/01/12 22:11:34  didenko
# updated to use year's table
#
# Revision 1.2  2005/10/28 20:32:15  didenko
# get rid of onr more script
#
# Revision 1.2  2005/10/07 16:54:39  didenko
# updated limit
#
# Revision 1.2  2005/10/06 15:41:10  didenko
# updated
#
#
#
##########################################################


BEGIN {
 use CGI::Carp qw(fatalsToBrowser carpout);
}

use DBI;
use CGI;
use GIFgraph::linespoints;
use GD;
use Mysql;


$dbhost="duvall.star.bnl.gov";
$dbuser="starreco";
$dbpass="";
$dbname="operation";


my @reqperiod = ("day","week","1_month","2_months","3_months","4_months","5_months","6_months","7_months","8_months","9_months","10_months","11_months","12_months");
my @plotview = ("numbers","percentage");
my @prodyear = ("2005","2006");

my $query = new CGI;

my $scriptname = $query->url(-relative=>1);

 my $pryear =  $query->param('ryear');
 my $fperiod  =  $query->param('period');
 my $plview   =  $query->param('plotvw');


 if( $fperiod eq "" and $plview eq "" and $pryear eq "") {


print $query->header;
print $query->start_html('CRS queue status');
print <<END;
<META HTTP-EQUIV="Expires" CONTENT="0">
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<META HTTP-EQUIV="Cache-Control" CONTENT="no-cache">
END
print $query->startform(-action=>"$scriptname");

print "<body bgcolor=\"cornsilk\">\n";
print "<h1 align=center><u>CRS queue status</u></h1>\n";
print "<br>";
print <<END;

<hr>
<table BORDER=0 align=center width=99% cellspacing=3>
<tr ALIGN=center VALIGN=CENTER NOSAVE>
<td>
END

print "<p>";
print "</td><td>";  
print "<h3 align=center> Select year of production</h3>";
print "<h4 align=center>";
print  $query->scrolling_list(-name=>'ryear',
                             -values=>\@prodyear,
                             -default=>2006,
                             -size =>1); 

print "<p>";
print "</td><td>";
print "<h3 align=center> Select period of monitoring</h3>";
print "<h4 align=center>";
print  $query->scrolling_list(-name=>'period',
                             -values=>\@reqperiod,
                             -default=>day,
                             -size =>1); 

print "<p>";
print "</td><td>";
print "<h3 align=center> How do you want to view plots:</h3>";
print "<h4 align=center>";
print  $query->scrolling_list(-name=>'plotvw',
                             -values=>\@plotview,
                             -default=>numbers,
                             -size =>1); 


print "<p>";
print "</td> </tr> </table><hr><center>";

print "</h4>";
print "<br>";
print "<br>";
print "<br>";
print $query->submit,"<p>";
print $query->reset;
print $query->endform;
print "<br>";
print "<br>";
print "<address><a href=\"mailto:didenko\@bnl.gov\">Lidia Didenko</a></address>\n";

print $query->end_html;

  }else{

my $qqr = new CGI;

my $pryear    =  $qqr->param('ryear');
my $fperiod   =  $qqr->param('period');
my $plview    =  $qqr->param('plotvw');

my $dyear = $pryear - 2000 ;


# Tables
$crsJobStatusT = "crsJobStatusY".$dyear;
$crsQueueT = "crsQueueY".$dyear;


my $day_diff = 1;
my $max_y = 10000;
my $min_y = 0;
my @data;
my @legend;
my $ymax = 1;

my @numjobs1 = ();
my @numjobs2 = ();
my @numjobs3 = ();
my @numjobs4 = ();
my @numjobs5 = ();
my @Npoint = ();
my @maxvalue = ();

($sec,$min,$hour,$mday,$mon,$year) = localtime;


if( $mon < 10) { $mon = '0'.$mon };
if( $mday < 10) { $mday = '0'.$mday };
if( $hour < 10) { $hour = '0'.$hour };
if( $min < 10) { $min = '0'.$min };
if( $sec < 10) { $sec = '0'.$sec };


my $nowdate = ($year+1900)."-".($mon+1)."-".$mday;
my $thisyear = $year+1900;


 if( $thisyear eq $pryear) {

 $nowdate = $pryear."-".($mon+1)."-".$mday;

 }else{

 $nowdate = $pryear."-12-31 23:59:59";

}

my $day_diff = 0;
my $nmonth = 0;
my @prt = ();

    if( $fperiod eq "day") {
           $day_diff = 1;
    
    }elsif( $fperiod eq "week") {
           $day_diff = 7;
    }elsif ( $fperiod =~ /month/) {
       @prt = split("_", $fperiod);
       $nmonth = $prt[0];
       $day_diff = 30*$nmonth + 1; 
    }

  $day_diff = int($day_diff);


   &StcrsdbConnect();

 @numjobs1 = ();
 @numjobs2 = ();
 @numjobs3 = ();
 @numjobs4 = ();
 @numjobs5 = ();
 @Npoint = ();
 @maxvalue = ();

    if($plview eq "numbers") {

             $sql="SELECT max(queue0), max(queue1), max(queue2), max(queue3), max(queue4) FROM  $crsQueueT WHERE (TO_DAYS(\"$nowdate\") - TO_DAYS(sdate)) <= ? ";

	$cursor = $dbh->prepare($sql) || die "Cannot prepare statement: $dbh->errstr\n";
	$cursor->execute($day_diff) ;
	while(@fields = $cursor->fetchrow_array) {  

 		$maxvalue[0] = $fields[0];
		$maxvalue[1] = $fields[1];
		$maxvalue[2] = $fields[2];
		$maxvalue[3] = $fields[3]; 
                $maxvalue[4] = $fields[4];   
	    }

 my $ii = 0;

            $sql="SELECT queue0, queue1, queue2, queue3, queue4, sdate FROM  $crsQueueT WHERE (TO_DAYS(\"$nowdate\") - TO_DAYS(sdate)) <= ? ORDER by sdate ";

	$cursor = $dbh->prepare($sql) || die "Cannot prepare statement: $dbh->errstr\n";
	$cursor->execute($day_diff) ;
	while(@fields = $cursor->fetchrow_array) {

		$numjobs1[$ii] = $fields[0];
		$numjobs2[$ii] = $fields[1];
		$numjobs3[$ii] = $fields[2];
		$numjobs4[$ii] = $fields[3];
		$numjobs5[$ii] = $fields[4];
                $Npoint[$ii] =  $fields[5]; 
               	$ii++;
 
      }
    for ($k = 0; $k < scalar(@maxvalue); $k++) {
        if( $ymax <= $maxvalue[$k]) {
     $ymax = $maxvalue[$k];    
       }
    }
  $min_y = 0;
  $max_y = $ymax + 200 ;  
  $ylabel = "Number of jobs in the queues";
  $gtitle = "Number of jobs in the queues for the period of $fperiod ";

##

    }else{

            $sql="SELECT Rqueue0, Rqueue1, Rqueue2, Rqueue3, Rqueue4, sdate FROM  $crsQueueT WHERE (TO_DAYS(\"$nowdate\") - TO_DAYS(sdate)) < ? ORDER by sdate ";

	$cursor = $dbh->prepare($sql) || die "Cannot prepare statement: $dbh->errstr\n";
	$cursor->execute($day_diff) ;
	while(@fields = $cursor->fetchrow_array) {

		$numjobs1[$ii] = $fields[0];
		$numjobs2[$ii] = $fields[1];
		$numjobs3[$ii] = $fields[2];
		$numjobs4[$ii] = $fields[3];
		$numjobs5[$ii] = $fields[4];
                $Npoint[$ii] =  $fields[5]; 
               	$ii++;
 
      }
   $min_y = 0;
  $max_y = 140;       
  $ylabel = "Number of jobs in the queue in %";
  $gtitle = "Number of jobs in the queues in % of available slots for the period of $fperiod ";

    }

    &StcrsdbDisconnect();

    @data = (\@Npoint, \@numjobs1, \@numjobs2, \@numjobs3, \@numjobs4, \@numjobs5 );

    $legend[0] = "Jobs in queue 0'";
    $legend[1] = "Jobs in queue 1";
    $legend[2] = "Jobs in queue 2";
    $legend[3] = "Jobs in queue 3";
    $legend[4] = "Jobs in queue 4"; 


  $graph = new GIFgraph::linespoints(750,650);

if ( ! $graph){
    print STDOUT $query->header(-type => 'text/plain');
    print STDOUT "Failed\n";
} else {
    print STDOUT $query->header(-type => 'image/gif');
    binmode STDOUT;


 my $xLabelsVertical = 1;
 my $xLabelPosition = 0;
 my $xLabelSkip = 1;

$xLabelSkip = 2  if( $fperiod eq "day" );
$xLabelSkip = 12  if( $fperiod eq "week" );
$xLabelSkip = 24 if( $fperiod eq "1_month" );
$xLabelSkip = 48 if( $fperiod eq "2_months" );
$xLabelSkip = 72 if( $fperiod eq "3_months" );
$xLabelSkip = 96 if( $fperiod eq "4_months" );
$xLabelSkip = 120 if( $fperiod eq "5_months" );
$xLabelSkip = 144 if( $fperiod eq "6_months" );
$xLabelSkip = 168 if( $fperiod eq "7_months" );
$xLabelSkip = 192 if( $fperiod eq "8_months" );
$xLabelSkip = 216 if( $fperiod eq "9_months" );
$xLabelSkip = 240 if( $fperiod eq "10_months" );
$xLabelSkip = 264 if( $fperiod eq "11_months" );
$xLabelSkip = 288 if( $fperiod eq "12_months" );
 
    $graph->set(x_label => "  ",
		y_label => $ylabel,
		title   => $gtitle,
		y_tick_number => 10,
                x_label_position => 0.5,
		y_min_value => $min_y,
		y_max_value => $max_y,
		y_number_format => \&y_format,
		#labelclr => "lblack",
                titleclr => "lblack",
		dclrs => [ qw(lblack lblue lred lgreen lpink lpurple lorange lyellow ) ],
		line_width => 2,
		markers => [ 2,3,4,5,6,7,8,9],
		marker_size => 1,
                x_label_skip => $xLabelSkip, 
                x_labels_vertical =>$xLabelsVertical, 		
		);

    $graph->set_legend(@legend);
    $graph->set_legend_font(gdMediumBoldFont);
    $graph->set_title_font(gdLargeFont);
    $graph->set_x_label_font(gdMediumBoldFont);
    $graph->set_y_label_font(gdMediumBoldFont);
    $graph->set_x_axis_font(gdMediumBoldFont);
    $graph->set_y_axis_font(gdMediumBoldFont);
    print STDOUT $graph->plot(\@data);
}

}
####################
sub y_format
{
    my $value = shift;
    my $ret;

    $ret = sprintf("%8.2f", $value);
}
######################
sub StcrsdbConnect {
    $dbh = DBI->connect("dbi:mysql:$dbname:$dbhost", $dbuser, $dbpass)
        || die "Cannot connect to db server $DBI::errstr\n";
}

######################
sub StcrsdbDisconnect {
    $dbh = $dbh->disconnect() || die "Disconnect failure $DBI::errstr\n";
}
