#!/usr/local/bin/perl

#
# Get a detail of the space on each disks
# Look 2 directory deep for what is there assuming
#
# $0 > /afs/rhic/star/doc/www/html/tmp/pub/overall.html
#

#
# List of disks will be by numbers
#
$MIN   =  1;  # 4
$MAX   =  6;  # for testing
$MAX   = 50;
$MAIN  = "/star/data";

@COLORS = ("#FFFACD","#C1FFC1","#7FFFD4","#00DEFF","#87CEFA","#ccccee","#D8BFD8","#FF69B4");

# Insert an extra table break before those numbers
$BREAK{"01"} =  "User Space";  
$BREAK{"03"} =  "Reserved Usage Space Area";  
$BREAK{"06"} =  "Production Disks / Assigned TEMPORARY space for Projects";  
$BREAK{"08"} =  "Production Disks";  

# Standard header style
$TD  = "<TD BGCOLOR=\"black\" align=\"center\"><FONT FACE=\"Arial, Helvetica\"><FONT COLOR=\"white\"><B>";
$ETD = "</FONT></B></FONT></TD>";


for ( $i = $MIN ; $i <= $MAX ; $i++){
    push(@DISKS,sprintf("$MAIN%2.2d",$i));
}

foreach $disk (@DISKS){
    if ( ! -e "$disk" && ! -e "$disk/."){  next;}  # -l may be a unmounted disk
    if ( ! -d "$disk/." ){                         # this is definitly not mounted
	$DINFO{$disk} = "?;?;?;?;".
	    "<BLINK><B><FONT COLOR=#FF0000>Offline or bad mount point".
		"</FONT></B></BLINK>; ";
	next;
    }


    chomp($res = `df -k $disk | grep % | grep '/'`);
    $res   =~ s/^\s*(.*?)\s*$/$1/;
    $res   =~ s/\s+/ /g;
    @items =  split(" ",$res);

    #print STDERR "$disk $res\n";

    if ($#items < 5){
	$tota = $items[0];
	$used = $items[1];
	$avai = $items[2];
	$prct = $items[3];
    } else {
	$tota = $items[1];
	$used = $items[2];
	$avai = $items[3];
	$prct = $items[4];
    }
    #print STDERR "\t$tota $used $avai $prct\n";

    # Now scan the disk for a reco directory
    undef(@TRGS);
    undef(%LIBS);

    # Logic sorting our the trigger level and the library
    # level. This implies a strict naming convention.
    if( -d "$disk/reco"){
	@TMP = glob("$disk/reco*/*");
	foreach $trg (@TMP){
	    #print "Found $trg\n";
	    if ($trg =~ /StarDb/){ next;}
	    $tmp = $trg;
	    $tmp =~ s/.*\///;
	    push (@TRGS,$tmp);
	    @vers = glob("$trg/*/*");
	    foreach $ver (@vers){
		if (! -d $ver && ! -l $ver){ next;}
		#print "\tFound $ver\n";
		$ver =~ s/.*\///;
		$LIBS{$ver} = 1;
		if ( defined($ALIBS{$ver}) ){
		    $ALIBS{$ver} .= "$tmp;$disk ";
		} else {
		    $ALIBS{$ver}  = "$tmp;$disk ";
		}
	    }
	}

    }


    $DINFO{$disk} = "$tota;$used;$avai;$prct;";
    $trg = " ";
    foreach $tmp (@TRGS){
	$trg .= "$tmp ";
    }
    if ( -e "$disk/AAAREADME"){
	@all = `cat $disk/AAAREADME`;
	$DINFO{$disk} .= "<B><PRE>".join("",@all)."</PRE></B>";
    }
    $DINFO{$disk} .= "$trg;";

    $ver = " ";
    foreach $tmp (keys %LIBS){
	$ver .= "<A HREF=\"#".&GetRef($tmp)."\">$tmp</A> ";
    }
    $DINFO{$disk} .= "$ver;";
    #print STDERR "$disk --> $DINFO{$disk}\n";
}

if ( defined($ARGV[0]) ){
    open(FO,">$ARGV[0]-tmp") || die "Could not open $ARGV[0]-tmp\n";
    $FO = FO;
} else {
    $FO = STDOUT;
}

print $FO
    "<HTML>\n",
    "<HEAD><TITLE>Disk space overview</TITLE></HEAD>\n",
    "<BODY>\n",
    "<H1 ALIGN=\"center\">Disk space overview</H1>\n",
    "<h5 align=\"center\">Generated on ".localtime()."</h5>\n",
    "<H1>Information</H1>\n",
    "<OL>\n",
    "<LI><A HREF=\"#DSO\">Disk Space Overview</A>",
    "<LI><A HREF=\"#PLOC\">Production locations</A>",
    "<LI><A HREF=\"#SUM\">Summary</A>",
    "</OL>\n",
    "\n",
    "<H2><A NAME=\"DSO\">Disk space overview</A></H2>\n",
    "Note that each disk may contain other directories than reco*/. Those ",
    "are the only ones scanned ...\n",
    "The reported structure reflects a tree assumed to be of the form ",
    " Trigger/Field/Production.\n",
    "<P>\n",
    "<TABLE border=\"0\">\n<TR><TD>Color Scale</TD>\n";

for ($i=0 ; $i <= $#COLORS ; $i++){
    $low  = int(100*$i/($#COLORS+1));
    $high = int(100*($i+1)/($#COLORS+1));
    print $FO "\t<TD BGCOLOR=\"$COLORS[$i]\">$low - $high</TD>\n";
}

print $FO 
    "</TR>\n</TABLE>\n",
    "<P>\n",
    "<TABLE border=\"1\" cellspacing=\"0\" width=\"50\">\n";


printf $FO
    "<TR>$TD%10s$ETD $TD%11s$ETD $TD%11s$ETD $TD%11s$ETD $TD%3s$ETD $TD%s$ETD $TD%s$ETD</TR>\n",
    "Disk","Total","Used","Avail","Used %","Triggers","Libs";


$col     = 0;
@$totals = (0,0,0);
foreach $disk (sort keys %DINFO){

    #print STDERR "$DINFO{$disk}\n";
    @items = split(";",$DINFO{$disk});
    $items[4] =~ s/\s/&nbsp; /;
    $items[5] =~ s/\s/&nbsp; /;

    $icol =  $items[3];
    $icol =~ s/%//;
    #print "$icol ";
    $col =  int( ($#COLORS+1) * ( $icol /100.0));
    #print "$col ";
    if( $icol >= 99 ){
	$col = "red";
    } else {
	$col = $COLORS[$col];
    }

    #print "$col\n";
    $disk =~ m/(\d+)/;
    if ( defined($BREAK{$1}) ){
	printf $FO 
	    "<TR BGCOLOR=\"#333333\"><TD ALIGN=\"center\" COLSPAN=\"7\">".
	    "<FONT COLOR=\"white\"><B>$BREAK{$1}</B></FONT></TD></TR>\n";
    }
    printf $FO
	"<TR bgcolor=\"$col\">\n".
	"  <TD align=\"right\"><A NAME=\"%s\">%10s</A></TD>\n".
	"  <TD align=\"right\">%11s</TD>\n".
	"  <TD align=\"right\">%11s</TD>\n".
	"  <TD align=\"right\">%11s</TD>\n".
	"  <TD align=\"right\">%3s</TD>\n".
	"  <TD>%s</TD>\n".
	"  <TD align=\"right\">%s</TD>\n".
	"</TR>\n",
	    &GetRef($disk),"<i><b>$disk</b></i>",$items[0],$items[1],$items[2],
	    $items[3],$items[4],$items[5];

    $totals[0] += $items[0];
    $totals[1] += $items[1];
    $totals[2] += $items[2];

    #$col++;
    #if($col > $#COLORS){ $col = 0;}
}


print $FO
    "</TABLE>\n",
    "<P>",
    "<H2><A NAME=\"PLOC\">Production Location</A></H2>\n",
    "<TABLE align=\"center\" cellspacing=\"0\" border=\"1\" width=\"750\">\n",
    "<TR>$TD Production $ETD$TD Trigger setup $ETD$TD Location list$ETD</TR>\n";

undef(@LINES);
foreach $tmp (sort keys %ALIBS){
    @items = split(" ",$ALIBS{$tmp}); undef(%UD);
    foreach $disku (sort(@items)){ 
	($trg,$disk) = split(";",$disku);
	if ( ! defined($TR{$tmp.$trg}) ){
	    if ($#LINES != -1){ 
		push(@LINES,"</TD>\n</TR>\n");
		# make a separator at each new library
		if ( ! defined($LB{$tmp}) ){
		    push(@LINES,"<TR BGCOLOR=\"#333333\" HEIGHT=\"1\"><TD COLSPAN=\"3\">".
			 "</TD></TR>\n");
		}
	    }
	    push(@LINES,
		 "<TR>\n",
		 "\t<TD BGCOLOR=\"#DDDDDD\"><FONT FACE=\"Arial, Helvetica\">".
		 (defined($LB{$tmp})?"&nbsp;":"<A NAME=\"".&GetRef($tmp)."\">$tmp</A>")."</FONT></TD>\n".
		 "\t<TD BGCOLOR=\"#EEEEEE\">$trg</TD>\n".
		 "\t<TD BGCOLOR=\"#EEEEEE\">");
	    $TR{$tmp.$trg} = 1;
	    $LB{$tmp}      = 1;
	}
	if ( ! defined($UD{$tmp.$trg.$disk}) ){
	    push(@LINES,"<A HREF=\"#".&GetRef($disk)."\">$disk</A> ");
	    $UD{$tmp.$trg.$disk} = 1;
	}
    }
}
push(@LINES,"</TD>\n</TR>\n");
foreach $tmp (@LINES){  print $FO $tmp;}


print $FO
    "</TABLE>\n",
    "<P>",
    "<H2><A NAME=\"SUM\">Summary</A></H2>\n",
    "<BLOCKQUOTE>\n",
    "Total Space = ".sprintf("%.2f",$totals[0]/1024/1024)." GB<br>\n",
    "Total Used =  ".sprintf("%.2f",$totals[1]/1024/1024)." GB<br>\n",
    "Available  =  ".sprintf("%.2f",$totals[2]/1024/1024)." GB<br>\n",
    "</BLOCKQUOTE>\n",
    "</BODY>\n",
    "</HTML>\n";

if ( defined($ARGV[0]) ){
    if ( -e $ARGV[0]){ 
	# delete if exits the preceedingly renamed file
	if ( -e "$ARGV[0]-old"){ unlink("$ARGV[0]-old");}
	# move the file to a new target name
	if ( ! rename("$ARGV[0]","$ARGV[0]-old") ){
	    # if we cannot rename() try to delete
	    unlink($ARGV[0]);
	}
    }
    # move new file to target
    rename("$ARGV[0]-tmp","$ARGV[0]");
}




sub GetRef
{
    my($el)=@_;
    
    $el =~ s/[\/ ]/_/g;
    $el;
}
