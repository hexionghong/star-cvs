#! /usr/bin/perl 
#
# derived QA_object for offline
#
#========================================================
package QA_object_offline;
#========================================================
use CGI qw(:standard :html3);
use IO_object;
use QA_db_utilities;
use Logreport_object_offline;

use base qw(QA_object);

use strict;
1;
#--------------------------------------------------------

sub new{
  my $classname = shift;
  my $self      = $classname->SUPER::new(@_);  

  return $self;
}

#========================================================
# is it on disk?

sub InitOnDisk{
  my $self = shift;

  my $ondisk = QA_db_utilities::OnDiskOffline($self->JobID);
  $self->OnDisk($ondisk);
}

#========================================================

sub InitControlFile{
  my $self = shift;

  $self->{_IOControlFile} = 
    IO_object->new("ControlFileOffline", $self);
 
}
#==========================================================
# (first column)
# identifies the job/dataset in the browser  

sub DataDisplayString{
  my $self = shift;

  my $prodSeries = $self->LogReport->ProdSeries;
  my $chainName  = $self->LogReport->ChainName;
  
  return $self->ReportKey.br.
         "(prodSeries: $prodSeries; chain name: $chainName)";
}


#========================================================
# create the log report object

sub NewLogReportObject{
  my $self = shift;

  return Logreport_object_offline->new($self->ReportKey);
}
1;
