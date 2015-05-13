#!/usr/local/bin/perl
#!/usr/bin/env perl
#
# 
#
#   prodSizePlots.cgi
#
# L. Didenko
# script to make plots for Mudst and raw data production size distribution sunk to and restored from HPSS  
# 
#########################################################################################################


BEGIN {
 use CGI::Carp qw(fatalsToBrowser carpout);
}

use DBI;
use CGI qw(:standard);
use GD;
use GD::Graph::linespoints;
use Class::Struct;


$dbhost="duvall.star.bnl.gov";
$dbuser="starreco";
$dbpass="";
$dbname="operation";


($sec,$min,$hour,$mday,$mon,$year) = localtime();

$mon++;
if( $mon < 10) { $mon = '0'.$mon };
if( $mday < 10) { $mday = '0'.$mday };
if( $hour < 10) { $hour = '0'.$hour };
if( $min < 10) { $min = '0'.$min };
if( $sec < 10) { $sec = '0'.$sec };


my $todate = ($year+1900)."-".$mon."-".$mday;

my $nowdate;
my $thisyear = $year+1900;
my $dyear = $thisyear - 2000;

my $pryear = "2014";

my @arrate = ("mudstsize","daqsize","all" );

my @arrprod = ();
my @trigs = ();
my @ndate = ();
my $ndt = 0;
my $nst = 0;
my $npr = 0;
my $ntr = 0;

my @jbsize = ();
my $psize = 0;
my @daqsize = ();
my @prt = ();
my @ardays = ();
my @data = ();


my $ProdSizeT = "ProductionSize";

 &StDbProdConnect();


   $sql="SELECT DISTINCT prodtag  FROM $ProdSizeT ";

      $cursor =$dbh->prepare($sql)
          || die "Cannot prepare statement: $DBI::errstr\n";
       $cursor->execute();

       while( my $mpr = $cursor->fetchrow() ) {

          $arrprod[$npr] = $mpr;
          $npr++;
        }
    $cursor->finish();

   $arrprod[$npr] = "all2014";

   $sql="SELECT DISTINCT Trigset  FROM $ProdSizeT ";

      $cursor =$dbh->prepare($sql)
          || die "Cannot prepare statement: $DBI::errstr\n";
       $cursor->execute();

       while( my $mpr = $cursor->fetchrow() ) {
          $trigs[$ntr] = $mpr;
          $ntr++;
       }
    $cursor->finish();

   $trigs[$ntr] = "all";


&StDbProdDisconnect();

my $query = new CGI;

my $scriptname = $query->url(-relative=>1);

my $qprod   = $query->param('prod');
my $srate   = $query->param('prate');
my $qtrig   = $query->param('ptrig');

if( $qprod eq "" and $srate eq "" and  $qtrig eq "" ) {

    print $query->header();
    print $query->start_html('Production size distribution');
    print <<END;
<META HTTP-EQUIV="Expires" CONTENT="0">
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<META HTTP-EQUIV="Cache-Control" CONTENT="no-cache">
END
    print $query->startform(-action=>"$scriptname");

    print "<body bgcolor=\"cornsilk\">\n";
    print "<h1 align=center><u>Production size distribution </u></h1>\n";
    print "<br>";
    print "<br>";
    print <<END;
<hr>
<table BORDER=0 align=center width=99% cellspacing=3>
<tr ALIGN=center VALIGN=CENTER NOSAVE>
<td>
END

    print "<p>";
    print "</td><td>";
    print "<h3 align=center> Production series <br></h3>";
    print "<h4 align=center>";
    print  $query->scrolling_list(-name=>'prod',
                                  -values=>\@arrprod,
                                  -default=>all2014,
                                  -size =>1);

    print "<p>";
    print "</td><td>";
    print "<h3 align=center> Dataset name <br></h3>";
    print "<h4 align=center>";
    print  $query->scrolling_list(-name=>'ptrig',
                                  -values=>\@trigs,
                                  -default=>all,
                                  -size =>1);


    print "<p>";
    print "</td><td>";
    print "<h3 align=center> Size of raw/MuDst data<br></h3>";
    print "<h4 align=center>";
    print  $query->scrolling_list(-name=>'prate',
                                  -values=>\@arrate,
                                  -default=>all,
                                  -size =>1);


    print "<p>";
    print "</td><td>";
    print "</td> </tr> </table><hr><center>";

    print "</h4>";
    print "<br>";
    print "<br>";
    print $query->submit(),"<p>";
    print $query->reset();
    print $query->endform();
    print "<br>";
    print "<br>";
    print "<address><a href=\"mailto:didenko\@bnl.gov\">Lidia Didenko</a></address>\n";

    print $query->end_html();

 } else{

  my $qqr = new CGI;

my $qprod =   $qqr->param('prod');
my $srate =   $qqr->param('prate');
my $qtrig =   $qqr->param('ptrig');

# Tables

 @prt = ();
 @ardays = ();

 my $myday;
 my $nday = 0;
 my $tdate;

 &StDbProdConnect();

 $nowdate = $todate;

  if($qtrig eq "all") {

  if($qprod eq "all2014"){


   $sql="SELECT DISTINCT date_format(starttime, '%Y-%m-%d') as SDATE FROM $ProdSizeT WHERE (prodtag = 'P15ic' or prodtag = 'P15ie')  and date_format(starttime, '%Y-%m-%d') <> '0000-00-00'  order by SDATE";

    $cursor =$dbh->prepare($sql)
      || die "Cannot prepare statement: $DBI::errstr\n";
    $cursor->execute();

    while($myday = $cursor->fetchrow) {
        $ardays[$nday] = $myday;
        $nday++;
    }

         $cursor->finish();

  }else{


   $sql="SELECT DISTINCT date_format(starttime, '%Y-%m-%d') as SDATE FROM $ProdSizeT WHERE prodtag = ?  and date_format(starttime, '%Y-%m-%d') <> '0000-00-00'  order by SDATE";

    $cursor =$dbh->prepare($sql)
      || die "Cannot prepare statement: $DBI::errstr\n";
    $cursor->execute($qprod);

    while($myday = $cursor->fetchrow) {
        $ardays[$nday] = $myday;
        $nday++;
    }

         $cursor->finish();


  }else{


   $sql="SELECT DISTINCT date_format(starttime, '%Y-%m-%d') as SDATE  FROM $ProdSizeT WHERE prodtag = ? and Trigset = ? and  date_format(starttime, '%Y-%m-%d') <> '0000-00-00' order by SDATE";

    $cursor =$dbh->prepare($sql)
      || die "Cannot prepare statement: $DBI::errstr\n";
    $cursor->execute($qprod,$qtrig);

    while($myday = $cursor->fetchrow) {
        $ardays[$nday] = $myday;
        $nday++;
    }

         $cursor->finish();
  }

#############################  size of MuDst

 

@jbsize = ();
@ndate = ();
$ndt = 0;

     if($qtrig eq "all") {  

  if($qprod eq "all2014"){

  foreach my $tdate (@ardays) {

  $sql="SELECT date_format(createtime, '%Y-%m-%d') as PDATE, sum(mudstsize) FROM $ProdSizeT WHERE (createTime BETWEEN '$tdate 00:00:00' AND '$tdate 23:59:59') and (prodtag = 'P15ic' or prodtag = 'P15ie') group by PDATE  ";

            $cursor =$dbh->prepare($sql)
              || die "Cannot prepare statement: $DBI::errstr\n";
            $cursor->execute();


       while(@fields = $cursor->fetchrow) {

       $ndate[$ndt] = $fields[0];
       $jbsize[$ndt] = $fields[1]/1000000000;

      $ndt++;

     }
  }

  }else{

  foreach my $tdate (@ardays) {

  $sql="SELECT date_format(createtime, '%Y-%m-%d') as PDATE, sum(mudstsize) FROM $ProdSizeT WHERE (createTime BETWEEN '$tdate 00:00:00' AND '$tdate 23:59:59') and prodtag = ? group by PDATE  ";

            $cursor =$dbh->prepare($sql)
              || die "Cannot prepare statement: $DBI::errstr\n";
            $cursor->execute($qprod);


       while(@fields = $cursor->fetchrow) {

       $ndate[$ndt] = $fields[0];
       $jbsize[$ndt] = $fields[1]/1000000000;

      $ndt++;

     }
  }

  }else{

  foreach my $tdate (@ardays) {

  $sql="SELECT date_format(createtime, '%Y-%m-%d') as PDATE, sum(mudstsize) FROM $ProdSizeT WHERE (createTime BETWEEN '$tdate 00:00:00' AND '$tdate 23:59:59') and prodtag = ? and Trigset = ? and filename like '%MuDst.root' group by PDATE  ";

            $cursor =$dbh->prepare($sql)
              || die "Cannot prepare statement: $DBI::errstr\n";
            $cursor->execute($qprod,$qtrig);


       while(@fields = $cursor->fetchrow) {

	$ndate[$ndt] = $fields[0];
        $jbsize[$ndt] = $fields[1]/1000000000;

       $ndt++;

     }
    }
#
  }
  
#################################  size of daq files


@daqsize = ();
@ndate = ();
$ndt = 0;

 
     if($qtrig eq "all") {  

   if($qprod eq "all2014"){

  foreach my $tdate (@ardays) {

  $sql="SELECT date_format(starttime, '%Y-%m-%d') as PDATE, sum(daqsize) FROM $ProdSizeT WHERE  (starttime BETWEEN '$tdate 00:00:00' AND '$tdate 23:59:59') and (prodtag = 'P15ic' or prodtag = 'P15ie')  group by PDATE  ";

            $cursor =$dbh->prepare($sql)
              || die "Cannot prepare statement: $DBI::errstr\n";
            $cursor->execute();


       while(@fields = $cursor->fetchrow) {

       $ndate[$ndt] = $fields[0];
       $daqsize[$ndt] = $fields[1]/1000000000;

       $ndt++;

       }
      }

   }else{

  foreach my $tdate (@ardays) {

  $sql="SELECT date_format(starttime, '%Y-%m-%d') as PDATE, sum(daqsize) FROM $ProdSizeT WHERE  (starttime BETWEEN '$tdate 00:00:00' AND '$tdate 23:59:59') and prodtag = ?  group by PDATE  ";

            $cursor =$dbh->prepare($sql)
              || die "Cannot prepare statement: $DBI::errstr\n";
            $cursor->execute($qprod);


       while(@fields = $cursor->fetchrow) {

       $ndate[$ndt] = $fields[0];
       $daqsize[$ndt] = $fields[1]/1000000000;

       $ndt++;

       }
      }

     }else{

  foreach my $tdate (@ardays) {

  $sql="SELECT date_format(starttime, '%Y-%m-%d') as PDATE, sum(daqsize) FROM $ProdSizeT WHERE (starttime BETWEEN '$tdate 00:00:00' AND '$tdate 23:59:59') and  prodtag = ? and Trigset = ? group by PDATE  ";

            $cursor =$dbh->prepare($sql)
              || die "Cannot prepare statement: $DBI::errstr\n";
            $cursor->execute($qprod,$qtrig);


       while(@fields = $cursor->fetchrow) {

       $ndate[$ndt] = $fields[0];
       $daqsize[$ndt] = $fields[1]/1000000000;

       $ndt++;

       }
      }
     }
##

############################################################

    &StDbProdDisconnect();

 @data = ();
 my $ylabel;
 my $gtitle;
  my $max_y;

    my $graph = new GD::Graph::linespoints(750,650);

    if ( ! $graph){
        print STDOUT $qqr->header(-type => 'text/plain');
        print STDOUT "Failed\n";

    } else {

    if( $srate eq "mudstsize" ) {

    @data = ();

   $max_y = 21000;

       $ylabel = "Size of MuDst data in GB sinking to HPSS per day";
       $gtitle = "Size of MuDst data in GB sinking to HPSS in $qprod production ";

  @data = (\@ndate, \@jbsize);


 }elsif( $srate eq "daqsize" ) {

    @data = ();

    $max_y = 56000;

       $ylabel = "Size of raw data in GB restored from HPSS per day";
       $gtitle = "Size of raw data in GB restored from HPSS in $qprod production";

  @data = (\@ndate, \@daqsize);

 }else{

   @data = ();

    $max_y = 56000;

       $ylabel = "Size of data in GB transferred  from/to HPSS per day";
       $gtitle = "Size of data in GB transferred  from/to HPSS in $qprod production";

  @data = (\@ndate, \@daqsize, \@jbsize  );


   $legend[0] = "raw data size";
   $legend[1] = "mudst size";


 }

  my $xLabelsVertical = 1;
  my $xLabelPosition = 0;
  my $xLabelSkip = 1;
  my $skipnum = 1;

my  $min_y = 0;

  if (scalar(@ndate) >= 40 ) {
    $skipnum = int(scalar(@ndate)/20);
        }

  $xLabelSkip = $skipnum;

       $graph->set(x_label => "Date of files transferring from/to HPSS",
                    y_label => $ylabel,
                    title   => $gtitle,
                    y_tick_number => 14,
                    x_label_position => 0.5,
                    y_min_value => $min_y,
                    y_max_value => $max_y,
                    y_number_format => \&y_format,
                    #labelclr => "lblack",
                    titleclr => "lblack",
                    dclrs => [ qw(lblack lred lblue lgreen lpurple lorange marine lbrown lyellow lgray) ],
                    line_width => 4,
                    markers => [ 2,3,4,5,6,7,8,9],
                    marker_size => 3,
                    x_label_skip => $xLabelSkip,
                    x_labels_vertical =>$xLabelsVertical,
                    );

        $graph->set_legend(@legend);
        $graph->set_legend_font(gdMediumBoldFont);
        $graph->set_title_font(gdGiantFont);
        $graph->set_x_label_font(gdGiantFont);
        $graph->set_y_label_font(gdGiantFont);
        $graph->set_x_axis_font(gdMediumBoldFont);
        $graph->set_y_axis_font(gdMediumBoldFont);

        if ( scalar(@ndate) <= 1 ) {
            print $qqr->header(-type => 'text/html')."\n";
            &beginHtml();
        } else {
            my $format = $graph->export_format;
            print header("image/$format");
            binmode STDOUT;

            print STDOUT $graph->plot(\@data)->$format();
        }
#
     }
  }


############################

sub y_format
{
    my $value = shift;
    my $ret;

    $ret = sprintf("%8.2f", $value);
}

#==============================================================================

######################
sub StDbProdConnect {
    $dbh = DBI->connect("dbi:mysql:$dbname:$dbhost", $dbuser, $dbpass)
        || die "Cannot connect to db server $DBI::errstr\n";
}

######################
sub StDbProdDisconnect {
    $dbh = $dbh->disconnect() || die "Disconnect failure $DBI::errstr\n";
}

#####################################

sub beginHtml {

print <<END;
  <html>
   <head>
          <title>Production size distrubution</title>
   </head>
   <body BGCOLOR=\"#ccffff\">
     <h1 align=center>No Data for $qprod production </h1>


    </body>
   </html>
END
}

