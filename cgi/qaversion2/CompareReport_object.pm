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
sub SelectMultipleReports{

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
  my @matched_keys_ordered = &CompareReport_utilities::GetComparisonKeys;

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

  my @refKeys = $self->GetReferenceList();  # get the matching reference(s)

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
  print "<hr> \n";
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

  my @table_heading = ('Dataset (check to compare)', 'Created/On disk?' );
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

    my $box_name        = $key.".compare_report";
    my $button_string   = $gCGIquery->checkbox("$box_name", $checked, 'on', '');
    my $dataset_string  = $button_string.$QA_object_hash{$key}->DataDisplayString();
    my $creation_string = $QA_object_hash{$key}->CreationString();

    push @table_rows, td( [$dataset_string, $creation_string ]); 

  }

  return table( {-border=>undef}, Tr(\@table_rows) ."\n" );
}
  
#==========
# sets the reference report key(s).
# returns a perl ref to the reference report key(s)

sub GetReferenceList{
  my $self = shift;

  my $refSet = $gCGIquery->param('Which reference set');
  my (@userList, @defaultList, @refKeys);
  my $report_key = $self->ReportKey;

  # first get the appropriate reference list according to 
  # the reference set selected by the user.
  # see InitialDisplay
  if ($refSet eq 'User Selected' || $refSet eq 'Both'){
    @userList = $gCGIquery->param('user_reference_list');
  }
  if ($refSet eq 'Default' || $refSet eq 'Both'){
    
    my $dataClass = $gDataClass_object->DataClass;
    my $sub       = "Db_CompareReport_utilities::$dataClass";

    # arg=1 means we only want the report keys tagged as a reference dataset
    @defaultList   = &$sub($report_key,1);
  }

  # return it
  if ($refSet eq 'User Selected'){
    
    # if there's only one elt in the userList
    # and this in fact matches the report key, get out.
    if (scalar @userList == 1 && $userList[0] eq $self->ReportKey){
      return;
    }

    print h3("Using user defined reference list");
    return @userList;
  }
  elsif($refSet eq 'Default'){
    
    # check that if this report key is flagged as a reference.
    # if so, the user must specify his/her own references
    # for the comparisons to make sense.
    
    my $isReference = 
      Db_CompareReport_utilities::IsReference($self->ReportKey);
    
    if ($isReference and !scalar @defaultList){
      print h3("Note: this dataset has been flagged as a ",
	       "reference dataset.\n",
	       "To do comparisons, please go back and choose your own ",
	       "references.<br>\n");
      return;
    }
    print h3("Using the default reference list");
    return @defaultList;
  }
  elsif($refSet eq 'Both'){
    print h3("Using both the default and user defined reference list");
    return (@defaultList, @userList);
  }
  
}
