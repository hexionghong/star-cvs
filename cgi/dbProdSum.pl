#! /usr/local/bin/perl -w
#
# 
#
# 
#
# L.Didenko
#
# dbProdSum.pl
#
# Scanning FilesCatalog table to get production summary and put it to Web page
# 
################################################################################################

use CGI;

require "/afs/rhic/star/packages/scripts/dbCpProdSetup.pl";

use File::Find;
use Class::Struct;

my $debugOn=0;

#&cgiSetup();
my ($query) = @_;
$query = new CGI;

my $prodSer =  $query->param('SetP');
my $trigD   =  $query->param('SetT');
my $fieldM  =  $query->param('SetF');
my $detSet  =  $query->param('SetD');
my $colSet  =  $query->param('SetC');
my $Loc     =  $query->param('SetLc');

my @SetD = (
             $prodSer . "/2000/06",
             $prodSer . "/2000/07",
             $prodSer . "/2000/08", 
             $prodSer . "/2000/09" 
);

struct FilAttr => {
       flName   => '$',
       hpsize   => '$', 
       fpath    => '$',
       Nevts    => '$',
}; 
 
my $colls;
my $dPath;
if ($prodSer =~ /^P01/) {
  $dPath = "/2001/";
}
if ($prodSer eq "P01he" or $prodSer eq "P01hi" ) {
  $dPath = "/2000/";
}
if($colSet eq "AuAu130") {
  $colls = "AuAu1";
  $dPath = "/2000/";
}else{
  $colls = $colSet;
  $dPath = "/daq/";
}

#####  Find sets in DataSet table

 my $dstHpEvts = 0;
 my $daqHpEvts = 0;
 my $daqHpSize = 0;
 my $dstHpSize = 0;
 my $evtHpSize = 0;
 my $MuHpSize = 0;
 my $MuHpEvts = 0;
 my $emcHpSize = 0;
 my $emcHpEvts = 0;
 my $TdaqHpSize = 0;
 my $TdstHpSize = 0;
 my $TevtHpSize = 0;
 my $TMuHpSize = 0;
 my $TemcHpSize = 0;

 my $topHpss = "/home/starreco/reco";

#####  connect to RunLog DB

&StDbProdConnect();

##### select Geant files from FileCatalog

my $nmfile;
my @hpssInFiles;


#####  select DST files on HPSS from FileCatalog
my $dhfile;
my $dhpath;
my @OnlFiles;
my $nOnlFile = 0;
my @hpssDstFiles;

 $nhpssDstFiles = 0;



if ($trigD eq "all" and $fieldM eq "all"  and $detSet eq "all" ) {

 $sql="SELECT fName, size, path, Nevents  FROM $FileCatalogT WHERE fName LIKE '%.root' AND jobID LIKE '%$prodSer%' AND dataset like '$colls%' AND trigger <> 'n/a' AND site like '$Loc%'";
 
}elsif($trigD ne "all" and $fieldM eq "all" and $detSet eq "all") {

 $sql="SELECT fName, size, path, Nevents  FROM $FileCatalogT WHERE fName LIKE '%.root' AND jobID LIKE '%$prodSer%' AND dataset like '$colls%' AND trigger = '$trigD' AND site like '$Loc%' ";

}elsif ($trigD eq "all" and $fieldM ne "all" and $detSet eq "all" ) {

 $sql="SELECT fName, size, path, Nevents  FROM $FileCatalogT WHERE fName LIKE '%.root' AND jobID LIKE '%$prodSer%' AND dataset like '$colls%' AND trigger <> 'n/a' AND dataset like '%$fieldM%' AND site like '$Loc%'";
 
}elsif($trigD ne "all" and $fieldM ne "all" and $detSet eq "all") {

 $sql="SELECT fName, size, path, Nevents  FROM $FileCatalogT WHERE fName LIKE '%.root' AND jobID LIKE '%$prodSer%' AND dataset like '$colls%' AND trigger = '$trigD' AND dataset like '%$fieldM%' AND site like '$Loc%' ";

}elsif ($trigD eq "all" and $fieldM eq "all"  and $detSet ne "all" ) {

 $sql="SELECT fName, size, path, Nevents  FROM $FileCatalogT WHERE fName LIKE '%.root' AND jobID LIKE '%$prodSer%' AND dataset like '$colls%' AND trigger <> 'n/a' AND dataset like '%$detSet%'  AND site like '$Loc%'";
 
}elsif($trigD ne "all" and $fieldM eq "all" and $detSet ne "all") {

 $sql="SELECT fName, size, path, Nevents  FROM $FileCatalogT WHERE fName LIKE '%.root' AND jobID LIKE '%$prodSer%' AND dataset like '$colls%' AND trigger = '$trigD' AND dataset like '%$detSet%' AND site like '$Loc%' ";

}elsif ($trigD eq "all" and $fieldM ne "all" and $detSet ne "all" ) {

 $sql="SELECT fName, size, path, Nevents  FROM $FileCatalogT WHERE fName LIKE '%.root' AND jobID LIKE '%$prodSer%' AND dataset like '$colls%' AND trigger <> 'n/a' AND dataset like '%$fieldM%' AND dataset like '%$detSet%' AND site like '$Loc%'";
 
}elsif($trigD ne "all" and $fieldM ne "all" and $detSet ne "all") {

 $sql="SELECT fName, size, path, Nevents  FROM $FileCatalogT WHERE fName LIKE '%.root' AND jobID LIKE '%$prodSer%' AND dataset like '$colls%' AND trigger = '$trigD' AND dataset like '%$fieldM%' AND dataset like '%$detSet%' AND site like '$Loc%' ";
}



   $cursor =$dbh->prepare($sql)
     || die "Cannot prepare statement: $DBI::errstr\n";
   $cursor->execute;

   while(@fields = $cursor->fetchrow) {
      my $cols=$cursor->{NUM_OF_FIELDS};
       $fObjAdr = \(FilAttr->new());

      for($i=0;$i<$cols;$i++) {
        my $fvalue=$fields[$i];
         my $fname=$cursor->{NAME}->[$i];
       print "$fname = $fvalue\n" if $debugOn;

       ($$fObjAdr)->flName($fvalue)   if( $fname eq 'fName');
       ($$fObjAdr)->fpath($fvalue)    if( $fname eq 'path');
       ($$fObjAdr)->hpsize($fvalue)   if( $fname eq 'size');
       ($$fObjAdr)->Nevts($fvalue)    if( $fname eq 'Nevents');
     }
  
     $hpssDstFiles[$nhpssDstFiles] = $fObjAdr;  
     $nhpssDstFiles++;
   
    }

  foreach my $dsfile (@hpssDstFiles) {

      $dhfile = ($$dsfile)->flName; 
      $dhpath = ($$dsfile)->fpath;
     if ($dhfile =~ /.dst.root/) {
        $dstHpSize  += ($$dsfile)->hpsize;
   }elsif($dhfile =~ /.event.root/) {
        $evtHpSize  += ($$dsfile)->hpsize;
        $dstHpEvts  += ($$dsfile)->Nevts;  
   }elsif($dhfile =~ /.MuDst.root/) {
        $MuHpSize  += ($$dsfile)->hpsize;
        $MuHpEvts  += ($$dsfile)->Nevts;   
   }elsif($dhfile =~ /.emcEvent.root/) {
        $emcHpSize  += ($$dsfile)->hpsize;
        $emcHpEvts  += ($$dsfile)->Nevts;   
  }else{
   next;
 }
}
#####  select daq files from FileCatalog
 my $dqfile;
 my $dqpath;

if ($trigD eq "all" and $fieldM eq "all" and $detSet eq "all") {

  $sql="SELECT fName, path, size, Nevents  FROM $FileCatalogT WHERE trigger <> 'n/a' AND dataset <> 'tpc_laser' AND fName LIKE '%daq' AND path like '%$dPath%' AND dataset like '$colls%' AND site like 'hpss%' ";

}elsif ($trigD ne "all" and $fieldM eq "all" and $detSet eq "all") {

 $sql="SELECT fName, path, size, Nevents  FROM $FileCatalogT WHERE trigger = '$trigD' AND fName LIKE '%daq' AND path like '%$dPath%'  AND dataset like '$colls%' AND site like 'hpss%' ";

}elsif ($trigD eq "all" and $fieldM ne "all" and $detSet eq "all") {

 $sql="SELECT fName, size, path, Nevents  FROM $FileCatalogT WHERE fName LIKE '%.daq' AND path like '%$dPath%' AND trigger <> 'n/a' AND dataset like '%$fieldM%' AND dataset like '$colls%' AND site like 'hpss%'";
 
}elsif($trigD ne "all" and $fieldM ne "all" and $detSet eq "all") {

 $sql="SELECT fName, size, path, Nevents  FROM $FileCatalogT WHERE fName LIKE '%.daq' AND path like '%$dPath%' AND trigger = '$trigD' AND dataset like '%$fieldM%' AND dataset like '$colls%' AND site like 'hpss%' ";

}elsif ($trigD eq "all" and $fieldM eq "all" and $detSet ne "all") {

  $sql="SELECT fName, path, size, Nevents  FROM $FileCatalogT WHERE trigger <> 'n/a' AND dataset <> 'tpc_laser' AND fName LIKE '%daq' AND dataset like '%$detSet%' AND path like '%$dPath%' AND dataset like '$colls%' AND site like 'hpss%' ";

}elsif ($trigD ne "all" and $fieldM eq "all" and $detSet ne "all") {

 $sql="SELECT fName, path, size, Nevents  FROM $FileCatalogT WHERE trigger = '$trigD' AND fName LIKE '%daq' AND path like '%$dPath%'  AND dataset like '%$detSet%' AND dataset like '$colls%' AND site like 'hpss%' ";

}elsif ($trigD eq "all" and $fieldM ne "all" and $detSet ne "all") {

 $sql="SELECT fName, size, path, Nevents  FROM $FileCatalogT WHERE fName LIKE '%.daq' AND trigger <> 'n/a' AND path like '%$dPath%' AND dataset like '%$detSet%' AND  dataset like '%$fieldM%' AND dataset like '$colls%' AND site like 'hpss%'";
 
}elsif($trigD ne "all" and $fieldM ne "all" and $detSet ne "all") {

 $sql="SELECT fName, size, path, Nevents  FROM $FileCatalogT WHERE fName LIKE '%.daq' AND path like '%$dPath%' AND trigger = '$trigD' AND dataset like '%$detSet%' AND dataset like '%$fieldM%' AND dataset like '$colls%' AND site like 'hpss%' ";
}
   $cursor =$dbh->prepare($sql)
      || die "Cannot prepare statement: $DBI::errstr\n";
   $cursor->execute;

   while(@fields = $cursor->fetchrow) {
     my $cols=$cursor->{NUM_OF_FIELDS};
     $fObjAdr = \(FilAttr->new());

     for($i=0;$i<$cols;$i++) {
       my $fvalue=$fields[$i];
       my $fname=$cursor->{NAME}->[$i];
       print "$fname = $fvalue\n" if $debugOn;
# print "$fname = $fvalue\n";
       ($$fObjAdr)->flName($fvalue)   if( $fname eq 'fName');
       ($$fObjAdr)->fpath($fvalue)    if( $fname eq 'path');
       ($$fObjAdr)->hpsize($fvalue)   if( $fname eq 'size'); 
       ($$fObjAdr)->Nevts($fvalue)    if( $fname eq 'Nevents');
     }
  
     $OnlFiles[$nOnlFile] = $fObjAdr;  
     $nOnlFile++;
   
   }


  foreach my $onfile (@OnlFiles) {

     $dqfile = ($$onfile)->flName;
     $dqpath = ($$onfile)->fpath;
     $dqEvts = ($$onfile)->Nevts;
#     $dqEvts -= 1;

    $daqHpEvts  += $dqEvts;
    $daqHpSize  += ($$onfile)->hpsize; 

 }

    $TdaqHpSize = int($daqHpSize/1024/1024/1024);
    $TevtHpSize = int($evtHpSize/1024/1024/1024);
    $TdstHpSize = int($dstHpSize/1024/1024/1024);
    $TMuHpSize = int($MuHpSize/1024/1024/1024); 
    $TemcHpSize = int($emcHpSize/1024/1024/1024);       

&cgiSetup();
&beginHtml();

 &printTotal(); 

#####  finished with database
 &StDbProdDisconnect();
  
  &endHtml();

#######################

 sub printTotal {

print <<END;
<TR ALIGN=CENTER HEIGHT=80 bgcolor=lightblue>
<td HEIGHT=80><h3>$TdaqHpSize</h3></td>
<td HEIGHT=80><h3>$daqHpEvts</h3></td>
<td HEIGHT=80><h3>$TdstHpSize</h3></td>
<td HEIGHT=80><h3>$TevtHpSize</h3></td>
<td HEIGHT=80><h3>$TMuHpSize</h3></td>
<td HEIGHT=80><h3>$TemcHpSize</h3></td>
<td HEIGHT=80><h3>$dstHpEvts</h3></td>
<td HEIGHT=80><h3>$MuHpEvts</h3></td>
</TR>
END

}

######################
sub beginHtml {

print <<END;

  <html>
  <head>
          <title>Production Summary</title>
   </head>
   <body BGCOLOR=\"#ccffff\"> 
     <h2 align=center>Summary for $trigD events in $prodSer production </h2>
<TABLE ALIGN=CENTER BORDER=5 CELLSPACING=1 CELLPADDING=2 >
<TR>
<TD ALIGN=CENTER WIDTH=\"20%\" HEIGHT=100><B>Size(GB) of DAQ files</B></TD>
<TD ALIGN=CENTER WIDTH=\"20%\" HEIGHT=100><B>Number of Events<br>in DAQ files</B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=100><B>Size(GB) of dst.root files</B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=100><B>Size(GB) of event.root files</B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=100><B>Size(GB) of MuDst.root files</B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=100><B>Size(GB) of emcEvent.root files</B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=100><B>Number of Events<br>in event.root file</B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=100><B>Number of Events<br>in MuDst.root file</B></TD>
</TR> 

     <h2 align=center>Event Summary by Trigger Words  </h2>
<TABLE ALIGN=CENTER BORDER=5 CELLSPACING=1 CELLPADDING=2 >
<TR>
<TD ALIGN=CENTER WIDTH=\"20%\" HEIGHT=100><B>Events<br> with primary vertex</B></TD>
<TD ALIGN=CENTER WIDTH=\"20%\" HEIGHT=100><B>Hadronic MinBias</B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=100><B>Hadronic Central</B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=100><B>Hi-mult</B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=100><B>Hi-mult & ZDC </B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=100><B>UPC MinBias</B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=100><B>TOPO</B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=100><B>TOPO & ZDC</B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=100><B>TOPOeff</B></TD>
<TD ALIGN=CENTER WIDTH=\"10%\" HEIGHT=100><B>Laser Events</B></TD>
</TR> 

   </head>
    <body>
END
}


#####################
sub endHtml {
my $Date = `date`;

print <<END;
</TABLE>
      <h5>
      <address><a href=\"mailto:didenko\@bnl.gov\">Lidia Didenko</a></address>
<!-- Created: Wed July 26  05:29:25 MET 2000 -->
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












