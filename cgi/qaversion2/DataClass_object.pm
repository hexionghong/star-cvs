#! /usr/bin/perl

# class to encapsulate all directory assignments for a specific data class

# these should not be used directly for I/O, but should be referenced by IO_object

# pmj 2/6/00

#========================================================
package DataClass_object;
#========================================================

use strict;

use CGI qw/:standard :html3/;

use CGI::Carp qw(fatalsToBrowser);

use File::Basename;
use File::Find;
use File::stat;
use Cwd;

use Storable;
use Data::Dumper;
use QA_globals;
#--------------------------------------------------------

use vars qw( $AUTOLOAD 

	     %ok_field 

	     @data_class_array_offline
	     @data_class_array_online
	     @data_class_array

	     %data_class_labels_offline
	     %data_class_labels_online
	     %data_class_labels

	     %ok_data_class_offline 
	     %ok_data_class_online
	     %ok_data_class
	   );
#-------------------------------------------------------
# define registered data classes for offline and online

my @data_class_array_offline = qw ( offline_real
				    offline_MC
				    nightly_real
				    nightly_MC
				    debug
				  );

my %data_class_labels_offline = (offline_real  => 'Real Data Production',
				 offline_MC    => 'MC Data Production',
				 nightly_real  => 'Real Data Nightly Tests',
				 nightly_MC    => 'MC Data Nightly Tests',
				 debug         => 'debug');

#----

my @data_class_array_online = qw ( online_raw
				   online_reco
				   online_debug
				  );

my %data_class_labels_online = (online_raw  => 'Raw Data',
				online_reco => 'Reco Data',
				online_debug => 'debug');

#----
			   
my $data_class;

for $data_class ( @data_class_array_offline ) { 
  $ok_data_class_offline{$data_class}++; 
}

for $data_class ( @data_class_array_online ) { 
  $ok_data_class_online{$data_class}++; 
}

#------------------------------------------------------------
# register allowed accessor fields for AUTOLOADER
# (see PERL Cookbook 13.11, pg 468)

for my $attr ( qw (
		   TopDir
		   TopDirWWW
		   TopDirReport
		   BatchDir
		   ControlDir
		   UpdateDir
		   BackupDir
		   CronDir
		   Scratch
		   CompareDir
		   TopDirReportOld
		   MessageDir
		   TopDirReportWWW
		   ControlDirWWW
		   BatchDirWWW
		   LogScratchDirWWW
		   LogScratchDir
		   LogfileWWW
		   KeyList_obj
		   QA_obj
		   GetOldReports
		   DataPool
		   EventPool
		   SummaryHistDir
		   DataType
		   MySQLHost
		   dbFile
		   FileCatalog
		   JobStatus
		   ProdOptions
		   JobRelations
		   dbQA
		   UpdateRoutine
		   ToDoKeys
		   BrowserBannerColor
		   BrowserBannerTextColor
		   BrowserBannerLabel
		   DataClassArray
		   DataClassLabels
		  )
	    ) { $ok_field {$attr}++; }
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

  my $self = shift;

  my $data_class = shift;
  @_ and my @args = @_;
  #--------------------------------------------------------
  my $server_type = $gServer_object->ServerType();
  
  if ($server_type eq 'online' ){
    @data_class_array = @data_class_array_online;
    %data_class_labels = %data_class_labels_online;
    %ok_data_class = %ok_data_class_online;

    # master top directory
    $self->Home("/home/users/starqa/qa");

    # mysql host
    $self->MySQLHost("onllinux1.star.bnl.gov");

    # online file system
    $self->DataPool("/online/datapool/QA");
    $self->EventPool("/online/evtpool/QAfifo");
    

  }
  elsif ($server_type eq 'offline' ){
    @data_class_array = @data_class_array_offline;
    %data_class_labels = %data_class_labels_offline;
    %ok_data_class = %ok_data_class_offline;

    # BEN(8jun2000):  changed for rcas
    $self->Home("/afs/rhic/star/starqa/qa01");

    $self->MySQLHost("duvall.star.bnl.gov");
  }
  else{
    die "DataClass_object::_init: unknown server type $server_type<br>\n";
  }

  #--------------------------------------------------------
  $ok_data_class{$data_class} or die "DataClass_object::init_: unregistered data class $data_class \n";
  #--------------------------------------------------------

  $self->DataClass($data_class);

  $self->DataClassArray(\@data_class_array);
  $self->DataClassLabels(\%data_class_labels);

  #--------------------------------------------------------
  # call data class-specific routine with rest of arguments
  $self->$data_class(@args);

}
#========================================================
sub Home{

  my $self = shift;
  @_ and $self->{_Home} = shift;
  return $self->{_Home};
}
#========================================================
sub DataClass{

  my $self = shift;
  @_ and $self->{_DataClass} = shift;
  return $self->{_DataClass};
}
#========================================================
sub AUTOLOAD{
  
  # see PERL Cookbook recipe 13.11

  my $self = shift;
  my $attr = $AUTOLOAD;

  #--------------------------------------------------------
  # strip package qualifiers
  $attr =~ s/.*:://;

  # test if valid call

  $attr =~ /[^A-Z]/ or croak "DataClass_object::AUTOLOAD: invalide accessor $attr \n";
  $ok_field{$attr} or croak "DataClass_object::AUTOLOAD: unregistered accessor $attr \n";

  #--------------------------------------------------------
  # do accessor
  no strict;

  *{$attr} = sub {
    my $self = shift;;
    $self->{uc $attr} = shift if @_;
    return $self->{uc $attr};
  };
  

  @_ and $self->{uc $attr} = shift;
  return $self->{uc $attr}
}
#********************************************************
# Data class-specific routines follow
#********************************************************
#========================================================
sub StandardDirectories{

  my $self = shift;

# called after TopDir defined below

  my $topdir = $self->TopDir();
  $self->TopDirReport("$topdir/reports");
  $self->ControlDir("$topdir/control_and_test");
  $self->UpdateDir("$topdir/update");
  $self->BackupDir("$topdir/backups");
  $self->Scratch("$topdir/scratch");
  $self->CompareDir("$topdir/compare_runs");
  $self->TopDirReportOld("$topdir/reports_old");
  $self->MessageDir("$topdir/messages");

  my $topdir_WWW = $self->TopDirWWW();
  my $data_class = $self->DataClass();

  $self->TopDirReportWWW("$topdir_WWW/report_dir_$data_class");
  $self->ControlDirWWW("$topdir_WWW/control_dir_$data_class");
  $self->BatchDirWWW("$topdir_WWW/batch_dir_$data_class");

  # need to make a temporary link to the logfiles
  # obsolete - jul 30 2000
  $self->LogScratchDirWWW("$topdir_WWW/scratch");
  $self->LogScratchDir("/star/u2e/starqa/WWW/qa/scratch");

  # logfile_WWW is a soft link to '/' ...
  $self->LogfileWWW("$topdir_WWW/logfile_WWW");

  my $batch_dir = "$topdir/batch";
  $self->BatchDir("$batch_dir");
  $self->CronDir("$batch_dir/cronjob_logs");

}
#========================================================
sub offline_real{

  my $self = shift;
  @_ and my @args = @_;
  #------------------------------------------------------

  my $home = $self->Home();

  $self->TopDir("$home/offline_real");
  $self->TopDirWWW("http://connery.star.bnl.gov/~starqa/qa");

  $self->StandardDirectories();
  
  # objects to create
  $self->KeyList_obj("KeyList_object_offline_real");
  $self->QA_obj("QA_object_offline_real");

  # database stuff
  $self->dbFile("operation");
  $self->FileCatalog("FileCatalog");
  $self->JobStatus("JobStatus");
  $self->ProdOptions("ProdOptions");
  $self->JobRelations("jobRelations");
  $self->dbQA("prod_QA");
  
  # for updating from DB
  $self->UpdateRoutine("Db_update_utilities::UpdateQAOfflineReal");

  # get old reports
  $self->GetOldReports("QA_db_utilities::GetOldReportsReal");

  # get todo QA keys
  $self->ToDoKeys("Db_update_utilities::GetToDoReportKeysReal");

  # browser banner for interactive display
  $self->BrowserBannerColor("red");
  $self->BrowserBannerTextColor("darkblue");
  $self->BrowserBannerLabel("Real Data Production");

}
#========================================================
sub offline_MC{

  my $self = shift;
  @_ and my @args = @_;
  #------------------------------------------------------

  my $home = $self->Home();

  $self->TopDir("$home/offline_MC");
  $self->TopDirWWW("http://connery.star.bnl.gov/~starqa/qa");

  $self->StandardDirectories();
  
  # objects to create
  $self->KeyList_obj("KeyList_object_offline_MC");
  $self->QA_obj("QA_object_offline_MC");
  
  # database stuff
  $self->dbFile("operation");
  $self->FileCatalog("FileCatalog");
  $self->JobStatus("JobStatus");
  $self->ProdOptions("ProdOptions");
  $self->JobRelations("jobRelations");
  $self->dbQA("prod_QA");
    
  # for updating from DB
  $self->UpdateRoutine("Db_update_utilities::UpdateQAOfflineMC");

  # get old reports
  $self->GetOldReports("QA_db_utilities::GetOldReportsMC");
  
  # get todo QA keys
  $self->ToDoKeys("Db_update_utilities::GetToDoReportKeysMC");

  # browser banner for interactive display
  $self->BrowserBannerColor("red");
  $self->BrowserBannerTextColor("darkblue");
  $self->BrowserBannerLabel("MC Data Production");

}


#========================================================
sub nightly_real{

  my $self = shift;
  @_ and my @args = @_;
  #------------------------------------------------------
  my $home = $self->Home();
  $self->TopDir("$home/nightly_real");
  $self->TopDirWWW("http://connery.star.bnl.gov/~starqa/qa");

  $self->StandardDirectories();
  
  # objects to create
  $self->KeyList_obj("KeyList_object_nightly_real");
  $self->QA_obj("QA_object_nightly_real");
  
  # database stuff
  $self->dbFile("TestJobs");
  $self->FileCatalog("FilesCatalog");
  $self->JobStatus("JobStatus");
  $self->dbQA("nightly_QA");
  
  # for updating from DB
  $self->UpdateRoutine("Db_update_utilities::UpdateQANightlyReal");


  # get old reports
  $self->GetOldReports("QA_db_utilities::GetOldReportsReal");

  # get todo QA keys 
  $self->ToDoKeys("Db_update_utilities::GetToDoReportKeysReal");

  # browser banner for interactive display
  $self->BrowserBannerColor("red");
  $self->BrowserBannerTextColor("darkblue");
  $self->BrowserBannerLabel("Real Data Nightly Tests");

}
#========================================================
sub nightly_MC{

  my $self = shift;
  @_ and my @args = @_;
  #------------------------------------------------------
  my $home = $self->Home();
  $self->TopDir("$home/nightly_MC");
  $self->TopDirWWW("http://connery.star.bnl.gov/~starqa/qa");

  $self->StandardDirectories();
  
  # objects to create
  $self->KeyList_obj("KeyList_object_nightly_MC");
  $self->QA_obj("QA_object_nightly_MC");
  
  # database stuff
  $self->dbFile("TestJobs");
  $self->FileCatalog("FilesCatalog");
  $self->JobStatus("JobStatus");
  $self->dbQA("nightly_QA");
  
  # for updating from DB
  $self->UpdateRoutine("Db_update_utilities::UpdateQANightlyMC");


  # get old reports
  $self->GetOldReports("QA_db_utilities::GetOldReportsMC");

  # get todo QA keys
  $self->ToDoKeys("Db_update_utilities::GetToDoReportKeysMC");

  # browser banner for interactive display
  $self->BrowserBannerColor("red");
  $self->BrowserBannerTextColor("darkblue");
  $self->BrowserBannerLabel("MC Data Nightly Tests");

}
#========================================================
sub debug{

  my $self = shift;
  @_ and my @args = @_;
  #------------------------------------------------------
  my $home = $self->Home();

  $self->TopDir("$home/debug");
  $self->TopDirWWW("http://connery.star.bnl.gov/~starqa/qa");

  $self->StandardDirectories();
  
  $self->KeyList_obj("KeyList_object_nightly_MC");
  $self->QA_obj("QA_object_nightly_MC");
  
  # database stuff
  $self->dbFile("TestJobs");
  $self->FileCatalog("FilesCatalog");
  $self->JobStatus("JobStatus");
  $self->dbQA("debug_QA");
 

  # for updating from DB
  $self->UpdateRoutine("Db_update_utilities::UpdateQANightlyMC");


  # get old reports
  $self->GetOldReports("QA_db_utilities::GetOldReportsMC");

  # get todo QA keys
  $self->ToDoKeys("Db_update_utilities::GetToDoReportKeysMC");

  # browser banner for interactive display
  $self->BrowserBannerColor("silver");
  $self->BrowserBannerTextColor("maroon");
  $self->BrowserBannerLabel("Debug");

}
#========================================================
sub online_raw{

  my $self = shift;
  @_ and my @args = @_;
  #------------------------------------------------------
  my $home = $self->Home();
  $self->TopDir("$home/online_raw");
  $self->TopDirWWW("http://onllinux1.star.bnl.gov/~starqa/qa");

  $self->StandardDirectories();

  # raw or reco
  $self->DataType("raw");

  # topdir of the summary hist files
  $self->SummaryHistDir($self->DataPool . "/raw/summary");

  $self->KeyList_obj("KeyList_object_online");
  $self->QA_obj("QA_object_online");
  
  # database stuff
  $self->dbQA("onlineRaw_QA");
 
  # utilities for KeyList_object
  $self->GetSelectedKeys("Db_KeyList_utilities::GetOnlineKeys");
  
  # for updating from DB
  $self->UpdateRoutine("Db_update_utilities::UpdateOnline");

  # get old reports
  $self->GetOldReports("QA_db_utilities::GetOldReportsReal");

  # get todo QA keys
  $self->ToDoKeys("Db_update_utilities::GetToDoReportKeys");

  # browser banner for interactive display
  $self->BrowserBannerColor("silver");
  $self->BrowserBannerTextColor("maroon");
  $self->BrowserBannerLabel("Raw Data");

}
#========================================================
sub online_reco{

  my $self = shift;
  @_ and my @args = @_;
  #------------------------------------------------------
  my $home = $self->Home();
  $self->TopDir("$home/online_reco");
  $self->TopDirWWW("http://onllinux1.star.bnl.gov/~starqa/qa");

  $self->StandardDirectories();

  $self->DataType('reco');

  # topdir of the summary hist files
  $self->SummaryHistDir($self->DataPool . "/reco/summary");

  $self->KeyList_obj("KeyList_object_online");
  $self->QA_obj("QA_object_online");
  
  # database stuff
   $self->dbQA("onlineReco_QA");

  # utilities for KeyList_object
  $self->GetSelectedKeys("Db_KeyList_utilities::GetOnlineKeys");
  
  # for updating from DB
  $self->UpdateRoutine("Db_update_utilities::UpdateOnline");

  # get old reports
#  $self->GetOldReports("QA_db_utilities::GetOldReportsReal");

  # get todo QA keys
  $self->ToDoKeys("Db_update_utilities::GetToDoReportKeys");

  # browser banner for interactive display
  $self->BrowserBannerColor("silver");
  $self->BrowserBannerTextColor("maroon");
  $self->BrowserBannerLabel("DST Data");


}
#========================================================
sub online_debug{

  my $self = shift;
  @_ and my @args = @_;
  #------------------------------------------------------
  my $home = $self->Home();
  $self->TopDir("$home/online_debug");
  $self->TopDirWWW("http://onllinux1.star.bnl.gov/~starqa/qa");

  $self->StandardDirectories();

  $self->DataType('reco');

  # topdir of the summary hist files
  $self->SummaryHistDir($self->DataPool . "/reco/summary");

  $self->KeyList_obj("KeyList_object_online");
  $self->QA_obj("QA_object_online");
  
  # database stuff
   $self->dbQA("onlineDebug_QA");

  # utilities for KeyList_object
  $self->GetSelectedKeys("Db_KeyList_utilities::GetOnlineKeys");
  
  # for updating from DB
  $self->UpdateRoutine("Db_update_utilities::UpdateOnline");

  # get old reports
#  $self->GetOldReports("QA_db_utilities::GetOldReportsReal");

  # get todo QA keys
  $self->ToDoKeys("Db_update_utilities::GetToDoReportKeys");

  # browser banner for interactive display
  $self->BrowserBannerColor("yellow");
  $self->BrowserBannerTextColor("maroon");
  $self->BrowserBannerLabel("Online Debug");


}
