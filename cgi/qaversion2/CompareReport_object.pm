#! /usr/bin/perl

# pmj 4/6/00

#========================================================
package CompareReport_object;
#========================================================
use CGI qw/:standard :html3/;
use CGI::Carp qw(fatalsToBrowser);

use Storable;
use Data::Dumper;
use File::Find;

use QA_globals;
use QA_object;
use Server_object;
use Button_object;
use HiddenObject_object;
use IO_object;
use IO_utilities;

use DataClass_object;
use CompareReport_utilities;
use CompareScalars_object;
use Db_CompareReport_utilities;

use strict qw(vars subs); # no strict 'refs'
#--------------------------------------------------------
my $CompareScalars_object;
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

  #--------------------------------------------------------

  $self->ReportKey($report_key);

}
#========================================================
sub InitialDisplay{

  my $self = shift;
  #--------------------------------------------------------
  $self->PrintHeader();
  #---------------------------------------------------------
  my $script_name = $gCGIquery->script_name;
  my $hidden_string = $gBrowser_object->Hidden->Parameters;
  #---------------------------------------------------------
  my $report_key = $self->ReportKey();

  # lets the user choose which set of references to choose
  # e.g. default, user selected, both

  my $header = h3("Choose the reference set with which to compare ");
  my $radio_name    = 'Which reference set';
  my $radio_values  = ['Default','User Selected','Both'];
  my $radio_default = 'Default';

  my $radio_group = $gCGIquery->radio_group(
					    -name    =>$radio_name,
					    -values  =>$radio_values,
					    -default =>$radio_default
					    );
  
  my $button_ref = Button_object->new('DoCompareToReference', 
				   'Compare to Reference', $report_key);
  my $button_string = $button_ref->SubmitString;

  #my $button_ref = Button_object->new('SelectMultipleReports', 
  #                                    'Compare to Multiple Reports', $report_key);
  #my $button_string = $button_ref->SubmitString;

  print $gCGIquery->startform(-action=>"$script_name/lower_display", 
			      -TARGET=>"display") . 
	"<center>"     .
	$header        .
        $radio_group   . br .
	$button_string . 
	$hidden_string .
	$gCGIquery->endform() . 
	"</center>" . "\n";
}
#========================================================
sub CompareMultipleReports{

  my $self = shift;

  my $file = shift;
  my $macro_name = shift;
  my $mult_class = shift;

  #--------------------------------------------------------
  my $report_key = $self->ReportKey();
  #---------------------------------------------------------
  my $title;

  if (  $gDataClass_object->DataClass =~ /offline/ ){

    my $run_id = $QA_object_hash{$report_key}->LogReport->RunID;
    my $file_seq = $QA_object_hash{$report_key}->LogReport->FileSeq;

    $title = "Comparison of similar runs to Run ID $run_id, File Seq $file_seq";
  }
  else{
    my $production_dirname = $QA_object_hash{$report_key}->ProductionDirectory;
    $title = "Comparison of similar runs to $production_dirname ($report_key)";
  }
  #---------------------------------------------------------
  print "<h2> $title </h2> \n"; 
  print "<h3> Macro: $macro_name</h3>";
  #---------------------------------------------------------
  my ($low, $high) = $QA_object_hash{$report_key}->MultClassLimits($mult_class);
  my $string = "Multiplicity class: $mult_class; track node limits ($low, $high)";

  print "<h4>$string</h4>\n";

  #---------------------------------------------------------
  my @matched_keys_ordered = CompareReport_utilities::GetReferenceList($report_key);
  #---------------------------------------------------------
  my ($ref_file_table_rows, $ref_match_label_hash, $ascii_string_file_table) = 
    CompareReport_utilities::BuildFileTable($report_key, @matched_keys_ordered);
  #---------------------------------------------------------
  # print file table

  my @file_table_rows = @$ref_file_table_rows;
  print "<hr> \n";

  print "<h3> Comparison datasets </h3> \n";
  print table( {-border=>undef}, Tr(\@file_table_rows) );
  #---------------------------------------------------------
  # generate comparison object
  $CompareScalars_object = 
    new CompareScalars_object($report_key, \@matched_keys_ordered, $ref_match_label_hash, $macro_name, $mult_class);
  #---------------------------------------------------------
  $self->PrintTables();
  #---------------------------------------------------------
  $self->MakeAsciiReport($ascii_string_file_table);
}
#========================================================
# shows the reference report_keys and 
# the buttons for the macros/multiplicity classes.

sub CompareToReference{
  my $self = shift;

  $self->PrintHeader();      

  my $report_key = $self->ReportKey();

  # get the matching reference(s)
  my @refKeys = CompareReport_utilities::GetReferenceList($report_key); 

  # get out if there are no reference report keys
  if ( ! scalar @refKeys ){
    print h2("<font color=red>Sorry.",
	     "There is no reference matching this dataset<br>\n");
    return;
  }

  # make the QA_objects in case they dont exist
  QA_utilities::make_QA_objects(@refKeys);
    
  my $text;
  # target is a new window
  my $script_name = $gCGIquery->script_name;
  $text = $gCGIquery->startform(-action=>"$script_name/lower_display", 
				-TARGET=>"ScalarsAndTests"); 
  $text .= h3("Push the button for macro and multiplicity class");

  # multiplicity-macros buttons
  $text .= 
    $QA_object_hash{$self->ReportKey}->MultClassButtonString('DoCompareMultipleReports') .
      "\n";
  
  $text .= h3("Comparison datasets : ");
  
  # table of matched keys (reference reportkeys)
  # 1 means all the boxes are checked by default
  $text .= $self->CompareKeysTable(1,@refKeys);

  $text .= $gBrowser_object->Hidden->Parameters;
  $text .= $gCGIquery->endform ."\n";

  print $text;

} 

#========================================================
sub ReportKey{
  my $self = shift;
  @_ and $self->{_Report_Key} = shift;
  return $self->{_Report_Key};
}

#===================================================================
sub PrintHeader{

  my $self = shift;
  #--------------------------------------------------------
  my $report_key = $self->ReportKey();
  #--------------------------------------------------------
  my $title;

  if (  $gDataClass_object->DataClass =~ /offline/ ){

    my $run_id = $QA_object_hash{$report_key}->LogReport->RunID;
    my $file_seq = $QA_object_hash{$report_key}->LogReport->FileSeq;

    $title = "Comparison of similar runs to Run ID $run_id, File Seq $file_seq";
  }
  else{
    my $production_dirname = $QA_object_hash{$report_key}->ProductionDirectory;
    $title = "Comparison of similar runs to $production_dirname ($report_key)";
  }
  
  print "<h2> $title </h2> \n"; 
  #-----------------------------------------------------------------
  # pmj 8/9/00 documentation on scalars

  print Browser_utilities::ScalarDocumentationString(),"<hr>\n";

  #-----------------------------------------------------------------
} 
#===================================================================
sub PrintTables{

  my $self = shift; 
  #--------------------------------------------------------

  my $macro = $CompareScalars_object->MacroName();

  my ($ref_table_rows_difference, $ref_table_rows_absolute) = $CompareScalars_object->GetTableRows();
  
  print "<hr> \n";
  print "<h3> Macro: $macro </h3> \n";
  
  print "<h4> Differences relative to this run </h4> \n";
  print table( {-border=>undef}, Tr($ref_table_rows_difference) );
  
  print "<h4> Absolute values </h4> \n";
  print table( {-border=>undef}, Tr($ref_table_rows_absolute) );
} 
#===================================================================
sub MakeAsciiReport{

  my $self = shift;
  my $table_string = shift; 
  #---------------------------------------------------------
  my $report_key = $self->ReportKey(); 
  #---------------------------------------------------------
  my $io = new IO_object("CompareFilename", $report_key);
  my $dh = $io->Open(">", "0644");
  my $filename_ascii = $io->Name();

  print "<h4> (Ascii version of this page written to $filename_ascii) </h4> \n";
  #---------------------------------------------------------
  my $title;
  if (  $gDataClass_object->DataClass =~ /offline/ ){
    my $run_id = $QA_object_hash{$report_key}->LogReport->RunID;
    my $file_seq = $QA_object_hash{$report_key}->LogReport->FileSeq;
    $title = "Comparison of similar runs to Run ID $run_id, File Seq $file_seq\n";
  }
  else{
    my $production_dirname = $QA_object_hash{$report_key}->ProductionDirectory;
    $title = "Comparison of similar runs to \n$production_dirname ($report_key)\n";
  }

  print $dh "*" x 80, "\n";
  print $dh "$title";
  print $dh "(up to 10 most recent runs compared) \n"; 
  print $dh "*" x 80, "\n";

  print $dh " Comparison datasets \n \n";

  print $dh "$table_string \n";

  print $dh "*" x 80, "\n";
  #---------------------------------------------------------

  my $macro = $CompareScalars_object->MacroName();

  my ($string_difference, $string_absolute) = $CompareScalars_object->GetAsciiStrings();
  
  print $dh "Macro: $macro \n";
  
  print $dh "\nDifferences relative to this run\n$string_difference\n";
  print $dh "\nAbsolute values\n$string_absolute\n";
  
  print $dh "*" x 80, "\n";

  #---------------------------------------------------------
  undef $io;
}
#==========
sub CompareKeysTable{
  my $self       = shift;
  my $checked    = shift; # check boxes default checked or not 
  my @compareKeys = @_;

  # display matching runs

  my @table_heading = ('Dataset', 'Created/On disk?' );
  my @table_rows    =  th(\@table_heading);

  #--- current run

  my $pre_string      = "<strong> this run: </strong>";
  my $dataset_string  = $pre_string.$QA_object_hash{$self->ReportKey}->DataDisplayString();
  my $creation_string = $QA_object_hash{$self->ReportKey}->CreationString();
  
  push @table_rows, td( [$dataset_string, $creation_string ]); 

  #--- comparison runs

  foreach my $key (@compareKeys){
    
    # skip possible duplicate
    next if ($key eq $self->ReportKey);

    # pmj 7/9/00 take out check boxes - not very useful now

    #my $box_name        = $key.".compare_report";
    #my $button_string   = $gCGIquery->checkbox("$box_name", $checked, 'on', '');
    #my $dataset_string  = $button_string.$QA_object_hash{$key}->DataDisplayString();
    #my $creation_string = $QA_object_hash{$key}->CreationString();

    my $dataset_string  = $QA_object_hash{$key}->DataDisplayString();
    my $creation_string = $QA_object_hash{$key}->CreationString();

    #---

    push @table_rows, td( [$dataset_string, $creation_string ]); 

  }

  return table( {-border=>undef}, Tr(\@table_rows) ."\n" );
}
  
#========================================================
# obsolete pmj 11/9/00
sub ObsoleteSelectMultipleReports{
#sub SelectMultipleReports{

  my $self = shift;
  #--------------------------------------------------------
  $self->PrintHeader();
  #---------------------------------------------------------
  my $script_name = $gCGIquery->script_name;
  print $gCGIquery->startform(-action=>"$script_name/lower_display", -TARGET=>"ScalarsAndTests"); 
  #---------------------------------------------------------
  my $report_key = $self->ReportKey();

  print "<h3> Select comparison runs from following list, then push button for macro and multiplicity class</h3>";

  my $button_string = $QA_object_hash{$report_key}->MultClassButtonString('DoCompareMultipleReports');
  print "$button_string.<br> \n";

  #---------------------------------------------------------
  print "<br>(multiple file selections allowed; more than 6-8 do not display or print well) <br> \n";
  
  #---------------------------------------------------------
  # get time-ordered set of keys, dependent upon data class
  
  my $data_class = $gDataClass_object->DataClass();
  my $function = "Db_CompareReport_utilities::$data_class";
  
  no strict 'refs';
  my @matched_keys_ordered = &$function($report_key);
  
  use strict 'refs';
  
  # BUM 000625
  # it's possible that no key in the %QA_object_hash corresponds
  # to a matched_key.  this creates the QA_object if it doesnt already
  # exist in the %QA_object_hash.

  QA_utilities::make_QA_objects(@matched_keys_ordered);

  print "<h3> Comparison runs: </h3>";
  print $self->CompareKeysTable(0,@matched_keys_ordered);

  #-------------------------------------------------------------------
  my $hidden_string = $gBrowser_object->Hidden->Parameters;
  print "$hidden_string";
  #-------------------------------------------------------------------
  print $gCGIquery->endform;

}
