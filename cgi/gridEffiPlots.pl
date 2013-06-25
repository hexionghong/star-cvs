#!/usr/local/bin/perl
#!/usr/bin/env perl 
#
# 
#
# L. Didenko 
# gridEffiPlots.pl to make grid production efficicency plots
#
##########################################################


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
$dbname="Scheduler_bnl";



struct JobAttr => {
    subday      => '$',
      jbid      => '$',
     prcid      => '$', 
     tsite      => '$',
    glstat      => '$',
    lgstat      => '$',
    erstat      => '$',
    exstat      => '$',
    intrs       => '$',
    outtrs      => '$',
    rftime      => '$',
    ovrstat     => '$',
    nsubmt      => '$', 
};


($sec,$min,$hour,$mday,$mon,$year) = localtime();


if( $mon < 10) { $mon = '0'.$mon };
if( $mday < 10) { $mday = '0'.$mday };
if( $hour < 10) { $hour = '0'.$hour };
if( $min < 10) { $min = '0'.$min };
if( $sec < 10) { $sec = '0'.$sec };


my $todate = ($year+1900)."-".($mon+1)."-".$mday;

my $nowdate;
my $thisyear = $year+1900;
my $dyear = $thisyear - 2000;

my @prodyear = ("2008","2009","2010","2011","2012");

# Tables

$JobEfficiencyT = "MasterJobEfficiency";

my @arsites = ( );
my $mydate;
my $nd = 0;
my $nsite = 0;

my @jbstat = ();
my $nstat = 0;
my $glStatus = 0;
my $lgStatus = 0;
my $erStatus = 0;
my $intrans = 0;
my $outtrans = 0;
my $ovrStat;
my $sdate = "0000-00-00 00:00:00:";
my $cretime = "0000-00-00 00:00:00:";
my $sbday;
my $gsite;
my @njobs = ();
my %siteH = { };
my @globeff = ();
my @logeff = ();
my @inputef = ();
my @outputeff = ();
my @recoComeff = ();
my @overeff = ();
my @overeffrs = (); 
my @ndate = ();
my %globEfH = { };
my %logEfH = { };
my %inEfH  = { };
my %outEfH  = { };
my %recoEfH = { };
my %overEfH = { };
my %siteEff = { };
my %siteEffRs = { };
my %rseffjid = { }; 
my @sites = ();
my $msite;
my $ptag = "none";
my $recoSt;
my $jid;
my $proid;
my $jobid;
my $globSt;
my $logSt;
my $inSt;
my $outSt;

 
&GRdbConnect();

 
my @arperiod = ("week","1_month","2_months","3_months","4_months","5_months","6_months","7_months","8_months","9_months","10_months","11_months","12_months");


$sql="SELECT DISTINCT site  FROM $JobEfficiencyT where site is not NULL ";

$cursor =$dbh->prepare($sql)
  || die "Cannot prepare statement: $DBI::errstr\n";
$cursor->execute();

while( $mysite = $cursor->fetchrow() ) {
    $arsites[$nsite] = $mysite;
    $nsite++;
}
$cursor->finish();


push(@sites, @arsites);
#  push @arsites, "ALL";
   
&GRdbDisconnect();

my $query = new CGI;

my $scriptname = $query->url(-relative=>1);

my $pryear  = $query->param('ryear');
my $qperiod = $query->param('period');
my $qsite   = $query->param('prodsite');

if( $qperiod eq "" and $qsite eq "" and $pryear eq "" ) {
    print $query->header();
    print $query->start_html('Grid Jobs efficiency');
    print <<END;
<META HTTP-EQUIV="Expires" CONTENT="0">
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<META HTTP-EQUIV="Cache-Control" CONTENT="no-cache">
END
    print $query->startform(-action=>"$scriptname");

    print "<body bgcolor=\"cornsilk\">\n";
    print "<h1 align=center><u>Grid Production Efficiency</u></h1>\n";
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
    print "<h3 align=center> Select year</h3>";
    print "<h4 align=center>";
    print  $query->scrolling_list(-name=>'ryear',
	                          -values=>\@prodyear,
	                          -default=>2012,
      			          -size =>1);

    print "<p>";
    print "</td><td>";  
    print "<h3 align=center> Period of monitoring</h3>";
    print "<h4 align=center>";
    print  $query->scrolling_list(-name=>'period',
                                  -values=>\@arperiod,
                                  -default=>week,
                                  -size =>1); 


    print "<p>";
    print "</td><td>";
    print "<h3 align=center>Production Site</h3>";
    print "<h4 align=center>";
    print  $query->scrolling_list(-name=>'prodsite',
	                          -values=>\@arsites,
                                  -default=>pdsf,
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

} else {
    # Argumnts are provided
    
    my $pryear  = $query->param('ryear');
    my $qperiod = $query->param('period');
    my $qsite   = $query->param('prodsite');
 
    my $dyear = $pryear - 2000;
    if($dyear < 10) { $dyear = "0".$dyear };

    # Tables

    $JobEfficiencyT = "MasterJobEfficiency";

    my $day_diff = 0;
    my $nmonth = 0;
    my @prt = ();
    my $myday;
    my $nday = 0;
    my @ardays = ();
    my $tdate;
    my $nsubmit;
    my @jbsub = ();
    my $njb = 0;
    my $jbt;
    my $nowdatetime;


 if( $thisyear eq $pryear) {

  $nowdate = $todate ;
 $nowdatetime = $todate ." ".$hour.":".$min.":59" ;

 }else{

 $nowdate = $pryear. "-12-31";
 $nowdatetime = $nowdate." 23:59:59";
 }


    if( $qperiod eq "week") {
	$day_diff = 8;
  
    } elsif ( $qperiod =~ /month/) {
	@prt = split("_", $qperiod);
	$nmonth = $prt[0];
	$day_diff = 30*$nmonth + 1; 
    }

    $day_diff = int($day_diff);

    &GRdbConnect();

    $sql="SELECT DISTINCT date_format(submitTime, '%Y-%m-%d') AS PDATE  FROM $JobEfficiencyT WHERE ( lastKnownState = 'done' OR lastKnownState = 'failed' OR lastKnownState = 'killed' OR lastKnownState = 'held' ) AND submitAttempt = 1 AND site = ? AND (TO_DAYS(\"$nowdate\") - TO_DAYS(submitTime)) < ?  and  submitTime <= '$nowdatetime' order by PDATE";

    $cursor =$dbh->prepare($sql)
      || die "Cannot prepare statement: $DBI::errstr\n";
    $cursor->execute($qsite,$day_diff);

    while($myday = $cursor->fetchrow) {
        $ardays[$nday] = $myday;
        $nday++;
    }

    ###########

    $sql="SELECT DISTINCT JobID_MD5, processID, submitAttempt, site, overAllState  FROM $JobEfficiencyT WHERE ( lastKnownState = 'done' OR lastKnownState = 'failed' OR lastKnownState = 'killed' OR lastKnownState = 'held' ) AND submitAttempt >= 1  AND site = ? ";

    $cursor =$dbh->prepare($sql)
      || die "Cannot prepare statement: $DBI::errstr\n";
    $cursor->execute($qsite);

    while( @fields = $cursor->fetchrow() ) {
        my $cols=$cursor->{NUM_OF_FIELDS};
	$fObjAdr = \(JobAttr->new());

	for($i=0;$i<$cols;$i++) {
	    my $fvalue=$fields[$i];
	    my $fname=$cursor->{NAME}->[$i];
#          print "$fname = $fvalue\n" ;

	    ($$fObjAdr)->jbid($fvalue)      if( $fname eq 'JobID_MD5');
	    ($$fObjAdr)->prcid($fvalue)     if( $fname eq 'processID');
	    ($$fObjAdr)->tsite($fvalue)     if( $fname eq 'site');
	    ($$fObjAdr)->ovrstat($fvalue)   if( $fname eq 'overAllState');
	    ($$fObjAdr)->nsubmt($fvalue)    if( $fname eq 'submitAttempt');

	}
	$jbsub[$njb] = $fObjAdr;
        $njb++;
    }

    my $ndt = 0;

    @overeff = ();
    @overeffrs = (); 
    %rseffjid = { }; 

    ######################
 
    foreach $jbt (@jbsub) {
	$jid       = ($$jbt)->jbid;
	$proid     = ($$jbt)->prcid;
	$gsite     = ($$jbt)->tsite; 
	$ovrStat   = ($$jbt)->ovrstat;
	$nsubmit   = ($$jbt)->nsubmt;  
	
	$jobid = $jid."_".$proid;


	if($ovrStat eq "success") {
	    if($nsubmit >= 2 and $nsubmit <= 5 ) { 
     		$rseffjid{$jobid} = 1;
	    } else {
		$rseffjid{$jobid} = 0;
	    }
	} else {
	    $rseffjid{$jobid} = 0;
 
	}
    }


    #####################

    foreach  $tdate (@ardays) {
	@jbstat = ();  
	$nstat = 0;

	if( $qsite eq "ALL" ) {

	    $sql="SELECT date_format(submitTime, '%Y-%m-%d') AS PDATE, JobID_MD5, processID, site, submitAttempt, globusError, dotOutHasSize, dotErrorHasSize, exec, transIn, transOut, overAllState FROM $JobEfficiencyT WHERE  submitTime like '$tdate%' AND (lastKnownState = 'done' OR lastKnownState = 'failed' OR lastKnownState = 'killed' OR lastKnownState = 'held' ) AND submitAttempt = 1 AND prodType <> 'simu' "; 

	    $cursor =$dbh->prepare($sql)
	      || die "Cannot prepare statement: $DBI::errstr\n";
	    $cursor->execute;

	} else {

	    $sql="SELECT date_format(submitTime, '%Y-%m-%d') AS PDATE, JobID_MD5, processID, site, submitAttempt, globusError, dotOutHasSize, dotErrorHasSize, exec, transIn, transOut, overAllState FROM $JobEfficiencyT WHERE site = ? AND submitTime like '$tdate%' AND (lastKnownState = 'done' OR lastKnownState = 'failed' OR lastKnownState = 'killed' OR lastKnownState = 'held') AND submitAttempt = 1 AND prodType <> 'simu' ";


	    $cursor =$dbh->prepare($sql)
	      || die "Cannot prepare statement: $DBI::errstr\n";
	    $cursor->execute($qsite);
	} 
	while(@fields = $cursor->fetchrow) {
	    my $cols=$cursor->{NUM_OF_FIELDS};
	    $fObjAdr = \(JobAttr->new());

	    for($i=0;$i<$cols;$i++) {
		my $fvalue=$fields[$i];
		my $fname=$cursor->{NAME}->[$i];
                # print "$fname = $fvalue\n" ;

		($$fObjAdr)->subday($fvalue)    if( $fname eq 'PDATE');
		($$fObjAdr)->jbid($fvalue)      if( $fname eq 'JobID_MD5');
		($$fObjAdr)->prcid($fvalue)     if( $fname eq 'processID');
		($$fObjAdr)->tsite($fvalue)     if( $fname eq 'site');
		($$fObjAdr)->glstat($fvalue)    if( $fname eq 'globusError');
		($$fObjAdr)->lgstat($fvalue)    if( $fname eq 'dotOutHasSize');
		($$fObjAdr)->erstat($fvalue)    if( $fname eq 'dotErrorHasSize');
		($$fObjAdr)->exstat($fvalue)    if( $fname eq 'exec');          
		($$fObjAdr)->intrs($fvalue)     if( $fname eq 'transIn');  
		($$fObjAdr)->outtrs($fvalue)    if( $fname eq 'transOut'); 
		($$fObjAdr)->ovrstat($fvalue)   if( $fname eq 'overAllState');
		($$fObjAdr)->nsubmt($fvalue)    if( $fname eq 'submitAttempt');

	    }
	    $jbstat[$nstat] = $fObjAdr;
	    $nstat++;
	}


	%siteH     = { };
	%globEfH   = { };
	%logEfH    = { };
	%inEfH     = { };
	%outEfH    = { };
	%recoEfH   = { };
	%overEfH   = { };
	%siteEff   = { };
	%siteEffRs = { };

	foreach $jstat (@jbstat) {
	    $sbday     = ($$jstat)->subday;
	    $jid       = ($$jstat)->jbid;
	    $proid     = ($$jstat)->prcid;
	    $gsite     = ($$jstat)->tsite; 
	    $glStatus  = ($$jstat)->glstat; 
	    $lgStatus  = ($$jstat)->lgstat;
	    $erStatus  = ($$jstat)->erstat; 
	    $intrans   = ($$jstat)->intrs;
	    $outtrans  = ($$jstat)->outtrs; 
	    $recoSt    = ($$jstat)->exstat;
	    $ovrStat   = ($$jstat)->ovrstat;
	    $nsubmit   = ($$jstat)->nsubmt;  

	    $jobid = $jid."_".$proid;

	    if(!defined($gsite)) {$gsite = "unknown"}


	    if( $glStatus == 129 ) {
		$glStatus = -1;
	    }

	    if ( $nsubmit == 1) {
       		$siteH{$sbday}++; 

		if($glStatus == -1) {
		    $globEfH{$sbday}++ ;
		}
 
		$logEfH{$sbday} =  $logEfH{$sbday} +  $lgStatus + $erStatus;
		$inEfH{$sbday} = $inEfH{$sbday} + $intrans; 
		if( $outtrans == 1) { 
		    $outEfH{$sbday}++;
		}
		if( $recoSt = 1) {     
		    $recoEfH{$sbday}++;
		}

		if ($ovrStat eq "success" ) {
		    $siteEff{$sbday}++;
		    $siteEffRs{$sbday}++;
		} else {
		    if($rseffjid{$jobid} == 1) {
			$siteEffRs{$sbday} = $siteEffRs{$sbday} + 1 ; 
		    }
		}

		$ndate[$ndt] = $sbday;    

		$njobs[$ndt] = $siteH{$sbday};

		$globeff[$ndt] = $globEfH{$sbday}*100/$njobs[$ndt];
		$logeff[$ndt] = $logEfH{$sbday}*100/(2*$njobs[$ndt]);
		$inputef[$ndt] = $inEfH{$sbday}*100/$njobs[$ndt];
		$outputeff[$ndt] = $outEfH{$sbday}*100/$njobs[$ndt];
		$recoComeff[$ndt] = $recoEfH{$sbday}*100/$njobs[$ndt]; 
		$overeff[$ndt] = $siteEff{$sbday}*100/$njobs[$ndt];
		$overeffrs[$ndt] = $siteEffRs{$sbday}*100/$njobs[$ndt];
	    }
	}
	$ndt++;
    } # foreach tdate
 

    &GRdbDisconnect();

    my @data = ();

    my $graph = new GD::Graph::linespoints(750,650);

    if ( ! $graph){
	print STDOUT $query->header(-type => 'text/plain');
	print STDOUT "Failed\n";

    } else {
	# 
	my $format = $graph->export_format();

	$ptag = $gsite; 

	$legend[0] = "Globus efficiency;        ";
	$legend[1] = "Log files delivery;       ";
	$legend[2] = "Input transferring;       ";
	$legend[3] = "Output transferring;      ";
	$legend[4] = "Reco completion;          ";
	$legend[5] = "Overall efficiency;       ";
	$legend[6] = "Overall efficiency with resubmission <= 4;";
	
	@data = (\@ndate, \@globeff, \@logeff, \@inputef, \@outputeff, \@recoComeff, \@overeff, \@overeffrs ) ;
  
	my $ylabel;
	my $gtitle; 
	my $xLabelsVertical = 1;
	my $xLabelPosition = 0;
	my $xLabelSkip = 1;
	my $skipnum = 1;
 

	$min_y = 0;
	$max_y = 140 ; 

	if (scalar(@ndate) >= 20 ) {
	    $skipnum = int(scalar(@ndate)/20);
	}

	$xLabelSkip = $skipnum;

	$ylabel = "Efficiency in %";
	$gtitle = "Efficiency of jobs execution for the period of $qperiod on  $qsite site";
	
	$graph->set(x_label => "Date of Production",
	            y_label => $ylabel,
                    title   => $gtitle,
                    y_tick_number => 14,
                    x_label_position => 0.5,
                    y_min_value => $min_y,
                    y_max_value => $max_y,
                    y_number_format => \&y_format,
	            #labelclr => "lblack",
                    titleclr => "lblack",
                    dclrs => [ qw(lblue lgreen lgray lpurple lorange lred lblack) ],
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
	    print $query->header(-type => 'text/html')."\n";
	    &beginHtml();
	} else {
	    print header("image/$format");
	    binmode STDOUT;

	    print STDOUT $graph->plot(\@data)->$format();
	}
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


######################
sub GRdbConnect {
    $dbh = DBI->connect("dbi:mysql:$dbname:$dbhost", $dbuser, $dbpass)
      || die "Cannot connect to db server $DBI::errstr\n";
}

######################
sub GRdbDisconnect {
    $dbh = $dbh->disconnect() || die "Disconnect failure $DBI::errstr\n";
}

#####################################

sub beginHtml {

print <<END;
  <html>
   <head>
          <title>Jobs Efficiency</title>
   </head>
   <body BGCOLOR=\"#ccffff\">
     <h1 align=center>No Data for $qperiod period</h1>

    </body>
   </html>
END
}
