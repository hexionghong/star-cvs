#! /usr/local/bin/perl -w
#
#  
#  dbRunBrows.pl 
#  L. Didneko
#  
###############################################################################

use CGI;

require "/afs/rhic/star/packages/scripts/dbCpProdSetup.pl";

my $debugOn = 0;
my %pair;
my @pck;

my ($query) = @_;
$query = new CGI;

my $runPr   = $query->param("runN");
my $prodSr;
my $detrSet;
my $trgSet;
my $colSet;
my $fldSet;
my $frSet;
my $lcSet;

$prSet = $query->param("prodSet");

@pck = split ("%",$prSet);

$colSet = $pck[0];
$trgSet = $pck[1];
$detrSet = $pck[2];
$fldSet = $pck[3];
$frSet  = $pck[4];
$lcSet  = $pck[5];
$prodSr = $pck[6];

&StDbProdConnect();

&cgiSetup();

&beginHtml();

my $dirRun = "/home/starreco/reco/" . $prodSr;
my $mstat;
my $mcomment;

if( $frSet eq "daq") {

if($detrSet eq "all" and $trgSet ne "all" and $fldSet ne "all") {

$sql="SELECT * FROM $FileCatalogT WHERE runID = '$runPr' AND trigger = '$trgSet' AND dataset like '$colSet%' AND dataset like '%$fldSet%' AND fName like '%$frSet' AND site like '$lcSet%' order by fileSeq ";

}elsif ($detrSet ne "all" and $trgSet ne "all"  and $fldSet ne "all") {

$sql="SELECT * FROM $FileCatalogT WHERE runID = '$runPr' AND trigger = '$trgSet' AND dataset like '%$detrSet%' AND dataset like '$colSet%' AND dataset like '%$fldSet%' AND fName like '%$frSet' AND site like '$lcSet%' order by fileSeq "; 

}elsif ($detrSet ne "all" and $trgSet eq "all" and $fldSet ne "all" ) {

$sql="SELECT * FROM $FileCatalogT WHERE runID = '$runPr' AND dataset like '%$detrSet%' AND dataset like '$colSet%' AND dataset like '%$fldSet%' AND fName like '%$frSet' AND site like '$lcSet%' order by fileSeq "; 

}elsif ($detrSet eq "all" and $trgSet eq "all" and $fldSet ne "all" ) {

$sql="SELECT * FROM $FileCatalogT WHERE runID = '$runPr' AND dataset like '$colSet%' AND dataset like '%$fldSet%' AND fName like '%$frSet' AND site like '$lcSet%' order by fileSeq ";

 }elsif($detrSet eq "all" and $trgSet ne "all" and $fldSet eq  "all") {

$sql="SELECT * FROM $FileCatalogT WHERE runID = '$runPr' AND trigger = '$trgSet' AND dataset like '$colSet%' AND fName like '%$frSet' AND site like '$lcSet%' order by fileSeq ";

}elsif ($detrSet ne "all" and $trgSet ne "all"  and $fldSet eq "all") {

$sql="SELECT * FROM $FileCatalogT WHERE runID = '$runPr' AND trigger = '$trgSet' AND dataset like '%$detrSet%' AND dataset like '$colSet%' AND fName like '%$frSet' AND site like '$lcSet%' order by fileSeq "; 

}elsif ($detrSet ne "all" and $trgSet eq "all" and $fldSet eq "all" ) {

$sql="SELECT * FROM $FileCatalogT WHERE runID = '$runPr' AND dataset like '%$detrSet%' AND dataset like '$colSet%' AND fName like '%$frSet' AND site like '$lcSet%' order by fileSeq "; 

}elsif ($detrSet eq "all" and $trgSet eq "all" and $fldSet eq "all" ) {

$sql="SELECT * FROM $FileCatalogT WHERE runID = '$runPr' AND dataset like '$colSet%' AND fName like '%$frSet' AND site like '$lcSet%' order by fileSeq ";
  }
}else{

if($detrSet eq "all" and $trgSet ne "all" and $fldSet ne "all") {

$sql="SELECT * FROM $FileCatalogT WHERE runID = '$runPr' AND jobID like '%$prodSr%' AND trigger = '$trgSet' AND dataset like '$colSet%' AND dataset like '%$fldSet%' AND fName like '%$frSet' AND site like '$lcSet%' order by fileSeq  ";

}elsif ($detrSet ne "all" and $trgSet ne "all"  and $fldSet ne "all") {

$sql="SELECT * FROM $FileCatalogT WHERE runID = '$runPr' AND jobID like '%$prodSr%'AND trigger = '$trgSet' AND dataset like '%$detrSet%' AND dataset like '$colSet%' AND dataset like '%$fldSet%' AND fName like '%$frSet' AND site like '$lcSet%'order by fileSeq "; 

}elsif ($detrSet ne "all" and $trgSet eq "all" and $fldSet ne "all" ) {

$sql="SELECT * FROM $FileCatalogT WHERE runID = '$runPr' AND jobID like '%$prodSr%' AND dataset like '%$detrSet%' AND dataset like '$colSet%' AND dataset like '%$fldSet%' AND fName like '%$frSet' AND site like '$lcSet%' order by fileSeq "; 

}elsif ($detrSet eq "all" and $trgSet eq "all" and $fldSet ne "all" ) {

$sql="SELECT * FROM $FileCatalogT WHERE runID = '$runPr' AND jobID like '%$prodSr%' AND dataset like '$colSet%' AND dataset like '%$fldSet%' AND fName like '%$frSet' AND site like '$lcSet%' order by fileSeq ";

}elsif($detrSet eq "all" and $trgSet ne "all" and $fldSet eq  "all") {

$sql="SELECT * FROM $FileCatalogT WHERE runID = '$runPr' AND jobID like '%$prodSr%' AND trigger = '$trgSet' AND dataset like '$colSet%' AND fName like '%$frSet' AND site like '$lcSet%' order by fileSeq ";

}elsif ($detrSet ne "all" and $trgSet ne "all"  and $fldSet eq "all") {

$sql="SELECT * FROM $FileCatalogT WHERE runID = '$runPr' AND jobID like '%$prodSr%' AND trigger = '$trgSet' AND dataset like '%$detrSet%' AND dataset like '$colSet%' AND fName like '%$frSet' AND site like '$lcSet%' order by fileSeq "; 

}elsif ($detrSet ne "all" and $trgSet eq "all" and $fldSet eq "all" ) {

$sql="SELECT * FROM $FileCatalogT WHERE runID = '$runPr' AND jobID like '%$prodSr%' AND dataset like '%$detrSet%' AND dataset like '$colSet%' AND fName like '%$frSet' AND site like '$lcSet%' order by fileSeq  "; 

}elsif ($detrSet eq "all" and $trgSet eq "all" and $fldSet eq "all" ) {

$sql="SELECT * FROM $FileCatalogT WHERE runID = '$runPr' AND jobID like '%$prodSr%' AND dataset like '$colSet%' AND fName like '%$frSet' AND site like '$lcSet%' order by fileSeq ";
  }
}

  $cursor =$dbh->prepare($sql)
     || die "Cannot prepare statement: $DBI::errstr\n";
  $cursor->execute;

 my $counter = 0;

 while(@fields = $cursor->fetchrow) {
   my $cols=$cursor->{NUM_OF_FIELDS};

 for($i=0;$i<$cols;$i++) {
    my $fvalue=$fields[$i];
    my $fname=$cursor->{NAME}->[$i];
    print "$fname = $fvalue\n" if $debugOn;
    $pair{$fname} = $fvalue;

   $mstat = $fvalue if( $fname eq 'dataStatus');   
   $mcomment = $fvalue if( $fname eq 'comment');
  }

  if ($mstat eq "OK"){$mcomment = "no"};
&printRow();

}

&endHtml();

&StDbProdDisconnect();

#################
sub beginHtml {

print <<END;

<html>
  <head>
          <title>Run Number $runPr</title>
  </head>
  <body BGCOLOR=\"#ccffff\">
<h5><a href=\"http://www.star.bnl.gov/devcgi/dbDataSetQuery.pl\">Production Query </a><br></h5> 
   <h2 ALIGN=CENTER>List of files for run number $runPr </h2>
<TABLE BORDER=5 CELLSPACING=1 CELLPADDING=2 >
<TR>
<TR ALIGN=CENTER VALIGN=CENTER>
<TD WIDTH=\"5%\" HEIGHT=50><B>jobID</B></TD>
<TD WIDTH=\"5%\" HEIGHT=50><B>runID</B></TD>
<TD WIDTH=\"5%\" HEIGHT=50><B>fileSeq</B></TD>
<TD WIDTH=\"5%\" HEIGHT=50><B>fName</B></TD>
<TD WIDTH=\"10%\" HEIGHT=50><B>path</B></TD>
<TD WIDTH=\"10%\" HEIGHT=50><B>dataset</B></TD>
<TD WIDTH=\"5%\" HEIGHT=50><B>trigger</B></TD>
<TD WIDTH=\"5%\" HEIGHT=50><B>size</B></TD>
<TD WIDTH=\"5%\" HEIGHT=50><B>createTime</B></TD>
<TD WIDTH=\"5%\" HEIGHT=50><B>Nevents</B></TD>
<TD WIDTH=\"5%\" HEIGHT=50><B>type</B></TD>
<TD WIDTH=\"5%\" HEIGHT=50><B>site</B></TD>
<TD WIDTH=\"5%\" HEIGHT=50><B>calib</B></TD>
<TD WIDTH=\"5%\" HEIGHT=50><B>dataStatus</B></TD>
<TD WIDTH=\"5%\" HEIGHT=50><B>comment</B></TD>
</TR>

END
}

###############
sub printRow {

print <<END;
<TR ALIGN=CENTER VALIGN=CENTER>
<td>$pair{'jobID'}</td>
<td>$pair{'runID'}</td>
<td>$pair{'fileSeq'}</td>
<td>$pair{'fName'}</td>
<td>$pair{'path'}</td>
<td>$pair{'dataset'}</td>
<td>$pair{'trigger'}</td>
<td>$pair{'size'}</td>
<td>$pair{'createTime'}</td>
<td>$pair{'Nevents'}</td>
<td>$pair{'type'}</td>
<td>$pair{'site'}</td>
<td>$pair{'calib'}</td>
<td>$pair{'dataStatus'}</td>
<td>$mcomment</td>
</tr>
END

}

###############
sub endHtml {
my $Date = `date`;

print <<END;
</TABLE>
      <h5>
      <address><a href=\"mailto:didenko\@bnl.gov\">Lidia Didenko</a></address>
<!-- Created: Tue Set 10  05:29:25 MET 1999 -->
<!-- hhmts start -->
Last modified: $Date
<!-- hhmts end -->
  </body>
</html>
END

}

##############
sub cgiSetup {
    $q=new CGI;
    if ( exists($ENV{'QUERY_STRING'}) ) { print $q->header };
}
