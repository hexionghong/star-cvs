#!/usr/bin/env perl
#
# Script to run rootcint : 
#              Input     : list of h-files
#              Output    : 
#
use Env;
use File::Basename;
#
my $Cint_cxx = shift;
my $Cint_h  = $Cint_cxx;
$Cint_h  =~ s/_Cint\.cxx/_Cint\.h/g;
my $DirName = dirname($Cint_cxx);		#print "DirName = $DirName\n";
my $LinkDef = $DirName . "/" . "LinkDef.h"; 	#print "Cint Files :", $Cint_cxx, ",", $Cint_h,",",$LinkDef,"\n";

my $LinkDefDirName;

my $sources  = shift; 				#print "sources =", $sources,"\n";
my $CPPFLAGS = shift; 				#print "CPPFLAGS = ", $CPPFLAGS, "\n";
my @cpps = split / /,$CPPFLAGS;			#print "cpps: @cpps \n";

my %class_hfile = (); 		# class h-file map
my %class_hfile_depens_on = (); # 
my %class_written = (); 
my @classes = 0; 		# list of classes
my $h_files = "";
my $coll = 0;
my $col  = 0;
# count no. of classes in LinkDef's
my $ListOfWrittenClasses = ":"; 
my $ListOfDefinedClasses = "";
my $off = 0;

open (Out, ">$LinkDef") or die "Can't open $LinkDef";

print Out "#pragma link off all globals;\n";  
print Out "#pragma link off all classes;\n";
print Out "#pragma link off all functions;\n";;
my $off = 1;
for my $def  (split /\s/,$sources) {		#print "SRC:", $def, "\n";
  if (!($def =~ /LinkDef/  ))		{next;}
  if ( ($def =~/^$LinkDef$/))   	{next;}
  
  open (In, $def) or die "Can't open $def";
  $LinkDefDirName = dirname($def);
  while ($line = <In>) {
    if (!($line  =~ /^\#pragma/))	{next;}
    if ($line =~ /link off all/) 	{next;}

##VP      print Out $line;# print $line; 
    if (!($line =~ / class / ))		{goto PRINT;}

    my @words = split /([ \(,\)\;\-\!+])/, $line;
    if ($words[10] != "class") 		{goto PRINT;}
    my $class = $words[12];
    if (!$class) 			{goto PRINT;}
    if ($class_written{$class})		{next;}
    push @classes, $class;
    $class_written{$class} = 1; 	#print "class: $class, written: $class_written{$class}\n";
PRINT: print Out $line;
  }
}
close (Out);


for my $h  (split /\s/,$sources) {	#print "SRC:", $h, "\n";
  next if !$h;
  next if $h =~ /LinkDef/;
  if ($h =~ /Stypes/)  {$h_files .= " " . basename($h); next;}
  #print "h = $h\n";
  my $hh = $h;
  if (!-f $hh) {($hh = $h) =~ s/\.share/StRoot/;}
  if (!-f $hh) {($hh = $h) =~ s/\.share/asps/;} 
  if (!-f $hh) {($hh = $h) =~ s/\.share//;} 
#  if (!-f $hh) {($hh = $h) =~ s/\.share/asps\/rexe/;} 
  #print "open $hh\n";
  open (In,$hh) or die "Can't open $hh";
  my $dummy;
  my $class;
  my $includes = "";
  my $com = 0;
  while ($line = <In>) {
    next if $line =~ /^\s*\/\//;
    if ($line =~ /\/\//) {$line =~ s/\/\/.*$//;}
    if ($com && $line =~ /\*\//) {$com = 0; $line =~ s/^*\*\///;}
    next if ($com); 
    if ($line =~ /\/\*/) {
      $com = 1; 
      if ($line =~ /\*\//) {
	$line =~ s/\/\*.*\*\///; 
	$com = 0;
      }
      else {$line =~ s/\/\*.*$//;}
    }
    if ($line =~ /\#include/ && $line !~ /(<>)/) {
      (my $inc_h = $line) =~ s/\#include\s//g; chop ($inc_h);
      $inc_h =~ s/\"//g; 		#print "inc_h = $inc_h\n";
      my $inc_hh = basename($inc_h);
      if ($sources =~ /$inc_hh/) {
	$includes .= ":" . $inc_hh; 	#print "--includes for $h: $includes\n";
      }
    }
    if ($line =~/ClassDef/) {
      if ($line =~ /\#\#/) {next;} # ClassDefs in macro definition
      my @words = split /([\(,\-\!\)])/, $line;
      my $class = $words[2];      	#print "=================class = ",$class,"\n";
      if ($class) {
	push @classes, $class;
	$class_hfile{$class} = $h; 	#print "class: $class, written: $class_written{$class}, h: $class_hfile{$class}\n"; 
	$class_hfile_depens_on{$class} = $includes;
      }
    }
    if ($line =~ /\#define/) {next;}
    if ($line =~ /StCollectionDef/) {
      $coll++;  #print "$coll, $line\n";
    }
  }
  close (In);
  if ($coll) {			# Collection Definition 
    my $macro = "./StRoot/St_base/StArray.h";
    if (-f $macro) {}
    else {$macro = `echo \$STAR/StRoot/St_base/StArray.h`;}
    my $tmp = "temp.h";
    open (INPUT, $h) or die "Can't open $h\n";
    my $new_h = $DirName . "/" . basename($h);
    open (OUTPUT, ">$tmp") or die "Can't open $tmp\n";
    while ($line = <INPUT>) {
      if ($line =~ /StCollectionDef/) {
	my @Class = split /([\(\)])/, $line;
	my $class = $Class[2];
	if ($class) {
	  (my $core = $class) =~ s/^St//g; 		#print "core $core\n";
          my $cl = "";
	  foreach my $stem ("Iterator","PtrVec","SPtrVec") {
	    if ($stem eq "Iterator") {$cl = "St" . $core . $stem      ;}
	    else                     {$cl = "St" . $stem . $core . "-";}
	    push @classes, $cl; 
	    $class_hfile{$cl} = $new_h; $class_hfile_depens_on{$cl} = $includes;
	    				#print "class: $stem $core $cl includes  $includes\n";
	  }
	  open(DEF, $macro) || die "Can't open Macros $macro \n";
	  my $def = 0;
	  while ($line = <DEF>) {
	    if ($line =~ /\#define/ && $line =~ /StCollectionDef/) {
	      $def = 1; next;
	    }
	    if ($def && $line =~ /\#define/) {last;}
	    if (! $def) { next; }
	    $line =~ s/\\//g;
	    $line =~ s/QWERTY/$class/g;
	    $line =~ s/ \#\# //g;
	    $line =~ s/\#\# //g;
	    $line =~ s/ \#\#//g;
	    print OUTPUT $line;				#print $line;
	  }
	}
      }
      else {print OUTPUT $line;}
    }
    close (OUTPUT);
    my $flag = rename $tmp, $new_h;  #print "rename $tmp $new_h;\n";
  }#end Collection Definition 
}
my $opened = "";
for my $class (@classes) {
#  if ($class =~ /Iterator/ || $class =~ /PtrVec/ ||
#      $class =~ /SPtrVec/) {$class_written{$class} = "YES";}
  if ($class) {
    if (!$class_written{$class}) {
      if (!$opened) {
	open (Out,">>$LinkDef")  or die "Can't open $LinkDef";
	print Out "#ifdef __CINT__\n";                  #print  "#ifdef __CINT__\n";
	if (! $off) {
	  print Out "#pragma link off all globals;\n";    #print  "#pragma link off all globals;\n";
	  print Out "#pragma link off all classes;\n";    #print  "#pragma link off all classes;\n";
	  print Out "#pragma link off all functions;\n";  #print  "#pragma link off all functions;\n";
	}
	$opened = "YES";
      }
      if ($class_hfile{$class} =~ /_Module/) {
	print Out "#pragma link C++ class $class-;\n"; print  "#pragma link C++ class $class-;\n";
	(my $global = $class) =~ s/St_//g;
	print Out "#pragma link C++ global $global;\n"; print  "#pragma link C++ global $global;\n";
      }
      else {
	if ($class =~ /^St/ and $class =~ /Iterator$/) {
	  print Out "#pragma link C++ typedef $class;\n"; print  "#pragma link C++ typedef $class;\n"; 
	}
	else {
	  if ($class =~ /-$/) {print Out "#pragma link C++ class $class;\n"; print  "#pragma link C++ class $class;\n";}
	  else {print Out "#pragma link C++ class $class+;\n"; print  "#pragma link C++ class $class+;\n";}
	}
      }
      $class_written{$class} = "YES";
    }
  }
}
if ($opened) {
  print Out "#endif\n";                           	#print  "#endif\n";
  close (Out);
  $opened = "";
}
if ($coll) { # order h-files with Collections 
  my $done = 0;
  while (!$done) {
    my $add = 0;
    for my $class (@classes) {				#print "list : $h_files\n";
      next if ! $class;
      my $h = $class_hfile{$class};
      next if ! $h;
      my $hh = " " . basename($h) . " "; 
      next if $h_files =~ /$hh/; 
      my $hd =  $class_hfile_depens_on{$class};
      if ($hd) { my @hds = split /:/, $hd; 	       	#print "h: $h => $hd => @hds\n";
		 my $incsd = "";
		 foreach my $hdd (@hds) {
		   next if ! $hdd or $hdd eq " ";	#print "hdd = $hdd h_files = $h_files\n";
		   next if $h_files =~ /$hdd/;
		   $incsd .= ":" . $hdd;		#print "parse $h: $hdd $incsd\n";
		 }
		 $hd = $incsd; 				#print "$h : depends on $hd\n";
		 $class_hfile_depens_on{$class} = $hd;	#print "h: $h => $hd\n";
	       }
      							#print "class $class h = $h depends on $hd\n";# if $hd;
      next if $hd;
      $h_files .= $hh; $add++;			#print "add $hh : $h_files\n";
    }
    if (!$add) {$done = 1;}
  }
}
for my $class (@classes) {	#loop over classes
  next if ! $class;
  my $h = $class_hfile{$class};  			#print "Class: $class h: $h written: $class_written{$class} \n";
  if (!$h) {	#No .h for class
     my $hfile = $DirName . "/" . $class . ".h"; 	#print "1 hfile = $hfile\n";
     if (! -f $hfile) {$hfile =$LinkDefDirName . "/" . $class . ".h";}	#print "2 hfile = $hfile\n";} 
     if (  -f $hfile) {$h = $hfile;} 
  }
  if (!$h) {
      print STDERR "RootCint.pl :: Warning : $class.h NOT FOUND\n"; 
      next;
  }

  my $hh = " " . basename($h) . " "; 				#print "hh = $hh\n";
  if ($h_files !~ /$hh/ )  {$h_files .= $hh;}
}#end loop over classes

							#print "h_files= $h_files\n";
my $hfile = $DirName . "/Stypes.h";
if (-f $hfile) {$h_files .= " Stypes.h";}
if ($h_files) {
  $h_files .= " " . "LinkDef.h"; 
  my $local_cint = basename($Cint_cxx);  		#print "files = $#files\n";
  $CPPFLAGS = " -I" . $DirName . " " . $CPPFLAGS;
  my $cmd  = "rootcint -f $Cint_cxx -c -DROOT_CINT -D__ROOT__ -I. $CPPFLAGS $h_files";
  print "cmd = ",$cmd,"\n";
  my $flag = `$cmd`; if ($?) {exit 2;}
  
}
exit(0);
# last line
