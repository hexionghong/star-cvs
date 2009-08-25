#!/usr/local/bin/perl

#
# Get a detail of the space on each disks
# Look 2 directory deep for what is there assuming
#
# $0 > /afs/rhic.bnl.gov/star/doc/www/html/tmp/pub/overall.html
#
#
# Hidden assumption is that  if a file named FC_$disk.txt or
# FC_$disk.html exists in $OUTD, a reference to it will appear.
#
# if dfpanfs exits, takes advantage of it.
#



# List of disks will be by numbers
$MIN   =  1;                                             # 4
$MAX   =  6;                                             # for testing
$MAX   = 85;                                             # Upper number ; can be as high
$MAIN  = "/star/data";                                   # default base path
@ADDD  = ("/star/institutions/*");                       # will be used in a glob statement


# Static configuration
$OUTD  = "/afs/rhic.bnl.gov/star/doc/www/html/tmp/pub/Spider"; # this will be used for Catalog hand-shake
$ICON1 = "/icons/transfer.gif";                                # Icon to display for indexer result
$ICON2 = "/images/Spider1.jpg";                                # Icon to display for spider result

$SpiderControl = "/cgi-bin/%%RELP%%/SpiderControl.cgi"; # a CGI controling the spiders

$DINFO = "(check '<A HREF=\"$SpiderControl\">nova</A>' Spiders)"; # Many tools may be used for indexing
                                                               # display info about which one.
$DINFO =~ s/%%RELP%%/public/;


@COLORS = ("#7FFFD4","#40E0D0","#00DEFF","#87CEFA","#CCCCEE","#D8BFD8","#DB7093"); #"#D02090"); #"#FF69B4");
$RED = "#B03060";

$UPGO = "<A HREF=\"#TOP\"><IMG ALT=\"[TOP]\"  BORDER=\"0\" SRC=\"/icons/up.gif\"   WIDTH=10></A>";
$DOWN = "<A HREF=\"#BOT\"><IMG ALT=\"[DOWN]\" BORDER=\"0\" SRC=\"/icons/down.gif\" WIDTH=10></A>";


# Insert an extra table break before those numbers
$BREAK{"01"}   =  "User Space";
$BREAK{"03"}   =  "Reserved Usage Space Area";
$BREAK{"06"}   =  "Production Disks / Assigned TEMPORARY space for Projects";
$BREAK{"09"}   =  "Production Disks";

# Addiitonal header based on patterns
$BHEAD{"inst"} =  "Institution disks";
$BEND          = "#terminate header#" ;  # a random header pattern indicating it will not be re-used

# A generic tag for addiitonal intremediate markers
$TAG           = "Disk_Group_";


# Exclude those completely (alias, un-usable etc ...)
#$DEXCLUDE{"46"} = 1;


# Added 2005 to skip searching the root dir for the README.
# data46 for example could not be scanned at its root by any tool whenever
# mounted over NFS (was PANFS).
$RDMEXCLUDE{"/star/data46"} = 1;


# Standard header style
$TD  = "<TD BGCOLOR=\"black\" align=\"center\"><FONT FACE=\"Arial, Helvetica\"><FONT COLOR=\"white\"><B>";
$ETD = "</FONT></B></FONT></TD>\n\t";


# Re-define DF command to be the generic dfpanfs command
$DF = "/bin/df -k";
$0  =~ m/(.*\/)(.*)/;
if ( -e $1."dfpanfs"){
    $DF = $1."dfpanfs";
}



for ( $i = $MIN ; $i <= $MAX ; $i++){
    push(@DISKS,sprintf("$MAIN%2.2d",$i));
}
if ( $#ADDD != -1 ){
    foreach $i (@ADDD){
	push(@DISKS,glob($i));
    }
}

foreach $disk (@DISKS){
    if ( ! -e "$disk" && ! -e "$disk/."){  next;}  # -l may be a unmounted disk
    if ( ! -d "$disk/." ){                         # this is definitly not mounted
	$DINFO{$disk} = "?;?;?;?;".
	    "<BLINK><B><FONT COLOR=#FF0000>Offline or bad mount point".
		"</FONT></B></BLINK>; ";
	next;
    }

    # print "DEBUG Checking $disk using $DF\n";
    chomp($res = `$DF $disk | /bin/grep % | /bin/grep '/'`);
    $res   =~ s/^\s*(.*?)\s*$/$1/;
    $res   =~ s/\s+/ /g;
    @items =  split(" ",$res);

    # print STDERR "$disk $res\n";

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
    # print STDERR "\t$tota $used $avai $prct\n";

    # Now scan the disk for a reco directory
    undef(@TRGS);
    undef(%LIBS);

    # Logic sorting our the trigger level and the library
    # level. This implies a strict naming convention.
    if( -d "$disk/reco"){
	# print "DEBUG $disk -d reco OK\n";
	@TMP = glob("$disk/reco/*");
	# print "DEBUG:: glob returned $#TMP args\n";
	foreach $trg (@TMP){
	    # print "Found $trg\n";
	    if ($trg =~ /StarDb/){ next;}
	    $tmp = $trg;
	    $tmp =~ s/.*\///;
	    push (@TRGS,$tmp);

	    # print "DEBUG Looking now in $trg\n";
	    @vers = glob("$trg/*/*");
	    foreach $ver (@vers){
		if (! -d $ver && ! -l $ver){ next;}
		# print "\tFound $ver\n";
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

    # print "DEBUG Will now search for a README file on $disk\n";

    if ( ! defined($RDMEXCLUDE{$disk}) ){
	if ( -e "$disk/AAAREADME"){
	    @all = `/bin/cat $disk/AAAREADME`;

	    $tmp = "<FONT SIZE=\"-1\" FACE=\"helvetica,sans-serif\">\n";
	    foreach $l (@all){
	  	chomp($l); 
		if ( length($l) >= 80){
		    $l = substr($l,0,75)." [...]";
		}
		$l =~ s/\t/&nbsp;/g;
		$l =~ s/\s/&nbsp;/g;
		$l =~ s/^(.*?)\s*$/$1/g;
		$tmp .= "&nbsp; $l<BR>\n";
	    }
	    $tmp .= "</FONT>\n";
	    $README{$disk} = $tmp;
	    
	}
    }

    #
    # Add the list of trigger setup
    #
    if ( $trg !~ m/^\s*$/ ){
	$DINFO{$disk} .= "<FONT SIZE=\"-1\" FACE=\"verdana\">$trg</FONT>";
    }
    $DINFO{$disk} .= ";";

    
    
    # print "DEBUG Done and ready to format information ...\n";

    $ver = " ";
    foreach $tmp (keys %LIBS){
	$ver .= "<A HREF=\"#".&GetRef($tmp)."\">$tmp</A> ";
    }
    $DINFO{$disk} .= "$ver;";
    # print STDERR "$disk --> $DINFO{$disk}\n";
}

if ( defined($ARGV[0]) ){
    open(FO,">$ARGV[0]-tmp") || die "Could not open $ARGV[0]-tmp\n";
    $FO = FO;
} else {
    $FO = STDOUT;
}

print $FO
    "<HTML>\n",
    "<HEAD><TITLE>Disk space overview</TITLE>\n",
    "      <META http-equiv=\"Refresh\" content=\"1800\">\n",
    "</HEAD>\n",
    "<BODY>\n",
    "<BASEFONT FACE=\"verdana,arial,helvetica,sans-serif\">\n",
    "<H1 ALIGN=\"center\">Disk space overview</H1>\n",
    "<A NAME=\"#TOP\"><H5 align=\"center\">Generated on ".localtime()."</H5></A>\n",
    "<H1>Information</H1>\n",
    "<OL>\n",
    "<LI><A HREF=\"#DSO\">Disk Space Overview</A>",
    "<LI><A HREF=\"#FCREF\">Disk needing indexing</A>",
    "<LI><A HREF=\"#PLOC\">Production locations</A>",
    "<LI><A HREF=\"#SUM\">Summary</A>",
    "</OL>\n",
    "\n",
    "<H2><A NAME=\"DSO\">Disk space overview</A></H2>\n",
    "Note that each disk may contain other directories than reco*/. Those ",
    "are the only ones scanned ...\n",
    "The reported structure reflects a tree assumed to be of the form ",
    " Trigger/Field/Production.\n",
    "<UL>\n";

# add markers
foreach $tmp (keys %BREAK){
    print $FO "<LI><A HREF=\"\#$TAG$tmp\">$BREAK{$tmp}</A>\n";
}
foreach $tmp (keys %BHEAD){
    print $FO "<LI><A HREF=\"\#$TAG$tmp\">$BHEAD{$tmp}</A>\n";
}

print $FO
    "</UL>",
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
    "<TABLE frame=\"border\" cellspacing=\"0\">\n"; # rules="rows"


printf $FO
    "<TR>$TD%10s$ETD $TD%11s$ETD $TD%11s$ETD $TD%3s$ETD $TD%s$ETD $TD%s$ETD</TR>\n",
    "Disk","Total","Used &amp; Avail","%Used","Triggers &amp Policies","Libs";

$idx     = 0;
$col     = 0;
@$totals = (0,0,0);
foreach $disk (sort keys %DINFO){

    # print STDERR "$DINFO{$disk}\n";
    @items = split(";",$DINFO{$disk});

    # 4 and 5 are the message and library listing
    # print "[$items[4]]\n";
    $items[4] =  "&nbsp;" if ( $items[4] =~ m/^\s*$/);
    $items[5] =  "&nbsp;" if ( $items[5] =~ m/^\s*$/);

    $icol =  $items[3];
    $icol =~ s/%//;
    # print "$icol ";
    $col =  int( ($#COLORS+1) * ( $icol /100.0));
    # print "$col ";
    if( $icol >= 99 ){
	$col = $RED;
    } else {
	$col = $COLORS[$col];
    }

    # print "$col\n";

    if ( $disk =~ m/(\d+)/ ){
	if ( defined($DEXCLUDE{$1} ) ){  next;}
	if ( defined($BREAK{$1}) ){
	    printf $FO
		"<TR BGCOLOR=\"#333333\">".
	        "  <TD ALIGN=\"center\" COLSPAN=\"6\">".
		"     <FONT COLOR=\"white\" FACE=\"arial\"><A NAME=\"$TAG$1\">$BREAK{$1}</A></FONT>".
	        "  </TD>\n".
	        "</TR>\n";
	}
    } else {
	foreach $tmp (keys %BHEAD){
	    if ( $BHEAD{$tmp} ne $BEND ){
		printf $FO
		    "<TR BGCOLOR=\"#333333\">".
		    "  <TD ALIGN=\"center\" COLSPAN=\"6\">".
		    "     <FONT COLOR=\"white\" FACE=\"arial\"><B><A NAME=\"$TAG$tmp\">$BHEAD{$tmp}</A></B></FONT>".
		    "  </TD>\n".
		    "</TR>\n";
		$BHEAD{$tmp} = $BEND;
		last;
	    }
	}
    }

    
    $idx++;
    $FCRef = &GetFCRef("FC",$ICON1,$disk);
    if ($FCRef ne ""){
	$FCRef = "<A HREF=\"#FCREF\">$FCRef</A>";
    }
    $dcol  = "#EFEFEF";
    
    if ( defined($README{$disk}) ){
	$dskinfo = $README{$disk}.$items[4];
    } else {
	$dskinfo = $items[4];
    }
    
    #
    # This is where the lines from the main table get formatted
    #
    # "<TR onMouseOver=\"style.backgroundColor='$col'\" onmouseout=\"style.backgroundColor='$dcol'\">\n".
    printf $FO
        "<TR><TD COLSPAN=\"6\"><A NAME=\"%s\"></TD></TR>\n".
        "<TR>\n".
	"  <TD align=\"right\" BGCOLOR=\"#DDDDDD\"><B> %10s </B>                                    </TD>\n".
	"  <TD align=\"right\" BGCOLOR=\"$col\"> %11s<br>(<I>%5.2f TB</I>)                          </TD>\n".
	"  <TD align=\"right\" BGCOLOR=\"$col\"> %11s<BR>%11s                                       </TD>\n".
	"  <TD align=\"right\" BGCOLOR=\"$col\"> <B> %3s </B>                                       </TD>\n".
	"  <TD BGCOLOR=\"$col\">                 %s                                                 </TD>\n".
	"  <TD align=\"right\" BGCOLOR=\"$col\"> <FONT SIZE=\"-1\">%s%s%s</FONT>                    </TD>\n".
	"</A></TR>\n",
	&GetRef($disk),$disk,
        $items[0],($items[0]/1024/1024/1024), 
        $items[1],$items[2],
        $items[3], 
        $dskinfo,
        $FCRef,(($FCRef eq "")?"":"<BR>"),$items[5];

    $totals[0] += $items[0];
    $totals[1] += $items[1];
    $totals[2] += $items[2];

    if ( $FCRef ne ""){
	push(@FCRefs,$FCRef." $disk ");
    }

    #$col++;
    #if($col > $#COLORS){ $col = 0;}
}



#
# FileCatalog index references
#
print $FO
    "</TABLE>\n",
    "<P>\n",
    "<H2><A NAME=\"FCREF\">Disk needing indexing</A> $UPGO</H2>\n";

if ($#FCRefs == -1){
    # not as straight forward
    print $FO "<I>Catalog is up-to-date or indexing daemon are down $DINFO.</I>\n";

} else {
    print $FO
	"<TABLE ALIGN=\"center\" CELLSPACING=\"0\"  BORDER=\"0\">\n",
	"  <TR>\n".
	"      $TD Disk    $ETD\n".
	"      $TD Indexer $ETD\n".
	"      $TD Spider  $ETD\n".
	"      <TD> &nbsp  </TD>\n".
	"      $TD Disk    $ETD\n".
	"      $TD Indexer $ETD\n".
	"      $TD Spider  $ETD\n".
	"      <TD> &nbsp  </TD>\n".
	"      $TD Disk    $ETD\n".
	"      $TD Indexer $ETD\n".
	"      $TD Spider  $ETD\n".
	"</TR>\n";

    $ii = 0;
    foreach $line (@FCRefs){
	$line  =~ m/(.*>\s+)(.*)(\s+)/;
	($ind,$disk) = ($1,$2);
	$refdisk = $disk;
	$refdisk =~ s/\//_/g;

	$FCRef = &GetFCRef("SD",$ICON2,$disk);
	if ( $FCRef ne ""){
	    $Info  = "<A HREF=\"$SpiderControl?disk=$refdisk&action=view\">$FCRef</A>";
	} else {
	    $Info  = "<A HREF=\"$SpiderControl?disk=$refdisk&action=ON\"><i>off</i></A>";
	}
	$ind = "<A HREF=\"$SpiderControl?disk=$refdisk&action=view\"><i>$ind</i></A>";

	$ind  =~ s/%%RELP%%/public/;
	$Info =~ s/%%RELP%%/protected/;

	if ($ii % 3 == 0){ print $FO "<TR>\n";}
	print $FO
	    "    <!-- $ii -->\n".
	    "    <TD BGCOLOR=\"#DDDDDD\">$disk</TD>\n".
	    "    <TD BGCOLOR=\"#EFEFEF\" ALIGN=\"middle\">$ind</TD>\n".
	    "    <TD BGCOLOR=\"#EFEFEF\" ALIGN=\"middle\">$Info</TD>\n";

	if (($ii+1) % 3 == 0){
	    print $FO "</TR>\n";
	} else {
	    print $FO "    <TD>&nbsp;</TD>\n";
	}
	$ii++;
    }
    if ($ii % 3 == 0){
	for ($ii=0 ; $ii < 2 ; $ii++){
	    print $FO
		"    <TD>&nbsp;</TD>\n".
		"    <TD>&nbsp;</TD>\n".
		"    <TD>&nbsp;</TD>\n".
		"    <TD>&nbsp;</TD>\n";
	}
	print $FO "</TR>\n";
    } elsif ($ii % 2 == 0){
	print $FO
	    "    <TD>&nbsp;</TD>\n".
	    "    <TD>&nbsp;</TD>\n".
	    "    <TD>&nbsp;</TD>\n".
	    "    <TD>&nbsp;</TD>\n".
	    "</TR>\n";
    }
    print $FO "</TABLE>\n";
}

# added this test in 2009
my(@killed)=glob("$OUTD/*.kill");
if ( $#killed != -1 ){
    print $FO
	"<P ALIGN=\"center\">\n".
	"  <B>WARNING</B><BR>\n".
	"  <I>Several daemon could not accomplish their task within timeout, a summary\n".
	"     of which is below</I><BR>\n".
	"  This may be caused by a high load of the FS or the FileCatalog DB, large FS\n".
	"  or many file modifications\n".
	"</P>\n".
	"\n".
	"<TABLE ALIGN=\"center\" BORDER=\"0\" CELLSPACING=\"0\">\n".
	"\t<TR>".
	"\t    <!-- column 1 -->\n".
	"\t    $TD Disk       $ETD\n".
	"\t    $TD Last tried $ETD\n".
	"\t    $TD Timeout    $ETD\n".
	"\t    $TD Mode       $ETD\n".
	"\t    $TD Node       $ETD\n".
	"\t    <TD>&nbsp;     </TD>\n".
	"\t    <!-- column 2 -->\n".
	"\t    $TD Disk       $ETD\n".
	"\t    $TD Last tried $ETD\n".
	"\t    $TD Timeout    $ETD\n".
	"\t    $TD Mode       $ETD\n".
	"\t    $TD Node       $ETD\n".
	"\t    <TD>&nbsp;     </TD>\n".
        "\t</TR>\n";

    $ii = 0;
    $col = "white";
    foreach $file (@killed){
	$file =~ m/(.*)(FC|AS|CK)(.*)(\..*)/;
	$disk = $3;  $disk =~ s/_/\//g;
	$node = $mode = $to = $date = "unknown";
	if ( open(FX,$file) ){
	    while ( defined($line = <FX>) ){
		if ($line =~ m/(failed with Mode=)(.*)/i){
		                               $mode = $2;
		                               $date = <FX>; next;}  # assumed to be the next line
		if ($line =~ m/(Node=)(.*)/){  $node = $2;   next;}
		if ($line =~ m/(\d+)( sec)/){  $to   = $1;   next;}

	    }
	    close(FX);
	}
	chomp($date); chomp($to); chomp($node);

	if ($ii == 0){
	    $ii = 1;
	    print $FO
		"\t<TR>\n".
		"\t    <!-- column 1 -->\n".
		"\t    <TD BGCOLOR=\"$col\"><font size=\"-2\">$disk</font></font> </TD>\n". # 1
		"\t    <TD BGCOLOR=\"$col\"><font size=\"-2\">$date</font></font> </TD>\n". # 2
		"\t    <TD BGCOLOR=\"$col\"><font size=\"-2\">$to</font></font>   </TD>\n". # 3
		"\t    <TD BGCOLOR=\"$col\"><font size=\"-2\">$mode</font></font> </TD>\n". # 4
		"\t    <TD BGCOLOR=\"$col\"><font size=\"-2\">$node</font></font> </TD>\n". # 5
		"\t    <TD><font size=\"-2\">&nbsp;</font></TD>\n";                         # 6

	    if ($col eq "#EEEEEE"){ $col = "white";}
	    else {                  $col = "#EEEEEE";}

	} else {
	    $ii = 0;
	    print $FO
		"\t    <!-- column 2 -->\n".
		"\t    <TD BGCOLOR=\"$col\"><font size=\"-2\">$disk</font></font> </TD>\n". # 1
		"\t    <TD BGCOLOR=\"$col\"><font size=\"-2\">$date</font></font> </TD>\n". # 2
		"\t    <TD BGCOLOR=\"$col\"><font size=\"-2\">$to</font></font>   </TD>\n". # 3
		"\t    <TD BGCOLOR=\"$col\"><font size=\"-2\">$mode</font></font> </TD>\n". # 4
		"\t    <TD BGCOLOR=\"$col\"><font size=\"-2\">$node</font></font> </TD>\n". # 5
		"\t    <TD><font size=\"-2\">&nbsp;</font></TD>\n".                         # 6
		"\t</TR>\n";
	}
    }
    if ($ii == 1){
	print $FO
	    "\t    <TD BGCOLOR=\"$col\"><font size=\"-2\">&nbsp;</font></font></TD>\n".     # 1
	    "\t    <TD BGCOLOR=\"$col\"><font size=\"-2\">&nbsp;</font></font></TD>\n".     # 2
	    "\t    <TD BGCOLOR=\"$col\"><font size=\"-2\">&nbsp;</font></font></TD>\n".     # 3
	    "\t    <TD BGCOLOR=\"$col\"><font size=\"-2\">&nbsp;</font></font></TD>\n".     # 4
	    "\t    <TD BGCOLOR=\"$col\"><font size=\"-2\">&nbsp;</font></font></TD>\n".     # 5
	    "\t    <TD><font size=\"-2\">&nbsp;</font></TD>\n".                             # 6
	    "\t</TR>\n";
    }
    print $FO
	"</TABLE>\n";


}




#
# Production Location summary
#
print $FO
    "<P>",
    "<H2><A NAME=\"PLOC\">Production Location</A> $UPGO</H2>\n",
    "<TABLE align=\"center\" cellspacing=\"0\" border=\"0\" width=\"750\">\n",
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
		 "\t<TD BGCOLOR=\"#EEEEEE\"><FONT SIZE=\"-1\">");
	    $TR{$tmp.$trg} = 1;
	    $LB{$tmp}      = 1;
	}
	if ( ! defined($UD{$tmp.$trg.$disk}) ){
	    push(@LINES,"<A HREF=\"#".&GetRef($disk)."\">$disk</A> ");
	    $UD{$tmp.$trg.$disk} = 1;
	}
    }
}
push(@LINES,"</FONT></TD>\n</TR>\n");
foreach $tmp (@LINES){  print $FO $tmp;}


print $FO
    "</TABLE>\n",
    "<P>",
    "<H2><A NAME=\"SUM\">Summary</A> $UPGO</H2>\n",
    "<BLOCKQUOTE>\n",
    "Total Space = ".sprintf("%.2f",$totals[0]/1024/1024)." GB<br>\n",
    "Total Used =  ".sprintf("%.2f",$totals[1]/1024/1024)." GB<br>\n",
    "Available  =  ".sprintf("%.2f",$totals[2]/1024/1024)." GB<br>\n",
    "</BLOCKQUOTE>\n",
    "</BODY>\n",
    "</HTML>\n";

if ( defined($ARGV[0]) ){
    if ( -e "$ARGV[0]-tmp"){
	if ( -e $ARGV[0]){
	    # delete if exists the preceedingly renamed file
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
}




sub GetRef
{
    my($el)=@_;

    $el =~ s/[\/ ]/_/g;
    $el;
}

sub GetFCRef
{
    my($What,$Icon,$el)=@_;
    my($x);

    $el =~ s/[\/ ]/_/g;

    # print "Searching for $OUTD/$What$el.txt\n";

    if ( -e $OUTD."/$What$el.txt"){
	if ( $What eq "FC"){
	    # For this report, only the unknown are important as
	    # we do not care of details, only if there are unknnown
	    # files
	    chomp($x = `/bin/grep Unknown $OUTD/$What$el.txt`);
	    return "" if ( $x eq "");
	    $x =~ m/(.*)(\d+\.\d+)(.*)/;
	    $x = "<BR><TT>$2$3</TT>";
	}

	return
	    "<IMG BORDER=\"0\" ALT=\"+\" SRC=\"$Icon\" WIDTH=\"25\" HEIGHT=\"25\">$x";
    }
    return "";

}
