#!/opt/star/bin/perl 
##
# $Id: dbGetConfig.pl,v 1.1 2000/04/28 14:08:03 porter Exp $
#
# Author: Bum Choi & R. Jeff Porter
#
#***************************************************************************
#
# Description: parses node relations from mysql into an xml format
#
#****************************************************************************
# 
# $Log: dbGetConfig.pl,v $
# Revision 1.1  2000/04/28 14:08:03  porter
# management perl scripts for db-structure accessible from StDbLib
#
#
#####################################
#===================================================================
# package ; (maybe later)
#===================================================================
use DBI;
#use strict;
use Getopt::Std;
use FileHandle;
#===================================================================
# -d databasename -h help 
getopts('hd:v:s:');
my $dbName        = $opt_d;
my $serverHost    = $opt_s;
my $configVersion = $opt_v;

($opt_h or not defined $dbName or not defined $serverHost) and Usage();
#-------------------------------------------------------------------
my ($dbh, $sth1);  
my ($query, $check);
my ($configname, $configversion, $configid);
my $xml_fh;  # handle used in all the print statements
#-------------------------------------------------------------------
$dbh = DBI->connect("DBI:mysql:$dbName:$serverHost",
		       {RaiseError => 1, AutoCommit => 0, Trace=>2})
  or die "couldnt connect to $dbName: $dbh->errstr\n";
#------------------------------------------------------------------- 
# bunch of checks to see that infact $configVersion is a 'Config' parent
if (defined $configVersion) {
    $query = qq{select distinct n.name, n.versionkey, n.id } .
	qq{from Nodes as n, NodeRelation as r } .
	qq{where n.id = r.parentid and n.nodetype = 'Config' } .
	qq{and n.versionkey = '$configVersion'};
} else {	
# get id of config nodes that are in NodeRelation as a parent 
    $query = qq{select distinct n.name, n.versionkey, n.id } .
	     qq{from Nodes as n, NodeRelation as r  } . 
             qq{where n.id = r.parentid and n.nodetype = 'Config'};
}
$sth1=$dbh->prepare($query);
$sth1->execute;

#------------------------------------------------------------------
$check = undef;
#$indent=" ";
while(($configname, $configversion, $configid) = $sth1->fetchrow_array){
      OpenFile("$dbName"."_$configversion");
      HeaderStart();
      MarkerStart($configname,"dbNode"," ");
      MarkerBoth($configversion,"version","");
      NextLine();
      my $tmpID = 'None';
      my $indent = 3;

      DoIt($configid,$tmpID,$indent);

      MarkerEnd("dbNode"," ");
      NextLine();
      HeaderEnd();
      $check = 'true';
}
defined $check or die "Couldnt find configversion $configVersion";
#---------------------------------------------------------------------
$dbh->disconnect;
#======================================================================
sub OpenFile{
    my $arg =  shift;
    my $filename = $arg . '_Config.xml'; 
    $xml_fh = new FileHandle; 
    open $xml_fh, ">$filename";
}
sub NextLine{
    print $xml_fh "\n";
}
sub Tab{
    print $xml_fh "\t";
}
sub HeaderStart{
    MarkerStart($dbName,"StarDataBase"), NextLine();
}
sub HeaderEnd{
    MarkerEnd("StarDataBase");
}
sub MarkerStart{
    my ($value, $marker, $mshft) = @_;
    print $xml_fh "$mshft <$marker> $value ";
}
sub MarkerEnd{
    my ($marker, $mshft) = @_;
#    my $marker = shift;
    print $xml_fh "$mshft </$marker>"; NextLine();    
}
sub MarkerBoth{
    my ($value, $marker, $mshft) = @_;
    print $xml_fh "$mshft <$marker> $value </$marker> ";
}
sub Usage{
    print "\n";
    print "Usage   : dbGetConfig.pl -d database -s server -v versionKey \n";
    print "                 -s server has the form 'hostname:portnumber' or simply\n";
    print "                 'hostname' if portnumber=3306\n";
    print "                 -d database name\n";
    print "                 -v versionKey of configuration \n";
    print "                 -h for this help\n\n";
    exit;
}
sub DoIt{
    # initially take the parentid of nodetype config
    my $parentid = shift;
    my $peID = shift;
    my $mindent = shift;
    my $mshift;
    my $mnextindent = $mindent+3;
    for($i=0;$i<$mindent;$i++){ $mshift = join(" ",$mshift,""); }

    my ($name, $version, $nodetype, $nodeid, $structName, $elementID);
    # each recursion needs its own statement handle
    my $sth = $dbh->prepare(qq{select n.name, n.versionkey, n.structName, n.elementID, r.nodeid } .
		         qq{from Nodes as n, NodeRelation as r } .
		         qq{where r.nodeid = n.id and r.parentid = $parentid }.
			 qq{and n.nodetype = ?});
    # to print in the order of 'DB', 'table', 'directory'
    foreach $nodetype ('DB', 'table', 'directory'){
 
	$sth->execute($nodetype);
	while(($name, $version, $structName, $elementID, $nodeid) = 
	      $sth->fetchrow_array){
	    MarkerStart($name,"dbNode",$mshift);
	    MarkerBoth($structName,"StDbTable","") if $nodetype eq 'table';
        if($peID eq 'None' && !($nodetype eq 'directory')){
	      MarkerBoth($elementID,"elementID","") if !($elementID eq 'None');
        } else {
	      MarkerBoth($peID,"elementID","") if !($peID eq 'None');
        }
	    MarkerBoth($version,"version","") if !($version eq 'default');
        NextLine() if $nodetype eq 'directory';
	   DoIt($nodeid,$elementID,$mnextindent) if $nodetype eq 'directory';
	    if($nodetype eq 'directory') {
            MarkerEnd("dbNode",$mshift);
        } else {
            MarkerEnd("dbNode","");
        }
	}
    }
    $sth->finish;
}











