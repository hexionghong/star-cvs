#!/usr/local/bin/perl
#!/usr/bin/env perl 
#
# 
#
# L. Didenko 
# jobsCpuPlots.pl to get distribution of CPU, realtime/cpu, total time of stream jobs by production date
#
#########################################################################################################


BEGIN {
 use CGI::Carp qw(fatalsToBrowser carpout);
}

use DBI;
use CGI qw(:standard);
use GD;
use GD::Graph::linespoints;
use Mysql;
use Class::Struct;
use POSIX qw(log10);

#  $dbhost="duvall.star.bnl.gov";

$dbhost="fc2.star.bnl.gov:3386";
$dbuser="starreco";
$dbpass="";
$dbname="operation";

struct JobAttr => {
      cpuv      => '$',
      rtmv      => '$',
      jbtot     => '$',
      strv      => '$'
};


($sec,$min,$hour,$mday,$mon,$year) = localtime();


if( $mon < 10) { $mon = '0'.$mon };
if( $mday < 10) { $mday = '0'.$mday };
if( $hour < 10) { $hour = '0'.$hour };
if( $min < 10) { $min = '0'.$min };
if( $sec < 10) { $sec = '0'.$sec };


my $todate = ($year+1900)."-".($mon+1)."-".$mday;

my $nowdate = $todate;

my $thisyear = $year+1900;
my $dyear = $thisyear - 2000;

my @prodyear = ("2009","2010");


my @arperiod = ( );
my $mstr;
my @arrate = ("cpu","rtime/cpu","jobtottime");

my @arrprod = ();
my @arstream = ();
my $nst = 0;
my $str;
my $npr = 0;
my $mpr;
my $phr;
my $pcpu;
my $prtime;
my $pstream;
my $jbTottime;
my $pryear = "2010";

my $rte = 0;

my @arupsilon = ();
my @armtd = ();
my @arphysics = ();
my @argamma = ();
my @arhlt = ();
my @arfmsfast = ();
my @arht = ();
my @aratomcules = ();
my @arupc = ();
my @armonitor = ();
my @arpmdftp = ();
my @ndate = ();
my $ndt = 0;
my @rdays = ();
my $ndy = 0;
my @rvdays = ();

my @cpupsilon = ();
my @cpmtd = ();
my @cpphysics = ();
my @cpgamma = ();
my @cphlt = ();
my @cpfmsfast = ();
my @cpht = ();
my @cpatomcules = ();
my @cpupc = ();
my @cpmonitor = ();
my @cppmdftp = (); 

my @jbupsilon = ();
my @jbmtd = ();
my @jbphysics = ();
my @jbgamma = ();
my @jbhlt = ();
my @jbfmsfast = ();
my @jbht = ();
my @jbatomcules = ();
my @jbupc = ();
my @jbmonitor = ();
my @jbpmdftp = ();

 $JobStatusT = "JobStatus2010";  

  &StDbProdConnect();

    $sql="SELECT DISTINCT prodSeries  FROM $JobStatusT ";

      $cursor =$dbh->prepare($sql)
          || die "Cannot prepare statement: $DBI::errstr\n";
       $cursor->execute();

       while( $mpr = $cursor->fetchrow() ) {
          $arrprod[$npr] = $mpr;
          $npr++;
       }
    $cursor->finish();


  $sql="SELECT DISTINCT runDay  FROM $JobStatusT where runDay >= '2010-07-20' order by runDay" ;

      $cursor =$dbh->prepare($sql)
          || die "Cannot prepare statement: $DBI::errstr\n";
       $cursor->execute();

       while( $dy = $cursor->fetchrow() ) {
          $rdays[$ndy] = $dy;
          $ndy++;
       }
    $cursor->finish();

@rvdays = reverse @rdays ;

&StDbProdDisconnect();

my $query = new CGI;

my $scriptname = $query->url(-relative=>1);

#my $pryear = $query->param('pyear');
#my $qperiod = $query->param('period');

my $qprod = $query->param('prod');
my $srate = $query->param('prate');
my $qday = $query->param('pday');

if( $qprod eq "" and $qday eq ""  and $srate eq "" ) {

    print $query->header();
    print $query->start_html('Production CPU usage');
    print <<END;
<META HTTP-EQUIV="Expires" CONTENT="0">
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<META HTTP-EQUIV="Cache-Control" CONTENT="no-cache">
END
    print $query->startform(-action=>"$scriptname");

    print "<body bgcolor=\"cornsilk\">\n";
    print "<h1 align=center><u>Production CPU&RealTime distributions of stream jobs </u></h1>\n";
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
    print "<h3 align=center> Production series</h3>";
    print "<h4 align=center>";
    print  $query->scrolling_list(-name=>'prod',
	                          -values=>\@arrprod,
	                          -default=>P10ik,
      			          -size =>1);

  
   print "<p>";
    print "</td><td>";
    print "<h3 align=center> Stream values: CPU, ratio rtime/CPU <br> or total jobs time on the farm </h3>";
    print "<h4 align=center>";
    print  $query->scrolling_list(-name=>'prate',
                                  -values=>\@arrate,
                                  -default=>cpu,
                                  -size =>1);


    print "<p>";
    print "</td><td>";  
    print "<h3 align=center> Date of production</h3>";
    print "<h4 align=center>";
    print  $query->scrolling_list(-name=>'pday',
                                  -values=>\@rvdays,
                                  -default=>$nowdate,
                                  -size =>1); 

    
    print "<p>";
    print "</td><td>";
    print "</td> </tr> </table><hr><center>";

    print "</h4>";
    print "<br>";
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

    my $qprod = $qqr->param('prod');
    my $qday = $qqr->param('pday'); 
    my $srate = $qqr->param('prate');
    
 # Tables

      $pryear = "2010";

    $JobStatusT = "JobStatus".$pryear;


  my $day_diff = 0;
  my $nmonth = 0;
  my @prt = ();
  my $mhr;
  my $nhr = 0;
  my @arhr = ();
  my $tdate;
  my @jbstat = ();  
  my $nstat = 0;
  my $jset;
  my $cpubin = 0;
  my $rcpubin = 0;
  my $obtotbin = 0;
 
    @arstream = ();


 &StDbProdConnect();

      $sql="SELECT DISTINCT streamName  FROM $JobStatusT where prodSeries = ? ";

      $cursor =$dbh->prepare($sql)
          || die "Cannot prepare statement: $DBI::errstr\n";
       $cursor->execute($qprod);

       while( $str = $cursor->fetchrow() ) {
          $arstream[$nst] = $str;
          $nst++;
       }
    $cursor->finish();


    $sql="SELECT DISTINCT date_format(createTime, '%Y-%m-%d %H') as PDATE  FROM $JobStatusT WHERE prodSeries = ?  AND runDay = ? order by createTime ";

    $cursor =$dbh->prepare($sql)
      || die "Cannot prepare statement: $DBI::errstr\n";
    $cursor->execute($qprod,$qday);

    while($mhr = $cursor->fetchrow() ) {
        $arhr[$nhr] = $mhr;
        $nhr++;
    }

     $cursor->finish();

  #####################

 $rte = 1;
 
 @arupsilon = ();
 @armtd = ();
 @arphysics = ();
 @argamma = ();
 @arhlt = ();
 @arfmsfast = ();
 @arht = ();
 @aratomcules = ();
 @arupc = ();
 @armonitor = ();
 @arpmdftp = ();

 @cpupsilon = ();
 @cpmtd = ();
 @cpphysics = ();
 @cpgamma = ();
 @cphlt = ();
 @cpfmsfast = ();
 @cpht = ();
 @cpatomcules = ();
 @cpupc = ();
 @cpmonitor = ();
 @cppmdftp = (); 

 @jbupsilon = ();
 @jbmtd = ();
 @jbphysics = ();
 @jbgamma = ();
 @jbhlt = ();
 @jbfmsfast = ();
 @jbht = ();
 @jbatomcules = ();
 @jbupc = ();
 @jbmonitor = ();
 @jbpmdftp = ();

   if( $srate eq "jobtottime" ) {

 $ndt = 0;
 @ndate = ();
 @jbstat = ();
 $nstat = 0;

    foreach  $tdate (@arhr) {
 
  $sql="SELECT jobtotalTime, streamName FROM $JobStatusT WHERE  createTime like '$tdate%' AND prodSeries = ? AND jobtotalTime > 0.1  AND submitAttempt = 1 AND jobStatus = 'Done' AND NoEvents >= 10 order by createTime "; 

	    $cursor =$dbh->prepare($sql)
	      || die "Cannot prepare statement: $DBI::errstr\n";
	    $cursor->execute($qprod);

	while(@fields = $cursor->fetchrow) {
	    my $cols=$cursor->{NUM_OF_FIELDS};
	    $fObjAdr = \(JobAttr->new());

	    for($i=0;$i<$cols;$i++) {
		my $fvalue=$fields[$i];
		my $fname=$cursor->{NAME}->[$i];
                # print "$fname = $fvalue\n" ;

                ($$fObjAdr)->jbtot($fvalue)   if( $fname eq 'jobtotalTime');
		($$fObjAdr)->strv($fvalue)    if( $fname eq 'streamName');

	    }
	    $jbstat[$nstat] = $fObjAdr;
	    $nstat++;
         }
    }

###########

 $jobtotbin = 2.0;
  $ndate[0] = 0;

 for ($i = 0; $i < 60; $i++) {
   $ndate[$i] = $jobtotbin*$i; 
 }

     foreach $jset (@jbstat) {
            $jbTottime = ($$jset)->jbtot;
	    $pstream   = ($$jset)->strv;

	    if($jbTottime <= 120 )  {
	   $ndt = int($jbTottime/$jobtotbin);
           $ndate[$ndt] = $jobtotbin*$ndt;  

	   if ( $pstream eq "physics" ) {
	       $jbphysics[$ndt]++;
	   }elsif( $pstream eq "mtd" ) {
               $jbmtd[$ndt]++;
           }elsif( $pstream eq "upsilon" ) {
               $jbupsilon[$ndt]++; 
           }elsif( $pstream eq "gamma" ) {
               $jbgamma[$ndt]++; 
           }elsif( $pstream eq "hlt" ) {
               $jbhlt[$ndt]++;  
           }elsif( $pstream eq "fmsfast" ) {
               $jbfmsfast[$ndt]++; 
           }elsif( $pstream eq "ht" ) {
               $jbht[$ndt]++;  
           }elsif( $pstream eq "atomcules" ) {
               $jbatomcules[$ndt]++; 
           }elsif( $pstream eq "monitor" ) {
               $jbmonitor[$ndt]++;  
           }elsif( $pstream eq "pmdftp" ) {
               $jbpmdftp[$ndt]++;   
           }elsif( $pstream eq "upc" ) {
               $jbupc[$ndt]++;
	       }
 	    }
        }
###################

   }else{

 $ndt = 0;
 @ndate = ();

 @jbstat = ();
 $nstat = 0;

    foreach  $tdate (@arhr) {

  $sql="SELECT CPU_per_evt_sec, RealTime_per_evt, streamName FROM $JobStatusT WHERE  createTime like '$tdate%' AND prodSeries = ? AND CPU_per_evt_sec > 0.01 AND RealTime_per_evt > 0.01 and jobStatus = 'Done' AND NoEvents >= 10 order by createTime "; 

	    $cursor =$dbh->prepare($sql)
	      || die "Cannot prepare statement: $DBI::errstr\n";
	    $cursor->execute($qprod);

	while(@fields = $cursor->fetchrow) {
	    my $cols=$cursor->{NUM_OF_FIELDS};
	    $fObjAdr = \(JobAttr->new());

	    for($i=0;$i<$cols;$i++) {
		my $fvalue=$fields[$i];
		my $fname=$cursor->{NAME}->[$i];
                # print "$fname = $fvalue\n" ;

		($$fObjAdr)->cpuv($fvalue)    if( $fname eq 'CPU_per_evt_sec');
		($$fObjAdr)->rtmv($fvalue)    if( $fname eq 'RealTime_per_evt');
		($$fObjAdr)->strv($fvalue)    if( $fname eq 'streamName');

	    }
	    $jbstat[$nstat] = $fObjAdr;
	    $nstat++;
         }
    }

###########

       if( $srate eq "cpu" ) {

 $ndate[0] = 0;
 $cpubin = 2.0; 
  $ndt = 0;

 for ($i = 0; $i < 50; $i++) {
   $ndate[$i] = $cpubin*$i; 
 }

     foreach $jset (@jbstat) {
	    $pcpu     = ($$jset)->cpuv;
	    $pstream  = ($$jset)->strv;

            if($pcpu <= 100.0 )     {

	    $ndt = int($pcpu/$cpubin);
            $ndate[$ndt] = $cpubin*$ndt;  

	       if ( $pstream eq "physics" ) {
	       $cpphysics[$ndt]++;
	      }elsif( $pstream eq "mtd" ) {
               $cpmtd[$ndt]++;
              }elsif( $pstream eq "upsilon" ) {
               $cpupsilon[$ndt]++; 
              }elsif( $pstream eq "gamma" ) {
               $cpgamma[$ndt]++; 
              }elsif( $pstream eq "hlt" ) {
               $cphlt[$ndt]++;  
              }elsif( $pstream eq "fmsfast" ) {
               $cpfmsfast[$ndt]++; 
              }elsif( $pstream eq "ht" ) {
               $cpht[$ndt]++;  
              }elsif( $pstream eq "atomcules" ) {
               $cpatomcules[$ndt]++; 
              }elsif( $pstream eq "monitor" ) {
               $cpmonitor[$ndt]++;  
              }elsif( $pstream eq "pmdftp" ) {
               $cppmdftp[$ndt]++;   
              }elsif( $pstream eq "upc" ) {
               $cpupc[$ndt]++;
	       }
	    }
	}

##################################################

     }elsif( $srate eq "rtime/cpu" ) {

 $ndate[0] = 0;
 $rcpubin = 0.01; 
 $ndt = 0;

 for ($i = 0; $i < 200; $i++) {
#   $ndate[$i] = 1 + $rcpubin*$i; 
   $ndate[$i] = $rcpubin*$i; 
 }

     foreach $jset (@jbstat) {
	    $pcpu     = ($$jset)->cpuv;
	    $prtime   = ($$jset)->rtmv;
	    $pstream  = ($$jset)->strv;

        if( $pcpu >= 0.01) {             

           $rte = $prtime/$pcpu; 

#	   if($rte >= 1.0 and $rte <= 2.0 )     {
	   if( $rte <= 2.0 )     {
#          $ndt = int(($rte - 1)/$rcpubin + 0.00001);
           $ndt = int($rte/$rcpubin + 0.00001);
#           $ndate[$ndt] = 1 + $rcpubin*$ndt;  
            $ndate[$ndt] = $rcpubin*$ndt;  
#
	       if ( $pstream eq "physics" ) {
	       $arphysics[$ndt]++ ;
	      }elsif( $pstream eq "mtd" ) {
               $armtd[$ndt]++;
              }elsif( $pstream eq "upsilon" ) {
               $arupsilon[$ndt]++ ;
              }elsif( $pstream eq "gamma" ) {
               $argamma[$ndt]++ ;
              }elsif( $pstream eq "hlt" ) {
               $arhlt[$ndt]++;
              }elsif( $pstream eq "fmsfast" ) {
               $arfmsfast[$ndt]++ ;
              }elsif( $pstream eq "ht" ) {
               $arht[$ndt]++ ;
              }elsif( $pstream eq "atomcules" ) {
               $aratomcules[$ndt]++ ;
              }elsif( $pstream eq "monitor" ) {
               $armonitor[$ndt]++ ;
              }elsif( $pstream eq "pmdftp" ) {
               $arpmdftp[$ndt]++ ;   
              }elsif( $pstream eq "upc" ) {
               $arupc[$ndt]++;
	       }
	    } 
	   }
	}
############

      for ($jj = 0; $jj<=$ndt; $jj++) {
 
      if( $arphysics[$jj] >= 1 ) { $arphysics[$jj] = log10($arphysics[$jj])};
      if( $armtd[$jj] >= 1 ) { $armtd[$jj] = log10($armtd[$jj])}; 
      if( $arupsilon[$jj]  >= 1 ) { $arupsilon[$jj]= log10($arupsilon[$jj])};
      if( $argamma[$jj] >= 1 ) { $argamma[$jj] = log10($argamma[$jj]) }; 
      if( $arhlt[$jj]  >= 1 ) {$arhlt[$jj]  = log10($arhlt[$jj])};
      if( $arfmsfast[$jj] >= 1 ) {$arfmsfast[$jj] = log10($arfmsfast[$jj])};
      if( $arht[$jj] >= 1 ) {$arht[$jj] = log10($arht[$jj])};
      if( $aratomcules[$jj] >= 1 ) {$aratomcules[$jj] = log10($aratomcules[$jj])};
      if( $armonitor[$jj] >= 1 ) {$armonitor[$jj] = log10($armonitor[$jj])};
      if( $arpmdftp[$jj] >= 1 ) { $arpmdftp[$jj] = log10($arpmdftp[$jj])};
      if( $arupc[$jj] >= 1 ) {$arupc[$jj] = log10($arupc[$jj])};

         }
      }
 
   }

    &StDbProdDisconnect();

my @data = ();
my $ylabel;
my $gtitle; 

    my $graph = new GD::Graph::linespoints(750,650);

    if ( ! $graph){
	print STDOUT $qqr->header(-type => 'text/plain');
	print STDOUT "Failed\n";

    } else {
	 
       $legend[0] = "st_physics  ";
       $legend[1] = "st_gamma    ";
       $legend[2] = "st_hlt      ";
       $legend[3] = "st_ht       ";
       $legend[4] = "st_monitor  ";
       $legend[5] = "st_pmdftp   ";
       $legend[6] = "st_upc      ";
       $legend[7] = "st_atomcules ";
       $legend[8] = "st_mtd       ";

#       $legend[3] = "st_upsilon   ";
    
       if( $srate eq "cpu" )  {

 @data = ();

	$xlabel = "CPU in sec/evt";
        $ylabel = "Number of jobs";
	$gtitle = "CPU in sec/evt for different stream jobs for $qday day";

    @data = (\@ndate, \@cpphysics, \@cpgamma, \@cphlt, \@cpht, \@cpmonitor, \@cppmdftp, \@cpupc, \@cpatomcules, \@cpmtd ) ; 

      }elsif( $srate eq "rtime/cpu"){

 @data = ();

        $xlabel = "Ratio RealTime/CPU";
        $ylabel = "Number of jobs";
	$gtitle = "Ratios RealTime/CPU for different stream jobs for $qday day";

  
    @data = (\@ndate, \@arphysics, \@argamma, \@arhlt, \@arht, \@armonitor, \@arpmdftp, \@arupc, \@rtatomcules, \@armtd ) ;

     }elsif( $srate eq "jobtottime"){

 @data = ();

        $xlabel = "Total job's time on the farm in hours";
        $ylabel = "Number of jobs";         
	$gtitle = "Total time on the farm for different stream jobs for $qday day";

  
    @data = (\@ndate, \@jbphysics, \@jbgamma, \@jbhlt, \@jbht, \@jbmonitor, \@jbpmdftp, \@jbupc, \@jbatomcules, \@jbmtd ) ;


     }

 my $xLabelsVertical = 1;
 my $xLabelPosition = 0;
 my $xLabelSkip = 1;
 my $skipnum = 2;
 
 $min_y = 0;

  if (scalar(@ndate) > 60 ) {
     $skipnum = int(scalar(@ndate)/20);
 }

 $xLabelSkip = $skipnum;

	$graph->set(x_label => $xlabel,
	            y_label => $ylabel,
                    title   => $gtitle,
                    y_tick_number => 14,
                    x_label_position => 0.5,
                    y_min_value => $min_y,
                    y_max_value => $max_y,
                    y_number_format => \&y_format,
	            #labelclr => "lblack",
                    titleclr => "lblack",
                    dclrs => [ qw(lblue lgreen lpurple lorange lred lblack lgray lbrown lyellow) ],
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
 



###############################
#  subs and helper routines
###############################
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
          <title>CPU and RealTime production usage</title>
   </head>
   <body BGCOLOR=\"#ccffff\">
     <h1 align=center>No Data for $qprod production and $qday day </h1>
     

    </body>
   </html>
END
}
