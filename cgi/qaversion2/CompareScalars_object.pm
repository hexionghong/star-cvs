#! /usr/bin/perl

# pmj 4/6/00

#========================================================
package CompareScalars_object;
#========================================================
use CGI qw/:standard :html3/;
use CGI::Carp qw(fatalsToBrowser);

use Storable;
use Data::Dumper;
use File::Basename;

use QA_globals;
use QA_object;
use Server_object;
use Button_object;
use HiddenObject_object;
use IO_object;
use IO_utilities;

use Text::Tabs;

use DataClass_object;
use CompareReport_utilities;

use strict 'vars';
#--------------------------------------------------------
1;
#========================================================
sub new{
  my $classname = shift;
  my $self = {};
  bless ($self, $classname);

  # initialize
  $self->_init(@_);

  return $self;
}
#========================================================
sub _init{

  my $self  = shift;
  my $report_key = shift;
  my $ref_matched_keys_ordered = shift;
  my $ref_match_key_label_hash = shift;

  # pmj 27/8/00
  my $macro_name = shift;
  my $mult_class = shift;

  #--------------------------------------------------------
  $self->ReportKey($report_key);
  $self->MacroName($macro_name);
  $self->MultiplicityClass($mult_class);
  #--------------------------------------------------------
  my @matched_keys_ordered = @$ref_matched_keys_ordered;
  $self->MatchedKeysOrdered(@matched_keys_ordered);

  $self->MatchedKeyLabelHashRef($ref_match_key_label_hash);
  #--------------------------------------------------------

  my $this_hash_ref = $self->GetRunScalarHash();
  my %this_hash = %$this_hash_ref;
  
  foreach my $match_key ( @matched_keys_ordered ){
    
    my $match_hash_ref = $self->GetRunScalarHash($match_key);
    my %match_hash = %$match_hash_ref;
    
    foreach my $scalar ( keys %this_hash ){
      my $value = $match_hash{$scalar};
      $self->MatchScalarValues($match_key, $scalar, $value);
    }
  }
}
#=======================================================================
sub GetTableRows{

  # returns table rows (differential and absolute) for one macro

  my $self = shift;

  #---------------------------------------------------------------------
  my $macro = $self->MacroName();
  my $report_key = $self->ReportKey();
  my @matched_keys_ordered = $self->MatchedKeysOrdered();

  my $ref = $self->MatchedKeyLabelHashRef();
  my %match_key_label = %$ref;
  #---------------------------------------------------------------------

  my @table_keys = ();
  my @table_label = ();

  # only print tables for keys with at least one defined value?
  foreach my $match_key (@matched_keys_ordered){

# take out test for testing
#    $self->CountDefinedScalars($macro, $match_key) and do{
	push @table_keys, $match_key;
	push @table_label, $match_key_label{$match_key};
 #     };
  }
  #---------------------------------------------------------------------
  my @table_heading = ('Scalar', 'This run', @table_label );
  
  my @table_rows_absolute =  th(\@table_heading);
  my @table_rows_difference =  th(\@table_heading);
  #---------------------------------------------------------------------
  tie my %this_hash, "Tie::IxHash"; 

  my $this_hash_ref = $self->GetRunScalarHash($report_key, $macro); 
  %this_hash = %$this_hash_ref;

  foreach my $scalar ( keys %this_hash ){

    my @row_data_absolute = $scalar;
    my @row_data_difference = $scalar;
      
    my $this_scalar_value = $this_hash{$scalar};
    push @row_data_absolute, $this_scalar_value;
    push @row_data_difference, $this_scalar_value;

    my $difference;

    foreach my $match_key ( @table_keys ){

      my $compare_scalar_value = $self->MatchScalarValues($macro, $match_key, $scalar);

      if ($compare_scalar_value =~ /\d+/ ){
	if ( $this_scalar_value =~ /\d+/ ){
	  $difference = $this_scalar_value - $compare_scalar_value;
	  $difference = (int ( 100 * $difference) ) / 100;
	}
	else{
	  $difference = 'undef';
	}
      }
      else{
	$difference = 'undef';
	$compare_scalar_value = 'undef';
      }
	
      push @row_data_absolute, $compare_scalar_value; 
      push @row_data_difference, $difference;
    }
      
    push @table_rows_absolute, td( \@row_data_absolute );
    push @table_rows_difference, td( \@row_data_difference );
      
  }

  #------------------------------------------------------------------------

  return ( \@table_rows_difference, \@table_rows_absolute);

}
#=======================================================================
sub GetAsciiStrings{

  # returns table rows in ascii strings (differential and absolute) for one macro

  my $self = shift;

  #---------------------------------------------------------------------
  my $macro = $self->MacroName();
  my $report_key = $self->ReportKey();
  my @matched_keys_ordered = $self->MatchedKeysOrdered();

  my $ref = $self->MatchedKeyLabelHashRef();
  my %match_key_label = %$ref;
  #---------------------------------------------------------------------

  my @table_keys = ();
  my @table_label = ();
  foreach my $match_key (@matched_keys_ordered){

#    $self->CountDefinedScalars($macro, $match_key) and do{
	push @table_keys, $match_key;
	push @table_label, $match_key_label{$match_key};
#      };
  }
  #---------------------------------------------------------------------
  $tabstop = 15;
  #---------------------------------------------------------------------
  my $header_string = join "\t", "Scalar", "\tThis run", @table_label;

  my $string_absolute = $header_string."\n";
  my $string_difference = $header_string."\n";
  #---------------------------------------------------------
  tie my %this_hash, "Tie::IxHash"; 

  my $this_hash_ref = $self->GetRunScalarHash($report_key, $macro); 
  %this_hash = %$this_hash_ref;

  foreach my $scalar ( keys %this_hash ){

    my @row_data_absolute = $scalar;
    my @row_data_difference = $scalar;

    my $tabstring = "\t";
    length $scalar < $tabstop and  $tabstring .= "\t";
      
    my $this_scalar_value = $tabstring.$this_hash{$scalar};
    push @row_data_absolute, $this_scalar_value;
    push @row_data_difference, $this_scalar_value;

    my $difference;

    foreach my $match_key ( @table_keys ){

      my $compare_scalar_value = $self->MatchScalarValues($macro, $match_key, $scalar);

      if ($compare_scalar_value =~ /\d+/ ){
	if ( $this_scalar_value =~ /\d+/ ){
	  $difference = $this_scalar_value - $compare_scalar_value;
	  $difference = (int ( 100 * $difference) ) / 100;
	}
	else{
	  $difference = 'undef';
	}
      }
      else{
	$difference = 'undef';
	$compare_scalar_value = 'undef';
      }

      push @row_data_absolute, "\t".$compare_scalar_value; 
      push @row_data_difference, "\t".$difference;
    }
      
    my $row_string_abs = join '',@row_data_absolute;
    my $row_string_dif = join '',@row_data_difference;

    $string_absolute .= "$row_string_abs\n";
    $string_difference .= "$row_string_dif\n";
      
  }

  #------------------------------------------------------------------------
  # expand tabs into spaces

  $string_absolute = expand($string_absolute);
  $string_difference = expand($string_difference);

  #------------------------------------------------------------------------

  return ( $string_difference, $string_absolute);
  
}
#========================================================
sub ReportKey{
  my $self = shift;
  @_ and $self->{_ReportKey} = shift;
  return $self->{_ReportKey};
}
#========================================================
sub MacroName{
  my $self = shift;
  @_ and $self->{_MacroName} = shift;
  return $self->{_MacroName};
}
#========================================================
sub MultiplicityClass{
  my $self = shift;
  @_ and $self->{_MultiplicityClass} = shift;
  return $self->{_MultiplicityClass};
}
#========================================================
sub MatchedKeyLabelHashRef{
  my $self = shift;
  @_ and $self->{_MatchedKeyLabelHashRef} = shift;
  return $self->{_MatchedKeyLabelHashRef};
}
#========================================================
sub MatchedKeysOrdered{
  my $self = shift;

  @_ and do{
    my @array = @_;
    $self->{_MatchedKeysOrdered} = \@array;
  };
  
  my $ref = $self->{_MatchedKeysOrdered};
  my @array = @$ref;
  return @array;
}
#========================================================
sub MatchScalarValues{
  my $self = shift;

  my $match_key = shift;
  my $scalar = shift;

  @_ and do{
    my $value = shift;

    $self->{_MatchScalarValues}->{$match_key}->{$scalar}->{_value} = $value;
    $value ne "undef" and $self->{_MatchScalarValues}->{$match_key}->{_count}++;
  };
  
  return $self->{_MatchScalarValues}->{$match_key}->{$scalar}->{_value};
}
#=====================================================================
sub CountDefinedScalars{
  my $self = shift;
  my $match_key = shift;

  return $self->{_MatchScalarValues}->{$match_key}->{_count}++;
}  
#========================================================
sub GetRunScalarHash{

  my $self = shift;

  # if there is an argument this is a foreign key, otherwise return hash for ReportKey
  my $report_key;
  if(@_){
    $report_key = shift;
  }
  else{
    $report_key = $self->ReportKey();
  }
  #----------------------------------------------------------------------
  my $macro_name = $self->MacroName();
  my $mult_class = $self->MultiplicityClass();
  #----------------------------------------------------------------------
  tie my %run_scalars, "Tie::IxHash"; 
  %run_scalars = ();
  #-----------------------------------------------------------------------
  my $io = new IO_object('EvaluationFilename', $report_key, $macro_name);
  my $filename = $io->Name();
  undef $io;

  -s $filename and do{
    my $ref = retrieve($filename) or print "Cannot retrieve file $filename:$! \n";
    %run_scalars = %{$$ref->RunScalarsHash($mult_class)};
  };
  #---------------------- -------------------------------------------------
  # flag values which are not numerical
  foreach my $key ( keys %run_scalars){
    $run_scalars{$key} =~ /\d+/ or $run_scalars{$key} = "undef";
  }
  #-----------------------------------------------------------------------
  return \%run_scalars;  
}

