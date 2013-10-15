#!/usr/local/bin/perl
#!/usr/bin/env perl 
#
#  newCRSstatusPlots.pl
#
#  newCRSstatusPlots.pl
#
# L.Didenko
#
# script to monitor on the Web CRS jobs state with new software
#
##########################################################


BEGIN {
 use CGI::Carp qw(fatalsToBrowser carpout);
}

use CGI qw(:standard);
use GD;
use GD::Graph::linespoints;
use DBI;

$dbhost="duvall.star.bnl.gov";
$dbuser="starreco";
$dbpass="";
$dbname="operation";

my $crsJobStatusT = "newcrsJobState";

my $query = new CGI;

# if ( exists($ENV{'QUERY_STRING'}) ) { print $query->header };

my $scriptname = $query->url(-relative=>1);

my @farmstat = ("created","submitted","queued","staging","importing","running","exporting","done","error","killed","held");

my @reqperiod = ("day","week","1_month","2_months","3_months","4_months","5_months","6_months","7_month","8_months","9_months","10_months","11_months","12_months");


my $day_diff = 0;
my $max_y = 10000;
my $min_y = 0;
my @data = ();
my @legend = ();
my $maxvalue = 10000;

my @numjobs = ();
my @Npoint = ();


 my $pryear    =  $query->param('ryear');
 my $fstatus   =  $query->param('jstatus');
 my $fperiod   =  $query->param('period');

 my @prodyear = ("2013","2014");

  if( $fperiod eq "" and $fstatus eq "" and $pryear eq "" ) {

print $query->header;
print $query->start_html('CRS jobs state');
print <<END;
<META HTTP-EQUIV="Expires" CONTENT="0">
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<META HTTP-EQUIV="Cache-Control" CONTENT="no-cache">
END
print $query->startform(-action=>"$scriptname");  

print "<body bgcolor=\"cornsilk\">\n";
print "<h1 align=center><u>CRS jobs status </u></h1>\n";

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
                             -default=>2013,
                             -size =>1); 

print "<p>";
print "</td><td>"; 
print "<h3 align=center>Select jobs state</h3>";
print "<h4 align=center>";
print $query->scrolling_list(-name=>'jstatus',
                             -values=>\@farmstat,
                             -default=>running,
                             -size=>1);
print "</td><td>";
print "<h3 align=center> Select period of monitoring</h3>";
print "<h4 align=center>";
print $query->scrolling_list(-name=>'period',
                             -values=>\@reqperiod,
                             -default=>day,
                             -size =>1); 

print "</td> </tr> </table><hr><center>";

print "</h4>";
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
 my $qstatus   =  $qqr->param('jstatus');
 my $fperiod   =  $qqr->param('period'); 

# my $dyear = $pryear - 2000 ;

my $dyear = $pryear;

# Tables
#$crsJobStatusT = "newcrsJobState".$dyear;

$crsJobStatusT = "newcrsJobState";

 $day_diff = 0;
 $max_y = 10000;
 $min_y = 0;
 @data = ();
 @legend = ();
 $maxvalue = 10000;

 @numjobs = ();
 @Npoint = ();

my $jbstatus;

  $jbstatus = (split(" ",$qstatus))[0];

($sec,$min,$hour,$mday,$mon,$year) = localtime;


if( $mon < 10) { $mon = '0'.$mon };
if( $mday < 10) { $mday = '0'.$mday };
if( $hour < 10) { $hour = '0'.$hour };
if( $min < 10) { $min = '0'.$min };
if( $sec < 10) { $sec = '0'.$sec };


my $nowdate = ($year+1900)."-".($mon+1)."-".$mday;
my $thisyear = $year+1900;
my $nowdatetime ;

if( $thisyear eq $pryear) {

 $nowdate = $thisyear."-".($mon+1)."-".$mday;
 $nowdatetime = $thisyear."-".($mon+1)."-".$mday." ".$hour.":".$min.":59" ;

 }else{

 $nowdate = $pryear."-12-31 23:59:59";
 $nowdatetime = $nowdate;
}

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

	   my $ii = 0;

 @numjobs = ();
 @Npoint = ();


        $sql="SELECT max($jbstatus) FROM  $crsJobStatusT WHERE (TO_DAYS(\"$nowdate\") - TO_DAYS(sdate)) <= ? ";

	$cursor = $dbh->prepare($sql) || die "Cannot prepare statement: $dbh->errstr\n";
	$cursor->execute($day_diff) ;
	while(@fields = $cursor->fetchrow_array) {

             $maxvalue =  $fields[0];
	 }


            $sql="SELECT $jbstatus, sdate FROM  $crsJobStatusT WHERE (TO_DAYS(\"$nowdate\") - TO_DAYS(sdate)) <= ?  and sdate <= '$nowdatetime' ORDER by sdate ";

	$cursor = $dbh->prepare($sql) || die "Cannot prepare statement: $dbh->errstr\n";
	$cursor->execute($day_diff);
	while(@fields = $cursor->fetchrow_array) {

		$numjobs[$ii] = $fields[0];
                $Npoint[$ii] =  $fields[1]; 
               	$ii++;
 
 }


    &StcrsdbDisconnect();

 @data = ();

my  $graph = new GD::Graph::linespoints(750,650);

if ( ! $graph){
    print STDOUT $query->header(-type => 'text/plain');
    print STDOUT "Failed\n";
} else {
 
  my $format = $graph->export_format;
  print header("image/$format");
  binmode STDOUT;


   if($maxvalue <= 20) {
    $max_y = $maxvalue + 10;
  }elsif($maxvalue <= 50) {
    $max_y = $maxvalue + 20;
  }elsif( $maxvalue <= 100) {
    $max_y = $maxvalue + 50;
  }elsif( $maxvalue <= 200) {
    $max_y = $maxvalue + 100;
  }elsif( $maxvalue <= 1000) {
    $max_y = $maxvalue + 200;
   }else{
   $max_y = $maxvalue + 400;  
  }

 my $xLabelsVertical = 1;
 my $xLabelPosition = 0;
 my $xLabelSkip = 1;

$xLabelSkip = 2  if( $fperiod eq "day" );
$xLabelSkip = 8 if( $fperiod eq "week" );
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

   @data = (\@Npoint, \@numjobs );
 
    $graph->set(x_label => "  ",
		y_label => "Number of jobs",
		title   => "Number of jobs in status '$fstatus' for the period of $fperiod ",
		y_tick_number => 10,
		y_min_value => $min_y,
		y_max_value => $max_y,
		y_number_format => \&y_format,
		dclrs => [ qw(lblack lblue lred lgreen lpink lpurple lorange lyellow ) ],
		line_width => 2,
		markers => [ 2,3,4,5,6,7,8,9],
		marker_size => 1,
                x_label_skip => $xLabelSkip, 
                x_labels_vertical =>$xLabelsVertical, 		
		);

#    $graph->set_legend(@legend);
#    $graph->set_legend_font(gdMediumBoldFont);
    $graph->set_title_font(gdLargeFont);
    $graph->set_x_label_font(gdMediumBoldFont);
    $graph->set_y_label_font(gdMediumBoldFont);
    $graph->set_x_axis_font(gdMediumBoldFont);
    $graph->set_y_axis_font(gdMediumBoldFont);


           if ( scalar(@Npoint) <= 1 ) {

            print  $query->header(-type => 'text/html')."\n";
            &beginHtml();

        } else {

     print STDOUT $graph->plot(\@data)->$format();

   }
  }
}

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

#########################
sub beginHtml {

print <<END;
  <html>
   <head>
          <title>CRS jobs state</title>
   </head>
   <body BGCOLOR=\"#ccffff\">
     <h1 align=center>No data for the period of $fperiod </h1>

    </body>
END
}

