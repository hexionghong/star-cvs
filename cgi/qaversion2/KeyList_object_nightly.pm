#! /usr/bin/perl
#
# derived KeyList class for nightly 
#
#========================================================
package KeyList_object_nightly;
#========================================================
use CGI qw/:standard :html3/;

use QA_globals;
use QA_utilities;
use Db_KeyList_utilities;
use Browser_object;

use strict;
use base qw(KeyList_object);
#--------------------------------------------------------
1;
#========================================================
sub new{
  my $classname = shift;
  my $self      = $classname->SUPER::new(@_);

  #$classname eq __PACKAGE__ and 
  #  die __PACKAGE__, " is virtual";

  return $self;
}

#========================================================
# popup menu for selecting jobs

sub JobPopupMenu{
  my $self = shift;

  no strict 'refs';

  # using 'map' technique in the cgi forms - originated by ben norman

  # possible criteria to select on 
  # these are the cgi parameter names
  $self->{select_fields} = [ 'eventGen',
			     'LibLevel',
			     'platform',
			     'eventType',
			     'geometry',
			     'QAstatus',
			     'onDisk',
			     'jobStatus',
			     'createTime' 
			   ];
  # selection labels for the user
  $self->{select_labels} = { eventGen  => 'event gen',
			     LibLevel  => 'library',
			     platform  => 'platform',
			     eventType => 'event type',
			     geometry  => 'geometry',
			     onDisk    => 'on disk',
			     QAstatus  => 'QA status',
			     jobStatus => 'job status',
			     createTime=> 'job createTime'
			   };

  # possible values for each selection field
  # ref of hash of refs to arrays
  
  %{$self->{values}} = map {$_, ['any']} @{$self->{select_fields}};

  # possible labels for each selection field
  # ref of hash of refs to hashes
  # not all values need labels...
  $self->{labels} = undef; 


  # get some selection values from the database
  my $select_ref = $self->GetSelectionOptions();
  
  # fill some selection values...
  push @{$self->{values}{eventGen}}, @{$select_ref->{eventGen}};
  push @{$self->{values}{LibLevel}}, @{$select_ref->{LibLevel}};
  push @{$self->{values}{platform}}, @{$select_ref->{platform}};
  push @{$self->{values}{eventType}},@{$select_ref->{eventType}};
  push @{$self->{values}{geometry}}, @{$select_ref->{geometry}};

  # on disk
  push @{$self->{values}{onDisk}}, ('on disk', 'not on disk');
  
  # job status 
  push @{$self->{values}{jobStatus}}, ('done', 'not done');

  # QA status
  my @macro_names = @{$select_ref->{macroName}};

  $self->FillQAStatusMenu(@macro_names);
    
  # createTime
  push @{$self->{values}{createTime}}, ('one_day', 'three_days','seven_days', 'fourteen_days');
  
  $self->{labels}{createTime}{one_day} = 'within last 24 hours';
  $self->{labels}{createTime}{three_days} = 'within last 3 days';
  $self->{labels}{createTime}{seven_days} = 'within last 7 days';
  $self->{labels}{createTime}{fourteen_days} = 'within last 14 days';
  
  # set defaults
  # unless specified otherwise, default will be 'any'
  $self->{defaults}{eventGen}   = 'hadronic_cocktail';
  $self->{defaults}{LibLevel}   = 'dev';
  $self->{defaults}{onDisk}     = 'on disk';
  $self->{defaults}{QAstatus}   = 'done';
  $self->{defaults}{createTime} = 'seven_days';

  my $submit_string = br.$gCGIquery->submit('Display datasets');

  my @table_rows;
  #--- 
  # pmj 21/6/00: more compact display, no header. Pulldown menu for changing classes
  # has been moved to banner so there is more horizontal space

  my @rows = 
    (
     td([$self->GetRowOfMenus(
			      'eventGen'
			      ,'LibLevel'
			      ,'platform'
			      ,'jobStatus'
			      ,'QAstatus'
			     )
	]),
     td([$self->GetRowOfMenus(
			      'eventType'
			      ,'geometry'
			      ,'onDisk'
			      ,'createTime'
			     )
	                      ,$submit_string
	])
    );
  #---

  my $script_name = $gCGIquery->script_name;
  my $hidden_string = $gBrowser_object->Hidden->Parameters;

  my $table_string =
    table({-align=>'left'}, Tr({-valign=>'top'}, [@rows]));

  my $select_data_string = 
      $gCGIquery->startform(-action=>"$script_name/upper_display",
			    -TARGET=>"list").
	$table_string.
	  $hidden_string.
	    $gCGIquery->endform;

  return $select_data_string;

}
#========================================================
# get the selected parameter values chose by the user
# returns an array of cgi values according to the popup menu

sub SelectedParameters{
  my $self = shift;

  return map { $gCGIquery->param($_) } @{$self->{select_fields}};


}
 
1;
