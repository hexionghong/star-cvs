#! /opt/star/bin/perl

# general utilities used by various scripts

# pmj 1/7/99
#=========================================================
package QA_cgi_utilities;
#=========================================================

use CGI qw(:standard escapeHTML);
#use CGI::Carp qw(fatalsToBrowser);

use Cwd;

use File::stat;
use File::Copy;
use File::Find;
use File::Basename;

use Time::Local;

use QA_globals;
#=========================================================
1.;
#===================================================================

sub make_anchor{

  my $message = shift (@_);
  my $filename = shift (@_);
  my $link = shift (@_);

  print "$message: <a href=$link>$filename</a> <br> \n";

}
#==========================================================
sub get_file{
  
  $filename = $File::Find::name;

  my @string = split  /\./, $filename;
  my $filetype = ".".$string[-2].".".$string[-1];

  $global_input_data_type eq $filetype and $global_filename = $filename;

}
#==========================================================
sub get_logfile{
  
  $filename = $File::Find::name;
    
  if ( $filename =~ /\.log$/ ) {
    $global_logfile = $filename;
  }
}
#==========================================================
sub get_root_dst_file{
  
  $filename = $File::Find::name;
    
  if ( $filename =~ /\.dst\.root/ ) {
    $global_root_dst_file = $filename;
  }
}
#==========================================================
sub get_root_hist_file{
  
  $filename = $File::Find::name;
    
  if ( $filename =~ /\.hist\.root/ ) {
    $global_root_hist_file = $filename;
  }
  
}
#==========================================================
sub get_root_event_file{
  
  $filename = $File::Find::name;
    
  if ( $filename =~ /\.event\.root/ ) {
    $global_root_event_file = $filename;
  }
  
}
#==========================================================
sub get_root_tags_file{
  
  $filename = $File::Find::name;
    
  if ( $filename =~ /\.tags\.root/ ) {
    $global_root_tags_file = $filename;
  }
  
}
#==========================================================
sub get_root_runco_file{
  
  $filename = $File::Find::name;
    
  if ( $filename =~ /\.runco\.root/ ) {
    $global_root_runco_file = $filename;
  }
  
}
#==========================================================
sub get_root_geant_file{
  $filename = $File::Find::name;

  if ( $filename =~ /\.geant\.root/ ) {
    $global_root_geant_file = $filename;
  }
}
#==========================================================
sub get_xdf_file{
  
  $filename = $File::Find::name;
    
  if ( $filename =~ /\.dst\.xdf/ ) {
    $global_dst_xdf_file = $filename;
  }
  
}
#==========================================================
sub convert_logtime_to_epoch_sec{
  
  # converts time string from format in log file to one usable by localtime

  $in_string = shift;

  # make sure there is something in the string;
  $in_string or return 0;

  # time given in log file as YYYYMMDD/(H)HMMSS, where H can be one or two digits (!!??)
  
  $year = substr($in_string, 0, 4) - 1900;
  $month = substr($in_string, 4, 2) - 1;
  $mday = substr($in_string, 6, 2);
  $hour = substr($in_string, -6, 2);
  $min = substr($in_string, -4, 2);
  $sec = substr($in_string, -2, 2);
  
  $hour =~ s/\///;
  
  return timelocal($sec, $min, $hour, $mday, $month, $year);
  
}
#==========================================================
sub yymmddhhmmss{
  
  ($sec, $min, $hour, $mday, $mon, $year) = localtime;

  $sec < 10 and $sec = "0".$sec;

  $min < 10 and $min = "0".$min;

  $hour < 10 and $hour = "0".$hour;

  $mday < 10 and $mday = "0".$mday;

  $mon += 1;
  $mon < 10 and $mon = "0".$mon;

  $year > 99 and $year -= 100;
  $year < 10 and $year = "0".$year;

  return $year.$mon.$mday.$hour.$min.$sec;
}
