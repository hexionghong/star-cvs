#!/usr/bin/env perl

# Will use nova monistoring information to display queue information. 
# This cgi may be moved to the local server as it will also have
# administrative tasks
#
# J. Lauret 2013
#

use CGI qw(:standard);
use DBI;

my $query = new CGI;
my $this_script= $query->url();


$title    = "Batch queue information at BNL - DISABLED";
$TEXTCOL  = "black";
$LINKCOL  = "navy";
$BGCOLOR  = "cornsilk"; 


print
    $query->header,
    $query->start_html(-title=>$title,
                       -AUTHOR=>$author,
                       -BGCOLOR=>$BGCOLOR,
                       -TEXT=>$TEXTCOL,
                       -LINK=>$LINKCOL),"\n",
    $query->h1($title)."\n";

print &MsgHdr()."As the farm expanded to a pool mechanism (all nodes shared across experiments), this script has been disabled. Please consult the <A HREF=\"/STAR/comp/sofi\">infrastructure page</A> for a new interface monitoring user NFS IO load.";

print $query->end_html;



sub MsgHdr
{
    return localtime()." - ";
}
