#!/usr/bin/perl -w
use Time::Local;
use strict;
my $nowtime = timelocal(localtime) - 60*60;
print scalar(localtime($nowtime)),"\n";
my %cens = (
	    central=>[qw(productionCentral productionCentral1200 productionCentral600 centralTopo Central central)],
	    minbias=>[qw(ProductionMinBias MinBias MinBiasVertex MinBiasVtx)],
	    "22gev"=>[qw(minBias22GeVZDC)],
	   );
for my $cen (sort keys %cens) {
  print STDERR "$cen\n";
  unless (-d $cen) {
    mkdir "$cen",0755;
  }

  chdir "$cen";
    symlink "../StRoot","StRoot";
    symlink "../.i386_redhat61",".i386_redhat61";
    symlink "../.share",".share";
  for my $type (qw(ReversedFullField FullField HalfField ReversedHalfField FieldOff)) {
    print STDERR "$type\n";
    mkdir "$type",0755 unless (-d $type);
    chdir $type;
    print `pwd`,"\n";
    mkdir "runs",0755;
    symlink "../StRoot","StRoot";
    symlink "../.i386_redhat61",".i386_redhat61";
    symlink "../.share",".share";
    my @setdirs = ();
    for my $i (1..28) {
#      next if $i == 14;
      my $is = "";
      for my $trig (@{$cens{$cen}}) {
	for my $production (qw(P02gc)) { 
	  my $is = sprintf qq(/star/data%02d/reco/%s/%s/%s/2001),$i,$trig,$type,$production;
	  if (-d $is) {
	    print $is;
	    print ": Good\n";
	    push @setdirs,$is;
	  }
	  else {
#	    print ": Bad\n";
	    next;
	  }
	}
      }
    }
    my @dirs = ();
    for my $dir (@setdirs) {
      opendir DIR,"$dir" or next;
      my @files = grep {/\d+/} readdir DIR;
      for my $file (@files) {
	push @dirs,"${dir}/${file}";
      }
    }
    for my $dir (@dirs) {
      opendir DIR,$dir or die;
      my @files = grep {/.event.root/} readdir DIR;
      closedir DIR;
      for my $file (@files) {
	my @stats = stat "$dir/$file";
	next if ($stats[9]>$nowtime);
	next if ($file=~/tags.root/);
	symlink "${dir}/${file}","runs/${file}";
      }
    }
    chdir "../";
  }
  chdir "../";
}






