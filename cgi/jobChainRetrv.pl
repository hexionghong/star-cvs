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

 my $q=new CGI;

 if ( exists($ENV{'QUERY_STRING'}) ) { print $q->header };

my $set = $q->param("set");

&beginHtml();

#################
sub beginHtml {

print <<END;
<html>
  <head>
          <title>Chain options</title>
  </head>
  <body BGCOLOR=\"cornsilk\"> 
<TABLE ALIGN=CENTER BORDER=5 CELLSPACING=1 CELLPADDING=2 >
<TR>
<TD  ALIGN=CENTER WIDTH=\"100%\" HEIGHT=50><h3>Chain options used for test</h3></TD>
</TR><TR>
<TD  ALIGN=CENTER WIDTH=\"100%\" HEIGHT=50><h3>$set</h3></TD>
</TR>

</TABLE>
   </body>
</html>
END
}

