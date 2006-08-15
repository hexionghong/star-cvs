#!/usr/bin/env perl
#
#  
#
#  dbProdOptionRetrv.pl - script to retrive Production Option table
#
###############################################################################

use CGI;

require "/afs/rhic.bnl.gov/star/packages/cgi/dbCpProdSetup.pl";

my $debugOn = 0;
my %pair;

my $chain;

&cgiSetup();

&StDbProdConnect();

&beginHtml();

$sql="SELECT * FROM $ProdOptionsT ORDER BY id DESC ";
$cursor =$dbh->prepare($sql)
  || die "Cannot prepare statement: $DBI::errstr\n";
$cursor->execute;


while(@fields = $cursor->fetchrow) {
  my $cols=$cursor->{NUM_OF_FIELDS};

 for($i=0;$i<$cols;$i++) {
    my $fvalue=$fields[$i];
    my $fname=$cursor->{NAME}->[$i];
    print "$fname = $fvalue\n" if $debugOn;
    $pair{$fname} = $fvalue;
    $chain = $fvalue  if( $fname eq 'chainOpt');
  }
    if($chain =~ /ITTF/) {
   
 &printRow();

}else{
  &printRoww();
  }
 }

&endHtml();

&StDbProdDisconnect();

#################
sub beginHtml {

print <<END;
<html>
  <head>
          <title>Production Options</title>
  </head>
  <body BGCOLOR=\"cornsilk\"> 
<TABLE BORDER=5 CELLSPACING=1 CELLPADDING=2 >
<TR>
<TR ALIGN=CENTER VALIGN=CENTER>
<TD WIDTH=\"20%\" HEIGHT=50><B>prodSeries</B></TD>
<TD WIDTH=\"20%\" HEIGHT=50><B>eventType</B></TD>
<TD WIDTH=\"20%\" HEIGHT=50><B>libVersion</B></TD>
<TD WIDTH=\"20%\" HEIGHT=50><B>chainOpt</B></TD>
<TD WIDTH=\"20%\" HEIGHT=50><B>chainName</B></TD>
</TR>

END
}

###############
sub printRow {

print <<END;
<TR BGCOLOR=\"#D8BFD8\" ALIGN=CENTER VALIGN=CENTER>
<td>$pair{'prodSeries'}</td>
<td>$pair{'eventType'}</td>
<td>$pair{'libVersion'}</td>
<td>$pair{'chainOpt'}</td>
<td>$pair{'chainName'}</td>
</tr>
END

}

###############
sub printRoww {

print <<END;
<TR ALIGN=CENTER VALIGN=CENTER>
<td>$pair{'prodSeries'}</td>
<td>$pair{'eventType'}</td>
<td>$pair{'libVersion'}</td>
<td>$pair{'chainOpt'}</td>
<td>$pair{'chainName'}</td>
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
