#! /usr/bin/perl
#
# derived Logreport_object for offline 
# this is a kluge.
# contains both information for offline real and offline MC
# the difference is controlled by the global DataClass object
# which determines which _initplus to call
# oh well.  
# 
#==========================================================
package Logreport_object_offline;
#==========================================================
use CGI qw/:standard :html3/;
use QA_globals;
use FileHandle;
use QA_db_utilities;
use Data::Dumper;

use strict;
use base qw(Logreport_object); # base class
#==========================================================
# more members

my %members = (
	        _RunID            => undef,     
	        _FileSeq          => undef,
	        _ProdSeries       => undef,
	        _ChainName        => undef, # abbrev of the chain
	        _EventGenDetails  => undef, # more info on the generator
	        _CollisionType    => undef, # e.g. auau200
	        _Dataset          => undef,
	        _Redone           => undef
	      );

#=========================================================
# 
sub new{
  my $classname = shift;
  my $self      = $classname->SUPER::new(@_) or return;  

  defined $self or return; # get out if something went wrong

  if (defined %members){
    # using SUPER::AUTOLOAD
    foreach my $element (keys %members) {
      $self->{_permitted}->{$element} = $members{$element};
    }
    # more members
    @{$self}{keys %members} = values %members;
  }

  $self->_init_offline();
 
  # depend on the DataClass_object to detemine which sub to call
  # DataClass should be 'offline_real' or 'offline_MC'

  my $init_dataclass = $gDataClass_object->DataClass;

  $self->$init_dataclass(); # even more initialization

  return $self;
}
#===========================================================
sub _init_offline{
  my $self = shift;

  # runID, file seq, dataset
  $self->RunID(QA_db_utilities::GetFromFileCatalog('runID',$self->JobID) );
  $self->FileSeq(QA_db_utilities::GetFromFileCatalog('fileSeq',$self->JobID));
  $self->Dataset(QA_db_utilities::GetFromFileCatalog('dataset',$self->JobID));
  $self->Redone(QA_db_utilities::GetFromFileCatalog('redone',$self->JobID));

  # get prod series, chain name, lib version, and chain options
  
  my ($prodSeries, $chainName, $lib, $chain) 
    = QA_db_utilities::GetProdOptions($self->JobID);
  
  $self->ProdSeries($prodSeries);
  $self->ChainName($chainName);
  $self->StarlibVersion($lib);
  $self->RequestedChain($chain);

}
#=======================================================
# additional init for offline real

sub offline_real{
  
  my $self = shift;
  
  # first and last event done (processed)
  my $lo = QA_db_utilities::GetFromFileCatalog('NevLo',$self->JobID);
  my $hi = QA_db_utilities::GetFromFileCatalog('NevHi', $self->JobID);

  $self->FirstEventDone($lo);
  $self->LastEventDone($hi); 

  # kill the NEventRequested field
  # this doesnt make sense for offline real data
  
  $self->NEventRequested(undef);

  # parse the dataset column
  
}

#===========================================================
# additional init for offline MC

sub offline_MC{
  my $self = shift;

  # parse the dataset column

  my ($collisionType, $eventGen, $details, $eventType, $geometry, $junk)=
    split /\//, $self->Dataset, 6;

  $self->CollisionType($collisionType);
  $self->EventGen($eventGen);
  $self->EventGenDetails($details);
  $self->EventType($eventType);
  $self->Geometry($geometry);

}
  
#==========================================================
# get the log file - called in SUPER::_init

sub GetLogFile{
  my $self  = shift;

  return QA_db_utilities::GetOfflineLogFile( $self->JobID );
}

#==========================================================
# parse the summary of the log file
# also parses the separate error file which contains
# the StError and StWarning info among other things

sub ParseLogfile{
  my $self = shift;

  my $logfile = $self->LogfileName; # found in SUPER::_init

  # change the logfile to the summary of the log file

  (my $sumfile = $logfile) =~ s|/log/|/sum/|g; # change the path
  $sumfile =~ s/\.log$/\.sum/;                 # change extension

  my $fh_sum = FileHandle->new( $sumfile, "r" ) or return;

  print "Found summary of logfile $sumfile\n" , br;

  # read the log file (actually summary of the log file)
  
  while (defined (my $line = $fh_sum->getline )) {

    # start time  
    if ($line =~ /Starting job execution at (.*?)on/){
      $self->{_JobStartTimeAndDate} = $1;
      next;
    }

    # star level - e.g. new
    if ($line =~ /STAR_LEVEL : (\w+)/ ) {
      $self->{_StarLevel} = $1; next;
    }

    # root level - e.g. 2.23.12
    if ($line =~ /ROOT_LEVEL : ([\d\.]+)/ ) {
      $self->{_RootLevel} =  $1; next;
    }
							  
    # run options - e.g. StDbT is ON - more than one line
    if ( $line =~ /^QAInfo: =====/ ){
      $line =~ s/QAInfo://;  # get rid of leading QAInfo
      my $run_option = $self->RunOptions;
      if ( not $run_option ){ $self->RunOptions("\n")}
      else { $self->RunOptions($run_option.$line) }
      next;
    }

    # first and last event requested 
    # only good for offline MC
    # offline real overwrites these with undef

    if ($line =~ /^QAInfo:\s*Process/){
      $line =~ /First=\s+(\d+)\/Last=\s+(\d+)\/Total=\s+(\d+)/;
      $self->{_FirstEventRequested} = $1;
      $self->{_LastEventRequested}  = $2;
      $self->{_NEventRequested} = $3;
      next;
    }

    # error string 
    if ($line =~ /Extra error message\W+(.*)/){
      $self->ErrorString($1); next;
    }

    # timing string - more than one line
    if ( $line =~/Real Time/ ){
      my $timing = $self->TimingString;
      $self->TimingString($timing.$line);
      next;
    }
  }
  
  # deduce the error file
  (my $errorfile = $logfile) =~ s/log$/err/;

  my $fh_error  = FileHandle->new($errorfile, "r");

  if (defined $fh_error)
  {

    # init StWarning and StError files
    my $io_warn = new IO_object("StWarningFile",$self->ReportKey);
    my $io_err  = new IO_object("StErrorFile",$self->ReportKey);
    
    my $FH_WARN  = $io_warn->Open(">", "0664");
    my $FH_ERR   = $io_err->Open(">", "0664");
 
    # print to StError and StWarning file
    while( my $line = <$fh_error> ){
      print $FH_ERR  $line if $line =~ /StError:/;
      print $FH_WARN $line if $line =~ /StWarning:/;

    }
    close $FH_ERR; close $FH_WARN;
  } 

  return 1;
}
#===========================================================
# gets some info about the job from the db

sub GetJobInfo{
  my $self = shift;

  # -- get from JobsStatus
  my @jobFields = ('nodeID', 'NoEvents', 'jobStatus', 'NoEventSkip');

  my ($node, $eventsDone, $jobStatus, $Nskip) =
    QA_db_utilities::GetFromJobStatus(\@jobFields,$self->JobID);
  
  $self->Machine($node);
  $self->NEventDone($eventsDone);
  $self->JobStatus($jobStatus);
  $self->NoEventSkipped($Nskip);  

  # input fn
  my $input = QA_db_utilities::GetInputFnOffline($self->JobID);
  $self->InputFn($input);

  # output directory
  my $path = 
    QA_db_utilities::GetFromFileOnDiskOffline('path',$self->JobID);
  $self->OutputDirectory($path);
  
  # job completion time
  my $donetime = QA_db_utilities::GetFromFileCatalog('createTime',$self->JobID);
  $self->JobCompletionTimeAndDate($donetime);

  # all output files
  my @files = 
    QA_db_utilities::GetFromFileOnDiskOffline('fname', $self->JobID);

  @files = map{ $self->OutputDirectory . "/$_" } @files;
  $self->ProductionFileListRef(\@files);


  # check for missing files.
  # depends on the data class - use global DataClass_object.
  # returns a string.
  
  no strict 'refs';
  my $sub_missing = $gDataClass_object->GetMissingFiles; # name of the sub
  $self->MissingFiles( &$sub_missing($self->JobID) );

  # backwards compatibility
  $self->{_SmallFiles} = QA_db_utilities::GetSmallFilesOffline($self->JobID);
  
  return 1;
}
1;
