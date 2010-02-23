#! /usr/local/bin/perl -w
#
#  
#
#  L. Didneko
#
###############################################################################

BEGIN {
 use CGI::Carp qw(fatalsToBrowser carpout);
}


use CGI;

&cgiSetup();
 

my $set = $q->param("set");


&beginHtml();

 &printRow(); 

&endHtml();


#################
sub beginHtml {

print <<END;
<html>
  <head>
          <title>Chain used for test</title>
  </head>
  <body BGCOLOR=\"#ccffff\"> 
<TABLE BORDER=5 CELLSPACING=1 CELLPADDING=2 >
<TR>
<TR ALIGN=CENTER VALIGN=CENTER>
<TD WIDTH=\"100%\" HEIGHT=50><h3>Chain options</h3></TD>
</TR>

END
}

###############
sub printRow {

print <<END;
<TR ALIGN=CENTER VALIGN=CENTER>
<td><h3>$set</h3></td>
</tr>
END

}

###############
sub endHtml {
my $Date = `date`;

print <<END;
</TABLE>
   </body>
</html>
END

}

##############
sub cgiSetup {
    $q=new CGI;
    if ( exists($ENV{'QUERY_STRING'}) ) { print $q->header };
}
