# FileCat.pm

# Written by Adam Kisiel, November-December 2001

# Methods of class FileCatalog:

#        ->new           : create new object FileCatalog
#        ->connect       : connect to the database FilaCatalog
#        ->destroy       : destroy object and disconnect database FileCatalog
#        ->set_context() : set one of the context keywords to the given operator and value
#        ->get_context() : get a context value connected to a given keyword
#        ->clear_context(): clear/reset the context
#        ->get_keyword_list() : get the list of valid keywords
#        ->get_delimeter() : get the current delimiting string
#        ->set_delimeter() : set the current delimiting string

# the following methods require connect to dbtable and are meant to be used outside the module

#        -> check_ID_for_params() : returns the database row ID from the dictionary table
#                          connected to this keyword
#        -> insert_dictionary_value() : inserts the value from the context into the dictionary table
#        -> insert_detector_configuration() : inserts the detector configuration from current context
#        -> get_current_detector_configuration() : gets the ID of a detector configuration described by the 
#                          current context
#        -> insert_run_param_info() : insert the run param record taking data from the current context
#        -> get_current_run_param() : get the ID of a run params corresponding to the current context
#        -> insert_file_data() : inserts file data record taking data from the current context
#        -> get_current_file_data() : gets the ID of a file data corresponding to the current context
#        -> insert_simulation_params() : insert the simulation parameters taking data from the current context
#        -> get_current_simulation_params : gets the ID of a simulation params corresponding to the current contex
#        -> insert_file_location() : insert the file location data taking data from the current context
#        -> run_query()   : get entries from dbtable FileCatalog according to query string defined by set_context
#                          you also give a list of fields to select form
#        -> delete_record() : deletes the current file location. If it finds that the current file data has no
#                          file locations left, it deletes it too
#        -> update_record() : modifies the data in the database. The field corresponding to the given keyword
#                          changes it value from the one in the current context to the one specified as an argument
#        -> bootstrap() : database maintenance procedure. Looks at the dictionary table and find all the records
#                         that are not referenced by the child table. It offers an option of deleting this records.

        
package FileCatalog;

use vars qw($VERSION);
$VERSION   =   0.01;

use DBI;
use strict;

# define to print debug information
my $debug = 1;

# db information
my $dbname    =   "FileCatalog"; 
my $dbhost    =   "duvall.star.bnl.gov";                    
my $dbuser    =   "FC_admin";
my $dbsource  =   "DBI:mysql:$dbname:$dbhost";
my $DBH;
my $sth;

# hash of keywords
my %keywrds;
# $keys{keyword} meaning of the parts of the field:
# 1 - parameter name as entered by the user
# 2 - field name in the database
# 3 - table name in the database for the given field
# 4 - critical for data insertion into the specified table
# 5 - type of the field (text,num,date)
# 6 - not used
# 7 - not used
# only the keywords in this table are accepted in set_context sub

$keywrds{"filetype"      }    =   "fileTypeName"              .",FileTypes"              .",1" .",text" .",0" .",1";
$keywrds{"extension"     }    =   "fileTypeExtension"         .",FileTypes"              .",1" .",text" .",0" .",1";
$keywrds{"storage"       }    =   "storageTypeName"           .",StorageTypes"           .",1" .",text" .",0" .",1";
$keywrds{"site"          }    =   "storageSiteName"           .",StorageSites"           .",1" .",text" .",0" .",1";
$keywrds{"production"    }    =   "productionTag"             .",ProductionConditions"   .",1" .",text" .",0" .",1";
$keywrds{"library"       }    =   "libraryVersion"            .",ProductionConditions"   .",1" .",text" .",0" .",1";
$keywrds{"triggername"   }    =   "triggerName"               .",TriggerWords"           .",1" .",text" .",0" .",1";
$keywrds{"triggerword"   }    =   "triggerWord"               .",TriggerWords"           .",1" .",text" .",0" .",1";
$keywrds{"triggersetup"  }    =   "triggerSetupName"          .",TriggerSetups"          .",1" .",text" .",0" .",1";
$keywrds{"runtype"       }    =   "runTypeName"               .",RunTypes"               .",1" .",text" .",0" .",1";
$keywrds{"configuration" }    =   "detectorConfigurationName" .",DetectorConfigurations" .",1" .",text" .",0" .",1";
$keywrds{"geometry"      }    =   "detectorConfigurationName" .",DetectorConfigurations" .",0" .",text" .",0" .",1";
$keywrds{"runnumber"     }    =   "runNumber"                 .",RunParams"              .",1" .",num"  .",0" .",1";
$keywrds{"runcomments"   }    =   "runComments"               .",RunParams"              .",0" .",text" .",0" .",1";
$keywrds{"collision"     }    =   "collisionEnergy"           .",CollisionTypes"         .",1" .",text" .",0" .",1";
$keywrds{"datetaken"     }    =   "dataTakingStart"           .",RunParams"              .",0" .",date" .",0" .",1";
$keywrds{"magscale"      }    =   "magFieldScale"             .",RunParams"              .",1" .",text" .",0" .",1";
$keywrds{"magvalue"      }    =   "magFieldValue"             .",RunParams"              .",0" .",num"  .",0" .",1";
$keywrds{"filename"      }    =   "filename"                  .",FileData"               .",1" .",text" .",0" .",1";
$keywrds{"size"          }    =   "size"                      .",FileData"               .",1" .",num"  .",0" .",1";
$keywrds{"fileseq"       }    =   "fileSeq"                   .",FileData"               .",1" .",num"  .",0" .",1";
$keywrds{"filecomment"   }    =   "fileDataComments"          .",FileData"               .",0" .",text" .",0" .",1";
$keywrds{"owner"         }    =   "owner"                     .",FileLocations"          .",0" .",text" .",0" .",1";
$keywrds{"protection"    }    =   "protection"                .",FileLocations"          .",0" .",text" .",0" .",1";
$keywrds{"node"          }    =   "nodeName"                  .",FileLocations"          .",0" .",text" .",0" .",1";
$keywrds{"available"     }    =   "availability"              .",FileLocations"          .",0" .",text" .",0" .",1";
$keywrds{"persistent"    }    =   "persistent"                .",FileLocations"          .",0" .",text" .",0" .",1";
$keywrds{"sanity"        }    =   "sanity"                    .",FileLocations"          .",0" .",num"  .",0" .",1";
$keywrds{"createtime"    }    =   "createTime"                .",FileLocations"          .",0" .",date" .",0" .",1";
$keywrds{"inserttime"    }    =   "insertTime"                .",FileLocations"          .",0" .",date" .",0" .",1";
$keywrds{"path"          }    =   "filePath"                  .",FileLocations"          .",1" .",text" .",0" .",1";
$keywrds{"simcomment"    }    =   "simulationParamComments"   .",SimulationParams"       .",0" .",text" .",0" .",1";
$keywrds{"generator"     }    =   "eventGeneratorName"        .",EventGenerators"        .",1" .",text" .",0" .",1";
$keywrds{"genversion"    }    =   "eventGeneratorVersion"     .",EventGenerators"        .",1" .",text" .",0" .",1";
$keywrds{"gencomment"    }    =   "eventGeneratorComment"     .",EventGenerators"        .",0" .",text" .",0" .",1";
$keywrds{"genparams"     }    =   "eventGeneratorParams"      .",EventGenerators"        .",1" .",text" .",0" .",1";
$keywrds{"tpc"           }    =   "dTPC"                      .",DetectorConfigurations" .",1" .",num"  .",0" .",1";
$keywrds{"svt"           }    =   "dSVT"                      .",DetectorConfigurations" .",1" .",num"  .",0" .",1";
$keywrds{"tof"           }    =   "dTOF"                      .",DetectorConfigurations" .",1" .",num"  .",0" .",1";
$keywrds{"emc"           }    =   "dEMC"                      .",DetectorConfigurations" .",1" .",num"  .",0" .",1";
$keywrds{"fpd"           }    =   "dFPD"                      .",DetectorConfigurations" .",1" .",num"  .",0" .",1";
$keywrds{"ftpc"          }    =   "dFTPC"                     .",DetectorConfigurations" .",1" .",num"  .",0" .",1";
$keywrds{"pmd"           }    =   "dPMD"                      .",DetectorConfigurations" .",1" .",num"  .",0" .",1";
$keywrds{"rich"          }    =   "dRICH"                     .",DetectorConfigurations" .",1" .",num"  .",0" .",1";
$keywrds{"ssd"           }    =   "dSSD"                      .",DetectorConfigurations" .",1" .",num"  .",0" .",1";
$keywrds{"triggerevents" }    =   "numberOfEvents"            .",TriggerCompositions"    .",1" .",text" .",0" .",1";
$keywrds{"events"        }    =   "numberOfEvents"            .",TriggerCompositions"    .",1" .",num"  .",0" .",1";
$keywrds{"simulation"    }    =   ",,,,,";
$keywrds{"nounique"      }    =   ",,,,,";
$keywrds{"noround"       }    =   ",,,,,";
$keywrds{"startrecord"   }    =   ",,,,,";
$keywrds{"limit"         }    =   ",,,,,";
$keywrds{"all"           }    =   ",,,,,";

# Fields that need to be rounded when selecting from the database
my $roundfields = "magFieldValue,2 collisionEnergy,0";

# The delimeter to sperate fields at output
my $delimeter = "::";

# The hashes that hold a current context
my %operset;
my %valuset;

# The list of connections between tables in the database
# needed to build queries with joins
# fields:
# 1 - Table being linked to
# 2 - Table that links the given table
# 3 - Name of the linking field in both tables
# 4 - "Level" of the table in the DB structure 
#     The table is at level 1, if it not directly referenced by any other table
#     The table is at level 2 if it is directly referenced by a table at level 1 
#     etc.

my @datastruct;
$datastruct[0]  = ( "StorageTypes"           . ",FileLocations"       . ",storageTypeID"           . ",2");
$datastruct[1]  = ( "StorageSites"           . ",FileLocations"       . ",storageSiteID"           . ",2");
$datastruct[2]  = ( "FileData"               . ",FileLocations"       . ",fileDataID"              . ",2");
$datastruct[3]  = ( "ProductionConditions"   . ",FileData"            . ",productionConditionID"   . ",3");
$datastruct[4]  = ( "FileTypes"              . ",FileData"            . ",fileTypeID"              . ",3");
$datastruct[5]  = ( "TriggerWords"           . ",TriggerCompositions" . ",triggerWordID"           . ",2");
$datastruct[6]  = ( "FileData"               . ",TriggerCompositions" . ",fileDataID"              . ",2");
$datastruct[7]  = ( "RunParams"              . ",FileData"            . ",runParamID"              . ",3");
$datastruct[8]  = ( "RunTypes"               . ",RunParams"           . ",runTypeID"               . ",4");
$datastruct[9]  = ( "DetectorConfigurations" . ",RunParams"           . ",detectorConfigurationID" . ",4");
$datastruct[10] = ( "CollisionTypes"         . ",RunParams"           . ",collisionTypeID"         . ",4");
$datastruct[11] = ( "TriggerSetups"          . ",RunParams"           . ",triggerSetupID"          . ",4");
$datastruct[12] = ( "SimulationParams"       . ",RunParams"           . ",simulationParamsID"      . ",4");
$datastruct[13] = ( "EventGenerators"        . ",SimulationParams"    . ",eventGeneratorID"        . ",5");
$datastruct[14] = ( "FileLocations"          . ","                    . ","                        . ",1");
$datastruct[15] = ( "TriggerCompositions"    . ","                    . ","                        . ",1");


# The operators allowed in set_context query - two-characters operators first
my @operators;
$operators[0] = "<=";
$operators[1] = ">=";
$operators[2] = "<>";
$operators[3] = "!=";
$operators[4] = "==";
$operators[5] = "!~";
$operators[6] = "=";
$operators[7] = ">";
$operators[8] = "<";
$operators[9] = "~";

# The possible aggregate values
my @aggregates;
$aggregates[0] = "sum";
$aggregates[1] = "avg";
$aggregates[2] = "min";
$aggregates[3] = "max";
$aggregates[4] = "grp";
$aggregates[5] = "orda";
$aggregates[6] = "ordd";

#============================================
# parse keywrds - get the field name for the given keyword
# Parameters:
# keyword to get the field name for
# Returns:
# field name for a given context keyword
sub get_field_name {
  my @params = @_;

  my ($fieldname, $tabname, $rest) = split(",",$keywrds{$params[0]});
  return $fieldname;
}

#============================================
# parse keywrds - get the table name for the given keyword
# Parameters:
# keyword to get the table name for
# Returns:
# table name for a given context keyword
sub get_table_name {
  my ($mykey) = (@_);
  my ($tabname, $a, $b);


  if (exists $keywrds{$mykey})
    {
      ($a,$tabname,$b) = split(",",$keywrds{$mykey});
    }
  else
    {
      print "Using non-existent key: $mykey !!!!!!!\n";
    }
  return $tabname;

}

#============================================
# get the list o valid keywords
# Returns:
# the list of valid keyowrds to use in FileCatalog queries
sub get_keyword_list {

  return (keys %keywrds);
}

#============================================
# change the deleimiting string between output fields
# Parameters:
# new deliemiting string
sub set_delimeter {
  if ($_[0] =~ m/FileCatalog/) {
    shift @_;
  }
  my @params = @_;

  $delimeter = $params[0];
}

#============================================
# get the current delimiting string
# Returns:
# current delimiting string
sub get_delimeter {
  return $delimeter;
}

#============================================
# parse keywrds - get the field type
# Parameters:
# keyword to get the type for
# Returns:
# type of the field for a given keyword
sub get_field_type {
  my @params = @_;

  my ($fieldname, $tabname, $req, $type, $rest) = split(",",$keywrds{$params[0]});
  return $type;
}

#============================================
# parse keywrds - see if the field is required in insert statement
# Parameters:
# keyword to check
# Returns:
# 0 if field is not needed
# 1 if field is mandatory for inserts
sub is_critical {
  my @params = @_;

  my ($fieldname, $tabname, $req, $type, $rest) = split(",",$keywrds{$params[0]});
  return $req;
}

#============================================
sub new {
  my $class= shift;
  my $self  = {};
  $self->{values} = [];
  $self->{entries} = undef;
  
  $delimeter = "::";
  $valuset{"all"} = 0;

  # Only way to bless it is to declare them inside
  # new(). See also use Symbol; and usage of my $bla = gensym;
  #my %operset=
  #my %valuset=

  bless($self);
  bless(\%valuset, "FileCatlog");
  bless(\%operset, "FileCatlog");

  return $self;
}

#============================================
sub connect {
  my $self  = shift;
  $DBH = DBI->connect($dbsource,$dbuser,"Kisiel") || die "cannot connect to $dbname : $DBI::errstr\n";
  if ( !defined($DBH) ) {
    return 0;
  }
  1; 
}

#============================================
# disentangle keyowrd, operator and value from a context string
# Params:
# the context string
# Returns:
# keyword - the keyword used
# operator - the operator used
# value - the value assigned to the keyword
sub disentangle_param {

  if ($_[0] =~ m/FileCatalog/) {
    shift @_;
  }
  ;
  my ($params) = @_;
  my $keyword;
  my $operator;
  my $value;

 OPS: foreach (@operators )
    {
      ($keyword, $value) = $params =~ m/(.*)$_(.*)/;
      $operator = $_;
      last if (defined $keyword and defined $value);
      $operator = "";
    }
  if ($debug > 0) {
    print " Keyword: |".$keyword."|\n";
    print " Value: |".$value."|\n";
  }
  return ($keyword, $operator, $value);
}

#============================================
# Set the context variable
# Params:
# context string in the form of:
# <context variable> <operator> <value>
sub set_context {
  
  if ($_[0] =~ m/FileCatalog/) {
    shift @_;
  }
  ;
  my( $params ) = @_;

  #  print ("Setting context for: $params \n");
  my $keyw;
  my $oper;
  my $valu;

  ($keyw, $oper, $valu) = disentangle_param($params);

  # Chop spaces from the key name and value;
  $keyw =~ y/ //d;
  if ($valu =~ m/.*[\"\'].*[\"\'].*/) {
    $valu =~ s/.*[\"\'](.*)[\"\'].*/$1/;
  } else {
    $valu =~ s/ //g;
  }
  
  if (exists $keywrds{$keyw}) {
    if ($debug > 0) {
      print "Query accepted: ".$keyw."=".$valu."\n";
    }
    $operset{$keyw} = $oper;
    $valuset{$keyw} = $valu;
  }
  else
    {
      if ($debug > 0)
	{ print "ERROR: $keyw is not a valid keyword.\nCannot set context.\n"; }
    }
}

#============================================
# Clears the context deleting all the values
# form the context hashes
sub clear_context {
  foreach my $key (keys %valuset) {
    delete $valuset{$key};
  }
  foreach my $key (keys %operset) {
    delete $operset{$key};
  }
}

#============================================
# Get an ID for a record with a given name
# from a dictionary table
# NOTE: Field values in dictionary tables are CaSe insensitive
# Params: 
# table name
# field name
# field value
# Returns:
# ID of record form a database or 0 if there is no such record
sub get_id_from_dictionary {
  if ($_[0] =~ m/FileCatalog/) {
    shift @_;
  }
  ;
  my @params = @_;
  my $idname = $params[0];
  chop($idname);
  $idname = lcfirst($idname);
  $idname.="ID";

  my $sqlquery = "SELECT $idname FROM $params[0] WHERE UPPER($params[1]) = UPPER(\"$params[2]\")";
  if ($debug > 0) {
    print "Executing: $sqlquery\n";
  }

  my $sth = $DBH->prepare($sqlquery);
  $sth->execute();
  my( $id );
  $sth->bind_columns( \$id );

  if ( $sth->fetch() ) {
    return $id;
  }
  
}

#============================================
# Check if there is a record with a given value
# in a corresponding dictionary table
# Parameters:
# the keyword to check for
# Returns:
# The ID value for a given keyword value
# or 0 if no such record exists
sub check_ID_for_params {
  if ($_[0] =~ m/FileCatalog/) {
    shift @_;
  }
  ;
  my @params = @_;
  my $retid;

  if (defined $valuset{$params[0]}) {
    my $fieldname;
    my $tabname;
    my $rest;
      
    ($fieldname, $tabname, $rest) = split(",",$keywrds{$params[0]});
    $retid = get_id_from_dictionary($tabname, $fieldname, $valuset{$params[0]});
    if ($retid == 0) {
      if ($debug > 0) {
	print "ERROR: No $params[0] with name: ".$valuset{$params[0]}."\n";
      }
      $retid = 0;
    }
  } else {
    if ($debug > 0) {
      print "ERROR: No $params[0] defined\n";
    }
    $retid = 0;
  }
  if ($debug > 0) {
    print "Returning: $retid\n";
  }
  return $retid;
}

#============================================
# inserts a value into a table corresponding to a given keyword
# Paramters:
# A keyword to use
# Returns:
# The ID of an inserted value
# or 0 if such insertion was not possible
sub insert_dictionary_value {
  if ($_[0] =~ m/FileCatalog/) {
    shift @_;
  }
  ;
  my ($keyname) = @_;
  my @additional;
  if (! defined $valuset{$keyname}) {
    if ($debug > 0) {
      print "ERROR: No value for keyword $keyname.\nCannot add record to dictionary table.\n";
    }
    return 0;
  }

  # Check if there are other fields from this table set
  foreach (keys(%keywrds)) {
    my ($fieldnameo, $tabnameo, $resto) = split(",",$keywrds{$keyname});
    my ($fieldnamet, $tabnamet, $restt) = split(",",$keywrds{$_});

    if ($tabnameo eq $tabnamet && $keyname ne $_) {
      if ($debug > 0) {
	print "The field $fieldnamet $tabnamet is from the same table as $fieldnameo $tabnameo\n";
      }
      if (defined $valuset{$_}) {
	push @additional, ($_);
      }
    }
  }
  
  my ($fieldname, $tabname, $rest) = split(",",$keywrds{$keyname});
  my $dtfields = "";
  my $dtvalues = "";
  my $dtinsert;
  foreach (@additional) {
    my ($fieldnamea, $tabnamea, $resta) = split(",",$keywrds{$_});
      
    $dtfields .= " , $fieldnamea";
    $dtvalues .= " , '".$valuset{$_}."'";
  }
  
  $dtinsert   = "INSERT INTO $tabname ";
  $dtinsert  .= "($fieldname $dtfields)";
  $dtinsert  .= " VALUES ('".$valuset{$keyname}."' $dtvalues)";
  if ($debug > 0) {
    print "Execute $dtinsert\n";
  }
  my $sth = $DBH->prepare( $dtinsert );
  if ( $sth->execute() ) {
    my $retid = get_last_id();
    if ($debug > 0) {
      print "Returning: $retid\n";
    }
    return $retid;
  }
  return 0;
}

#============================================
# inserts a value into a table of Detector Configurations
# Returns:
# The ID of an inserted value
# or 0 if such insertion was not possible
sub insert_detector_configuration {
  
  my ($tpcon, $svton, $emcon, $ftpcon, $richon, $fpdon, $tofon, $pmdon, $ssdon);
  if (! defined $valuset{"configuration"}) {
    if ($debug > 0) {
      print "ERROR: No detector configuration/geometry name given.\nCannot add record to the table.\n";
    }
    return 0;
  }
  $tpcon = ($valuset{"tpc"} == 1) ? "1" : "0";
  $svton = ($valuset{"svt"} == 1) ? "1" : "0";
  $emcon = ($valuset{"emc"} == 1) ? "1" : "0";
  $ftpcon = ($valuset{"ftpc"} == 1) ? "1" : "0";
  $richon = ($valuset{"rich"} == 1) ? "1" : "0";
  $fpdon = ($valuset{"fpd"} == 1) ? "1" : "0";
  $tofon = ($valuset{"tof"} == 1) ? "1" : "0";
  $pmdon = ($valuset{"pmd"} == 1) ? "1" : "0";
  $ssdon = ($valuset{"ssd"} == 1) ? "1" : "0";


  my $dtinsert   = "INSERT INTO DetectorConfigurations";
  $dtinsert  .= "(detectorConfigurationName, dTPC, dSVT, dTOF, dEMC, dFPD, dFTPC, dPMD, dRICH, dSSD)";
  $dtinsert  .= " VALUES ('".$valuset{"configuration"}."', $tpcon , $svton , $tofon , $emcon , $fpdon , $ftpcon , $pmdon , $richon , $ssdon)";
  if ($debug > 0) {
    print "Execute $dtinsert\n";
  }
  my $sth = $DBH->prepare( $dtinsert );
  if ( $sth->execute() ) {
    my $retid = get_last_id();
    if ($debug > 0) {
      print "Returning: $retid\n";
    }
    return $retid;
  }
  return 0;
}


#============================================
# get the ID for the current run number
# Returns:
# the ID of a runParams record,
# or 0 if no such record exists
sub get_current_detector_configuration {
  my $detConfiguration;

  $detConfiguration = check_ID_for_params("configuration");
  if ($detConfiguration == 0) {
    # There is no detector configuration with this name
    # we have to add it 
    $detConfiguration = insert_detector_configuration();
  }
  return $detConfiguration;
}

#============================================
# disentangle collision type parameters from the collsion type name
# Params:
# The collsion type
# Returns:
# first particle name
# second particle name
# collision energy
sub disentangle_collision_type {

  my ($colstring) = @_;
  my $firstParticle = "";
  my $secondParticle = "";

  my @particles = ("proton", "gas", "au", "ga", "si", "p", "s");


  if (($colstring =~ m/cosmic/) > 0)
    {
      $firstParticle = "cosmic";
      $secondParticle = "cosmic";
      $colstring = "0.0";
    }
  else
    {
      foreach (@particles)
	{
	  if (($colstring =~ m/$_/) > 0) {
	    $firstParticle = $_;
	    $colstring =~ s/$_(.*)/$1/;
	    last;
	  }      
	}
      foreach (@particles)
	{
	  if (($colstring =~ m/$_/) > 0) {
	    $secondParticle = $_;
	    $colstring =~ s/(.*)$_(.*)/$1$2/;
	    last;
	  }
	}
    }
  return ($firstParticle, $secondParticle, $colstring);
}

#============================================
# disentangle collision type parameters from the collsion type name
# Params:
# The collsion type to find
# Returns:
# the id of a collision type in DB or 0 if there is no such collsion type
sub get_collision_type {
  if ($_[0] =~ m/FileCatalog/) {
    shift @_;
  };

  my ($colstring) = @_;
  my $retid;
  my $firstParticle;
  my $secondParticle;
  my $energy;

#  my @particles = ("au", "p", "ga", "s");
  
  $colstring = lc($colstring);

  ($firstParticle, $secondParticle, $energy) = disentangle_collision_type($colstring);

  print "First particle: $firstParticle\nSecond particle: $secondParticle\nEnergy: $colstring\n";
  my $sqlquery = "SELECT collisionTypeID FROM CollisionTypes WHERE UPPER(firstParticle) = UPPER(\"$firstParticle\") AND UPPER(secondParticle) = UPPER(\"$secondParticle\") AND ROUND(collisionEnergy) = ROUND($energy)";
  if ($debug > 0) {
    print "Executing: $sqlquery\n";
  }

  my $sth = $DBH->prepare($sqlquery);
  $sth->execute();
  my( $id );
  $sth->bind_columns( \$id );

  if ( $sth->fetch() ) {
    #    print "Returning: $id\n";
    return $id;
  }
  if ($debug > 0) {
    print "ERROR: No such collsion type\n";
  }
  return 0;
  
}

#============================================
# insert a given collision tye into the database
# Returns:
# the id of a collision type in DB
# or 0 if the insertion was not possible
sub insert_collision_type {

  my $colstring = $valuset{"collision"};
  my $retid;
  my $firstParticle;
  my $secondParticle;
  my $energy;
  
#  my @particles = ("au", "p", "ga", "s");

  $colstring = lc($colstring);

  ($firstParticle, $secondParticle, $energy) = disentangle_collision_type($colstring);
  
  my $ctinsert   = "INSERT INTO CollisionTypes ";
  $ctinsert  .= "(firstParticle, secondParticle, collisionEnergy)";
  $ctinsert  .= " VALUES ('$firstParticle' , '$secondParticle' , $energy)";
  if ($debug > 0) {
    print "Execute $ctinsert\n";
  }
  my $sth = $DBH->prepare( $ctinsert );
  if ( $sth->execute() ) {
    my $retid = get_last_id();
    if ($debug > 0) {
      print "Returning: $retid\n";
    }
    return $retid;
  }
  return 0;
}

#============================================
# Get the ID of a last inserted record from the database
# Returns:
# The ID of a most recently successfully added record
sub get_last_id {
  my $sqlquery = "SELECT LAST_INSERT_ID()";

  $sth = $DBH->prepare($sqlquery);
  $sth->execute();
  my( $id );
  $sth->bind_columns( \$id );

  if ( $sth->fetch() ) {
    return $id;
  }
  if ($debug > 0) {
    print "ERROR: Cannot find the last inserted ID\n";
  }
  return 0;

}

#============================================
# Check for the availability of all parameters
# and execute run param info insert query
# Return:
# The id of the inserted run params
# 0 if there is insufficient data to insert run params
sub insert_run_param_info {

  my $triggerSetup;
  my $collisionType;
  my $runType;
  my $detConfiguration;
  my $comment;
  my $start;
  my $end;
  my $collision;
  my $simulation;
  my $magvalue;

  $triggerSetup = check_ID_for_params("triggersetup");
  $runType = check_ID_for_params("runtype");
  $detConfiguration = check_ID_for_params("configuration");

  if (! defined $valuset{"runnumber"}) {
    if ($debug > 0) {
      print "ERROR: Cannot add runtype\nrunnumber not defined.\n";
    }
    return 0;
  }
  if (defined $valuset{"collision"}) {
    $collision = get_collision_type($valuset{"collision"});
    if ($debug > 0) {
      print "Collsion: $collision\n";
    }
  } else {
    if ($debug > 0) 
      {
	print "ERROR: collsion not defined\n";
      }
  }
  if (($triggerSetup == 0) || ($runType == 0) || ($detConfiguration == 0) || ($collision == 0)) {
    if ($debug > 0) {
      print "ERROR: Cannot add run\nAborting file insertion query\n";
    }
    return 0;
  }
  if (! defined $valuset{"runcomments"}) {
    $comment = "NULL";
  } else {
    $comment = "\"".$comment."\"";
  }
  if (! defined $valuset{"magscale"}) {
    if ($debug > 0) {
      print "ERROR: Cannot add runtype\nmagscale not defined.\n";
    }
    return 0;
  }
  if (! defined $valuset{"magvalue"}) {
    $magvalue = "NULL";
  } else {
    $magvalue = $valuset{"magvalue"};
  }
  if (! defined $valuset{"datetaken"}) {
    $start = "NULL";
    $end = "NULL";
  } else {
    $start = "\"".$valuset{"datetaken"}."\"";
    $end = "\"".$valuset{"datetaken"}."\"";
  }
  if ((defined $valuset{"simulation"}) && (! ($valuset{"simulation"} eq '0') ) ) {
    if ($debug > 0) {
      print "Adding simulation data.\n";
    }
    $simulation = get_current_simulation_params();
  } else {
    $simulation = "NULL";
  }
  
  
  my $rpinsert;
  
  $rpinsert   = "INSERT INTO RunParams ";
  $rpinsert  .= "(runNumber, dataTakingStart, dataTakingEnd, triggerSetupID, collisionTypeID, simulationParamsID, runTypeID, detectorConfigurationID, runComments, magFieldScale, magFieldValue)";
  $rpinsert  .= " VALUES (".$valuset{"runnumber"}.",$start,$end,$triggerSetup,$collision,$simulation,$runType,$detConfiguration,$comment,'".$valuset{"magscale"}."',$magvalue)";
  if ($debug > 0) {
    print "Execute $rpinsert\n";
  }
  my $sth = $DBH->prepare( $rpinsert );
  my $retid;
  # Insert the event counts for a given FileData
  if ( $sth->execute() ) {
    $retid = get_last_id();
    if ($debug > 0) {
      print "Returning: $retid\n";
    }
  } else {
    return 0;
  }
  return $retid;
}

#============================================
# get the ID for the current run number
# Returns:
# the ID of a runParams record,
# or 0 if no such record exists
sub get_current_run_param {
  my $runNumber;

  $runNumber = check_ID_for_params("runnumber");
  if ($runNumber == 0) {
    # There is no run with this run number
    # we have to add it 
    $runNumber = insert_run_param_info();
  }
  return $runNumber;
}

#============================================
# Execute the query to insert the file data
# Check if we have all the information
# Returns:
# The id of a lastly inserted file data
# or 0 is there is insufficient data to insert a record
sub insert_file_data {
  my @params = @_;
  # Get the ID's of the dictionary tales
  my $production;
  my $library;
  my $fileType;
  my $runNumber;
  my $size;
  my $fileComment;
  my $fileSeq;
  my @triggerWords;
  my @eventCounts;
  my @triggerIDs;
  my $count;

  $production = check_ID_for_params("production");
  $library = check_ID_for_params("library");
  $fileType= check_ID_for_params("filetype");

  return 0 if ((($production == 0 ) && ($library == 0)) || $fileType == 0);

  if ($production == 0) {
    $production = $library;
  }
  $runNumber = get_current_run_param();
  if ($runNumber == 0) {
    if ($debug > 0) {
      print "Could not add run data\nAborting file insertion query\n";
    }
    return 0;
  }
  if (! defined $valuset{"filename"}) {
    if ($debug > 0) {
      print "ERROR: Cannot add file data\nfilename not defined.\n";
    }
    return 0;
  }
  if (! defined $valuset{"size"}) {
    $size = "NULL";
  } else {
    $size = $valuset{"size"};
  }

  if (! defined $valuset{"filecomment"}) {
    $fileComment = "NULL";
  } else {
    $fileComment = "\"".$valuset{"filecomment"}."\"";
  }
  if (! defined $valuset{"fileseq"}) {
    $fileSeq = "NULL";
  } else {
    $fileSeq = "\"".$valuset{"fileseq"}."\"";
  }
  # Try to disentangle the triggerword / event count combinations
  # tom the triggerevents string. It should have a format:
  # '<triggerword> <number of events> [ ; <triggerword> <number of events> ]'
  if (! defined $valuset{"triggerevents"}) {
    if ($debug > 0) {
      print "ERROR: Cannot add runtype\nevents for triggerWords not defined.\n";
    }
    return 0;
  } else {
    my @splitted;
    my $count = 0;
    (@splitted) = split(";",$valuset{"triggerevents"});
    foreach (@splitted) {
      ($triggerWords[$count], $eventCounts[$count]) = split(" ");
      if ($debug > 0) {
	print "Added triggerword ".$triggerWords[$count]." with event count ".$eventCounts[$count]."\n";
      }
      $triggerIDs[$count] = get_id_from_dictionary("TriggerWords","triggerName",$triggerWords[$count]);
      if ($triggerIDs[$count] == 0) {
	if ($debug > 0) {
	  print "Warnig: no triggerID for triggerword $triggerWords[$count]\n";
	}
      }
      $count++;
    }
  }

  # Prepare the SQL query and execute it
  my $fdinsert   = "INSERT INTO FileData ";
  $fdinsert  .= "(runParamID, fileName, productionConditionID, fileTypeID, size, fileDataComments, fileSeq)";
  $fdinsert  .= " VALUES ($runNumber, \"".$valuset{"filename"}."\",$production,$fileType,$size,$fileComment,$fileSeq)";
  if ($debug > 0) {
    print "Execute $fdinsert\n";
  }
  my $sth = $DBH->prepare( $fdinsert );
  my $retid;
  if ( $sth->execute() ) {
    $retid = get_last_id();
    if ($debug > 0) {
      print "Returning: $retid\n";
    }
  } else {
    return 0;
  }

  # Add the triggerword / event count combinations for this FileData
  if ($debug > 0) {
    print "Adding triggerWords $#triggerWords\n";
  }
  for ($count = 0; $count < ($#triggerWords+1); $count++) {
    if ($debug > 0) {
      print "Adding triggerWord $count\n";
    }
    my $tcinsert   = "INSERT INTO TriggerCompositions ";
    $tcinsert  .= "(fileDataID, triggerWordID, numberOfEvents)";
    $tcinsert  .= " VALUES ( $retid , ".$triggerIDs[$count]." , ".$eventCounts[$count].")";
    if ($debug > 0) {
      print "Execute $tcinsert\n";
    }
    my $sth = $DBH->prepare( $tcinsert );
    if ( ! $sth->execute() ) {
      if ($debug > 0) {
	print "ERROR: did not add the event count for triggerword $triggerWords[$count]\n";
      }
    }
  }
  return $retid;
  
}

#============================================
# get the ID for the current file data, or create it
# if not found.
# Returns:
# the ID of a fileData record
# or 0 if no such fielData, cannot create it, or more than one record exists for a given context.
sub get_current_file_data {
  my $runParam;
  my $fileName;
  my $production;
  my $library;
  my $fileType;
  my $fileSeq;
  my $sqlquery;

  $runParam = get_current_run_param();
  $production = check_ID_for_params("production");
  $library = check_ID_for_params("library");
  $fileType = check_ID_for_params("filetype");
  if ($production == 0) {
    $production = $library;
  }
  if ( $runParam != 0) {
    $sqlquery .= " runParamID = $runParam AND ";
  }
  if (defined $valuset{"filename"}) {
    $sqlquery .= " fileName = '".$valuset{"filename"}."' AND ";
  }
  if (defined $valuset{"fileseq"}) {
    $sqlquery .= " fileSeq = '".$valuset{"fileseq"}."' AND ";
  }
  if ( $production != 0) {
    $sqlquery .= " productionConditionID = $production AND ";
  }
  if ( $fileType != 0) {
    $sqlquery .= " fileTypeID = $fileType AND ";
  }
  if (defined $sqlquery) {
    $sqlquery =~ s/(.*)AND $/$1/g;
  } else {
    if ($debug > 0) {
      print "No parameters set to identify File Data\n";
    }
    return 0;
  }
  $sqlquery = "SELECT fileDataID FROM FileData WHERE $sqlquery";
  if ($debug > 0) {
    print "Executing query: $sqlquery\n";
  }
  my $sth = $DBH->prepare($sqlquery);
  $sth->execute();
  my( $id );
  $sth->bind_columns( \$id );

  if ($sth->rows == 0) {
    my $newid;
    $newid = insert_file_data();
    return $newid;
  }
  if ($sth->rows > 1) {
    if ($debug > 0) {
      print "More than one file data matches the query criteria\nAdd more data to unambiguously identify file data\n";
    }
    return 0;
  }

  if ( $sth->fetch() ) {
    if ($debug > 0) {
      print "Returning: $id\n";
    }
    return $id;
  }
  return 0;
  

}

#============================================
# inserts the file location data and the file and run data
# if neccessary.
# Returns: The ID of a newly created File Location
# or 0 if the insert fails
sub insert_simulation_params {
  my $simComments;
  my $evgenComments;
  my $eventGenerator;

  if (! (defined $valuset{"generator"} && defined $valuset{"genversion"} && defined $valuset{"genparams"})) {
    if ($debug > 0) 
      {
	print "Not enough parameters to insert event generator data\nDefine generator, genversion and genparams\n";
      }
    return 0;
  }
  if (! defined $valuset{"gencomment"}) {
    if ($debug > 0) {
      print "WARNING: gencomment not defined. Using NULL\n";
    }
    $evgenComments = "NULL";
  } else {
    $evgenComments = '"'.$valuset{"gencomment"}.'"';
  }
  if (! defined $valuset{"simcomment"}) {
    if ($debug > 0) {
      print "WARNING: simcomment not defined. Using NULL\n";
    }
    $simComments = "NULL";
  } else {
    $simComments = '"'.$valuset{"simcomment"}.'"';
  }

  my $sqlquery = "SELECT eventGeneratorID FROM EventGenerators WHERE eventGeneratorName = '".$valuset{"generator"}."' AND eventGeneratorVersion = '".$valuset{"genversion"}."' AND eventGeneratorParams = '".$valuset{"genparams"}."'";
  if ($debug > 0) {
    print "Executing query: $sqlquery\n";
  }
  my $sth = $DBH->prepare($sqlquery);
  $sth->execute();
  my( $id );
  $sth->bind_columns( \$id );

  if ($sth->rows == 0) {
    my $eginsert   = "INSERT INTO EventGenerators ";
    $eginsert  .= "(eventGeneratorName, eventGeneratorVersion, eventGeneratorComment, eventGeneratorParams)";
    $eginsert  .= " VALUES ('".$valuset{"generator"}."', '".$valuset{"genversion"}."', $evgenComments, '".$valuset{"genparams"}."')";
    if ($debug > 0) {
      print "Execute $eginsert\n";
    }
    my $sth = $DBH->prepare( $eginsert );
      
    if ( $sth->execute() ) {
      $eventGenerator = get_last_id();
      if ($debug > 0) {
	print "Returning: $eventGenerator\n";
      }
    } else {
      if ($debug > 0) {
	print "Could not add event gerator data.\nAborting simulation data insertion query\n";
      }
    }
  } else {
    $sth->fetch;
    if ($debug > 0) {
      print "Got eventGenerator: $id";
    }
    $eventGenerator = $id;
  }

  my $spinsert   = "INSERT INTO SimulationParams ";
  $spinsert  .= "(eventGeneratorID, simulationParamComments)";
  $spinsert  .= " VALUES ($eventGenerator, $simComments)";
  if ($debug > 0) {
    print "Execute $spinsert\n";
  }
  $sth = $DBH->prepare( $spinsert );
  if ( $sth->execute() ) {
    my $retid = get_last_id();
    if ($debug > 0) {
      print "Returning: $retid\n";
    }
  } else {
    if ($debug > 0) {
      print "Could not add simulation data.\nAborting simulation data insertion query\n";
    }
    return 0;
  }

}

#============================================
# get the ID for the current simulation parameters, or create them
# if not found.
# Returns:
# the ID of a SimulationParams record
# or 0 if no such record and cannot create it
sub get_current_simulation_params {
  my $simComment;
  my $generator;
  my $generatorVer;
  my $generatorParams;
  my $generatorComment;
  my $sqlquery;

  if (! (defined $valuset{"generator"} && defined $valuset{"genversion"} && defined $valuset{"genparams"})) {
    if ($debug > 0) {
      print "Not enough parameters to find event generator data\nDefine generator, genversion and genparams\n";
    }
    return 0;
  }
  $sqlquery = "SELECT simulationParamsID FROM SimulationParams, EventGenerators WHERE eventGeneratorName = '".$valuset{"generator"}."' AND eventGeneratorVersion = '".$valuset{"genversion"}."' AND eventGeneratorParams = '".$valuset{"genparams"}."' AND SimulationParams.eventGeneratorID = EventGenerators.eventGeneratorID";
  if ($debug > 0) {
    print "Executing query: $sqlquery\n";
  }
  my $sth = $DBH->prepare($sqlquery);
  $sth->execute();
  my( $id );
  $sth->bind_columns( \$id );

  if ($sth->rows == 0) {
    my $newid;
    $newid = insert_simulation_params();
    return $newid;
  }

  if ( $sth->fetch() ) {
    if ($debug > 0) {
      print "Returning: $id\n";
    }
    return $id;
  }
  return 0;
  
}

#============================================
# inserts the file location data and the file and run data
# if neccessary.
# Returns: The ID of a newly created File Location
# or 0 if the insert fails
sub insert_file_location {
  my $fileData;
  my $storageType;
  my $storageSite;
  my $filePath;
  my $createTime;
  my $owner;
  my $protection;
  my $nodeName;
  my $availability;
  my $persistent;
  my $sanity;

  $fileData = get_current_file_data();
  if ($fileData == 0) {
    if ($debug > 0) {
      print "No file data available\nAborting file insertion query\n";
    }
    return 0;
  }
  $storageType = check_ID_for_params("storage");
  $storageSite = check_ID_for_params("site");
  if (($storageType == 0 ) || ($storageSite == 0)) {
    if ($debug > 0) {
      print "Aborting file location insertion query\n";
    }
    return 0;
  }
  if (! defined $valuset{"path"}) {
    if ($debug > 0) {
      print "ERROR: file path not defined. Cannot add file location\nAborting File Location a";
    }
    return 0;
  } else {
    $filePath = "'".$valuset{"path"}."'";
  }
  if (! defined $valuset{"createtime"}) {
    if ($debug > 0) {
      print "WARNING: createtime not defined. Using a default value\n";
    }
    $createTime = "NULL";
  } else {
    $createTime = $valuset{"createtime"};
  }
  if (! defined $valuset{"owner"}) {
    if ($debug > 0) {
      print "WARNING: owner not defined. Using a default value\n";
    }
    $owner = "NULL";
  } else {
    $owner = '"'.$valuset{"owner"}.'"';
  }
  if (! defined $valuset{"protection"}) {
    if ($debug > 0) {
      print "WARNING: protection not defined. Using a default value\n";
    }
    $protection = "NULL";
  } else {
    $protection = '"'.$valuset{"protection"}.'"';
  }
  if (! defined $valuset{"node"}) {
    if ($debug > 0) {
      print "WARNING:  not defined. Using a default value\n";
    }
    $nodeName = "NULL";
  } else {
    $nodeName = '"'.$valuset{"node"}.'"';
  }
  if (! defined $valuset{"availability"}) {
    if ($debug > 0) {
      print "WARNING:  not defined. Using a default value\n";
    }
    $availability = 1 ;
  } else {
    $availability = $valuset{"availability"};
  }
  if (! defined $valuset{"persistent"}) {
    if ($debug > 0) {
      print "WARNING:  not defined. Using a default value\n";
    }
    $persistent = 0 ;
  } else {
    $persistent = $valuset{"persistent"};
  }
  if (! defined $valuset{"sanity"}) {
    if ($debug > 0) {
      print "WARNING:  not defined. Using a default value\n";
    }
    $sanity = 0;
  } else {
    $persistent = $valuset{"sanity"};
  }


  my $flinsert   = "INSERT INTO FileLocations ";
  $flinsert  .= "(fileLocationID, fileDataID, storageTypeID, filePath, createTime, insertTime, owner, storageSiteID, protection, nodeName, availability, persistent, sanity)";
  $flinsert  .= " VALUES (NULL, $fileData, $storageType, $filePath, $createTime, NULL, $owner, $storageSite, $protection, $nodeName, $availability, '$persistent', $sanity)";
  if ($debug > 0) {
    print "Execute $flinsert\n";
  }
  my $sth = $DBH->prepare( $flinsert );
  if ( $sth->execute() ) {
    my $retid = get_last_id();
    if ($debug > 0) {
      print "Returning: $retid\n";
    }
    return $retid;
  }
  return 0;
  
  
}


#====================================================
# Helper subroutines needed for the generic query building
#====================================================
# Gets the 'level' of a database table
# The table is at level 1, if it not directly referenced by any other table
# The table is at level 2 if it is directly referenced by a table at level 1 etc.
sub get_struct_level {
  my $count;
  my ($paramtable) = @_;

  for ($count = 0; $count<($#datastruct+1); $count++) {
    my ($mtable, $stable, $cfield, $level) = split(",",$datastruct[$count]);
    if ($mtable eq $paramtable) {
      return $level;
    }
  }
  return 0;
}

#====================================================
# Get all the tables at the lower level connnected to this table
sub get_lower_level {
  my $count;
  my ($paramtable) = @_;

  for ($count = 0; $count<($#datastruct+1); $count++) {
    my ($mtable, $stable, $cfield, $level) = split(",",$datastruct[$count]);
    if (($mtable eq $paramtable) && ($stable ne "")) {
      return ($stable, $level-1, $count);
    }
  }
  return 0;
}

#===================================================
# Get all the tables at the upper level connected to this table
sub get_all_upper {
  my $count;
  my ($paramtable) = @_;
  my @lower;

  for ($count = 0; $count<($#datastruct+1); $count++) {
    my ($mtable, $stable, $cfield, $level) = split(",",$datastruct[$count]);
    if ($stable eq $paramtable) {
      push(@lower, $mtable);
    }
  }
  return (@lower);
}

#===================================================
# Return the connection description index from a @datastruct table
# describing the connection between two tables 
sub get_connection {
  my ($amtable, $astable) = (@_);
  
  for (my $count = 0; $count<($#datastruct+1); $count++) {
    my ($mtable, $stable, $cfield, $level) = split(",",$datastruct[$count]);
    if (($stable eq $astable) && ($mtable eq $amtable)) {
      return $count;
    }
  }
  return -1;
}

#==================================================
# check if thwo sets of numbers have any common part
# Params:
# number of elements in the first collection
# first collection (a list)
# second collection
sub get_intersect {
  my @params = @_;
  my ($count, $cf, $cs);
  my (@first, @second);

  for ($count = 1; $count<$params[0]+2; $count++) {
    $first[$count-1] = $params[$count];
  }
  for ($count=$params[0]+2; $count < $#params+1; $count++) {
    $second[$count-$params[0]-2] = $params[$count];
  }
  if ($debug > 0) {
    print "First set: ".join(" ", (@first))."\n";
    print "Second set: ".join(" ", (@second))."\n";
  }
  for ($cf=0; $cf<$#first+1; $cf++) {
    for ($cs=0; $cs<$#second+1; $cs++) {
      if ($first[$cf] eq $second[$cs]) {
	if ($debug > 0) {
	  print "Got intersect: $cf, $cs\n";
	}
	return ($cf, $cs);
      }
    }
  }
  return -1;
}

#============================================
# Get the DB table connection path from a field to field
# as a list of connection indexes from @datastruct table
# Params: 
# keywords for fields to connect
# Returns:
# list of the numbers of the connections
sub connect_fields {
  if ($_[0] =~ m/FileCatalog/) {
    shift @_;
  }
  ;
  my ($begkeyword, $endkeyword) = (@_);
  my ($begtable, $begfield, $endtable, $endfield, $blevel, $elevel);
  my ($ftable, $stable, $flevel, $slevel);
  my (@connections, $connum);
  if ($debug > 0) {
    print "Looking for connection between fields: $begkeyword, $endkeyword\n";
  }
  $begtable = get_table_name($begkeyword);
  $begfield = get_field_name($begkeyword);
  $endtable = get_table_name($endkeyword);
  $endfield = get_field_name($endkeyword);
  $blevel = get_struct_level($begtable);
  $elevel = get_struct_level($endtable);
  if ($blevel > $elevel) {
    ($ftable, $stable, $flevel, $slevel) =
      ($begtable, $endtable, $blevel, $elevel)
    } else {
      ($stable, $ftable, $slevel, $flevel) =
	($begtable, $endtable, $blevel, $elevel)
      }
  if ($debug > 0) {
    print "First: $ftable , $flevel\n";
    print "Second: $stable , $slevel\n";
  }
  # Get to the fields on the same level in tree hierarchy
  while ($slevel < $flevel) {
    my ($ttable, $tlevel, $connum) = get_lower_level($ftable);
    push(@connections, $connum);
    $flevel = $tlevel;
    $ftable = $ttable;
  }
  # If not the same table - look again
  if ( $stable ne $ftable) {
    if ($debug > 0) {
      print "Looking for downward connections\n";
    }
    my @upconnections;
    # look upward in table structure
    while (($stable ne $ftable) && ($slevel != 1)) {
      my ($ttable, $tlevel, $connum) = get_lower_level($stable);
      push(@upconnections, $connum);
      $slevel = $tlevel;
      $stable = $ttable;
      ($ttable, $tlevel, $connum) = get_lower_level($ftable);
      push(@upconnections, $connum);
      $flevel = $tlevel;
      $ftable = $ttable;
    }
    push (@connections, @upconnections);
    if ( $stable ne $ftable) {
      if ($debug > 0) {
	print "Looking for upward connections\n";
      }
      # Go up from the tree root searching for the link between tables
      my (%froads, %sroads);
      my (@flevelfields, @slevelfields);
      my ($findex, $sindex);

      @flevelfields = $ftable;
      @slevelfields = $stable;
      ($findex, $sindex) = get_intersect($#flevelfields, @flevelfields, @slevelfields);
      while ($findex == -1) {
	if ($debug > 0) {
	  print "First fields: ".join(" ",(@flevelfields))."\n";
	  print "Second fields: ".join(" ",(@slevelfields))."\n";
	}
	my ($fcount, $scount);
	my (@flower, @slower);
	for ($fcount=0; $fcount<$#flevelfields+1; $fcount++) {
	  # Get all the fields that are connected to this one
	  # and one level up
	  (@flower) = get_all_upper($flevelfields[$fcount]);
	  if ($debug > 0) {
	    print "All first descendants: ".join(" ",(@flower))."\n";
	  }
	  for (my $cflow = 0; $cflow < $#flower+1; $cflow++) {
	    # Add a road going from the tree root to this field
	    $froads{$flower[$cflow]} = $froads{$flevelfields[$fcount]}." ".get_connection($flower[$cflow], $flevelfields[$fcount]);
	    if ($debug > 0) {
	      print "Added road $froads{$flower[$cflow]}\n";
	    }
	  }
	}
	for ($scount=0; $scount<$#slevelfields+1; $scount++) {
	  # Get all the fields that are connected to this one
	  # and one level up
	  (@slower) = get_all_upper($slevelfields[$scount]);
	  if ($debug > 0) {
	    print "All second descendants: ".join(" ",(@slower))."\n";
	  }
	  for (my $cslow = 0; $cslow < $#slower+1; $cslow++) {
	    # Add a road going from the tree root to this field
	    $sroads{$slower[$cslow]} = $sroads{$slevelfields[$scount]}." ".get_connection($slower[$cslow], $slevelfields[$scount]);
	    if ($debug > 0) {
	      print "Added road $sroads{$slower[$cslow]}\n";
	    }
	  }
	}
	@flevelfields = @flower;
	@slevelfields = @slower;
	($findex, $sindex) = get_intersect($#flevelfields, @flevelfields, @slevelfields);
      }
      push (@connections, $froads{$flevelfields[$findex]});
      push (@connections, $sroads{$slevelfields[$sindex]});
    }
  }
  return join(" ",@connections);
}

#============================================
# Runs the query to get the data from the database
# Params: list of keyowrds determining the data
# fields to get:
# Returns:
# list of rows matching the query build on the current context
# in each row the fileds are separated by ::
sub run_query {
  if ($_[0] =~ m/FileCatalog/) {
    shift @_;
  };

  my %functions;
  my $count;
  my $grouping = "";

  my (@keywords)  = (@_);

  $count = 0;
  # Check the validity of the keywords
  foreach(@keywords)
    {
      #First check if it is a request for an agregate value
      my $afun;
      my ($aggr, $keyw);

      $_ =~ y/ //d;

      foreach $afun (@aggregates)
	{
	  ($aggr, $keyw) = $_ =~ m/($afun)\((.*)\)/;
	  last if (defined $aggr and defined $keyw);
	}
      if (defined $keyw)
	{ 
	  if ($debug > 0) 
	    { print "Found aggregate function |$aggr| on keyword |$keyw|\n"; }
	  # If it is - save the function, and store only the bare keyword
	  $functions{$keyw} = $aggr;
	  $keywords[$count] = $keyw;
	}
      if (not defined ($keywrds{$_}))
	{
	  print "Wrong keyword: $_\n";
	  return 0;
	}
      $count++;
    }
  
  my @connections;
  my @from;

  push (@from, get_table_name($keywords[0]));
  for ($count=1; $count<$#keywords+1; $count++) {      
    push (@connections, (connect_fields($keywords[0], $keywords[$count])));
    push (@from, get_table_name($keywords[$count]));
  }

  # Also add to the FROM tables the tables for each set keyword
  foreach my $key (keys %valuset)
    {
      if (get_table_name($key) ne "")
	{
	  push (@connections, (connect_fields($keywords[0], $key)));
	  push (@from, get_table_name($key));
	}
    }

  if ($debug > 0) {
    print "Connections to build the query: ".join(" ",@connections)."\n";
  }

  if (defined $valuset{"simulation"})
    {
      push (@connections, (connect_fields($keywords[0], "runnumber")));
      push (@from, "RunParams");
    }

  # Fill the table of connections
  my $connections = join(" ",(@connections));
  my @toquery;
  foreach (sort (split(" ",$connections))) {
    if ((not $toquery[$#toquery]) || ($toquery[$#toquery] != $_)) {
      push (@toquery, $_);
    }
  }
  
  if ($debug > 0) {
    print "Connections to build the query: ".join(" ",@toquery)."\n";
  }
  # Get the select fields
  my @select;
  foreach (@keywords) {
    if ($debug > 0) {
      print "Adding keyword: $_<br>\n";
    }
    if (defined $functions{$_})
      {
	if ($functions{$_} eq "grp")
	  {
	    $grouping .= " GROUP BY ".get_field_name($_)." ";
	    push (@select, get_field_name($_));
	  }
	elsif ($functions{$_} eq "orda")
	  {
	    $grouping .= " ORDER BY ".get_field_name($_)." ASC ";
	    push (@select, get_field_name($_));
	  }
	elsif ($functions{$_} eq "ordd")
	  {
	    $grouping .= " ORDER BY ".get_field_name($_)." DESC ";
	    push (@select, get_field_name($_));
	  }
	else
	  {
	    push (@select, $functions{$_}."(".get_field_name($_).")");
	  }
      }
    elsif ($_ ne "collision") {
      push (@select, get_field_name($_));
    } 
    else
    {
      push (@select, "CONCAT( firstParticle, secondParticle, collisionEnergy )");
    }
  }

  # Build the FROM and WHERE parts of the query
  # using thew connection list
  my  ($where);
  foreach (@toquery) {
    my ($mtable, $stable, $field, $level) = split(",",$datastruct[$_]);
    push (@from, $mtable);
    push (@from, $stable);
    if (not $where) {
      $where .= " $mtable.$field = $stable.$field ";
    } else {
      $where .= " AND $mtable.$field = $stable.$field ";
    }
  }
  my $toquery = join(" ",(@from));
  if ($debug > 0) {
    print "Table list $toquery\n";
  }
  # Get only the unique table names
  my @fromunique;
  foreach (sort {$a cmp $b} split(" ",$toquery)) {
    if ($debug > 0) {
      print "Adding $_\n";
    }
    if ((not $fromunique[$#fromunique]) || ($fromunique[$#fromunique] ne $_)) {
      push (@fromunique, $_);
    }
  }

  # See if we have any constaint parameters
  my @constraint;
  foreach (keys(%valuset)) {
    my $fromlist = join(" " , (@fromunique));
    my $tabname = get_table_name($_);
    if ((($fromlist =~ m/$tabname/) > 0) && ($tabname ne "")) {
      my $fieldname = get_field_name($_);
      if ((($roundfields =~ m/$fieldname/) > 0) && (! defined $valuset{"noround"})) 
	{
	  my ($nround) = $roundfields =~ m/$fieldname|([0-9]*)/;
	  push( @constraint, "ROUND($fieldname, $nround) ".$operset{$_}." ".$valuset{$_} );
	} 
      elsif ($operset{$_} eq "~") 
	{
	  push( @constraint, "$fieldname LIKE '%".$valuset{$_}."%'" );	  
	}
      elsif ($operset{$_} eq "!~") 
	{
	  push( @constraint, "$fieldname NOT LIKE '%".$valuset{$_}."%'" );	  
	}
      else
	{
	  if (get_field_type($_) eq "text")
	    { push( @constraint, "$fieldname ".$operset{$_}." '".$valuset{$_}."'" ); }
	  else
	    { push( @constraint, "$fieldname ".$operset{$_}." ".$valuset{$_} ); }
	}
    }
  }

  if (defined $valuset{"simulation"})
    {
      if ($valuset{"simulation"} eq "1")
	{
	  push ( @constraint, "RunParams.simulationParamsID IS NOT NULL");
	}
      else
	{
	  push ( @constraint, "RunParams.simulationParamsID IS NULL");
	}
    }

  # Check to see if we are getting info from the FileLocations table
  # if so, and "all" keyword is not set - get only the records
  # with non-zero availability
  my $floc = join(" ",(@fromunique)) =~ m/FileLocations/;

  if ($debug > 0)
    { print "Checking for FileLocations ".(join(" ",@fromunique))." $floc\n"; }
  if (($floc > 0) && defined ($valuset{"all"}) && ($valuset{"all"} == 0 ))
    {
      push ( @constraint, "FileLocations.availability > 0");
    }

  my $constraint = join(" AND ",@constraint);

  # Build the actual query string
  my $sqlquery;
  $sqlquery = "SELECT ";
  if (! defined $valuset{"nounique"})
    { $sqlquery .= " DISTINCT "; }
  $sqlquery .= join(" , ",(@select))." FROM ".join(" , ",(@fromunique));
  if (defined $where) { 
    $sqlquery .=" WHERE $where"; 
    if ($constraint ne "") {
      $sqlquery .= " AND $constraint";
    }
  }
  elsif ($constraint ne "") {
    $sqlquery .= " WHERE $constraint";
  }

  $sqlquery .= $grouping;

  my ($offset, $limit);

  if (defined $valuset{"startrecord"})
    {
      $offset = $valuset{"startrecord"};
    }
  else
    { $offset = 0 };
  if (defined $valuset{"limit"})
    {
      $limit = $valuset{"limit"};
    }
  else
    { $limit = 100 };

  $sqlquery .= " LIMIT $offset, $limit";

  if ($debug > 0) {
    print "Using query: $sqlquery\n";
  }
  my $sth = $DBH->prepare($sqlquery);
  $sth->execute();
  my (@result);
  my (@cols);
  my $rescount = 0;

  while ( @cols = $sth->fetchrow_array() ) {
    $result[$rescount++] = join($delimeter, (@cols));
  }
  return (@result);
}

#============================================
# deletes the record that matches the current
# context. First it deletes it from the file
# locations. If this makes the current file
# data have no location, it deletes the file 
# data too.
# Returns:
# 1 if delete was successfull
# 0 if delete failed
sub delete_record {
  # first delete the file location
  my @deletes;

  foreach my $key (keys %keywrds) 
    {
      my $field = get_field_name($key);
      my $table = get_table_name($key);
      
      if ((is_critical($key) == 1) && ($table eq "FileLocations"))
	{
	  if (defined $valuset{$key})
	    {
	      if (get_field_type($key) eq "text")
		{ push (@deletes, "$field = '".$valuset{$key}."'"); }
	      else
		{ push (@deletes, "$field = ".$valuset{$key}); }
	    }
	  else
	    {
	      if ($debug > 0) { print "ERROR: Cannot delete record.\n".$key." not defined\n"; }
	      return 0;
	    }
	}
      if ((is_critical($key) == 0) && ($table eq "FileLocations"))
	{ 
	  if (defined($valuset{$key}))
	    {
	      if (get_field_type($key) eq "text")
		{ push (@deletes, "$field = '".$valuset{$key}."'"); }
	      else
		{ push (@deletes, "$field = ".$valuset{$key}); }
	    }
	}
    }
  my $storage = check_ID_for_params("storage");
  if ($storage == 0)
    {
      if ($debug > 0) { print "ERROR: Cannot delete record.\nstorage not defined\n"; }
      return 0;
    }
  else { push (@deletes, "storageTypeID = $storage"); }
  my $site = check_ID_for_params("site");
  if ($site == 0)
    {
      if ($debug > 0) { print "ERROR: Cannot delete record.\nsite not defined\n"; }
      return 0;
    }
      else { push (@deletes, "storageSiteID = $site"); }
  my $fdata = get_current_file_data();
  if ($fdata == 0)
    {
      if ($debug > 0) { print "ERROR: Cannot delete record.\nfiledata not defined\n"; }
      return 0;
    }
  else { push (@deletes, "fileDataID = $fdata"); }
  
  my $wheredelete = join(" AND ", (@deletes));
  
  my $fldelete;
  $fldelete = "DELETE FROM FileLocations WHERE $wheredelete";
  if ($debug > 0)
    {
      print "Executing delete: $fldelete\n";
    }

  my $sth = $DBH->prepare( $fldelete );
  if ( $sth->execute() )
    { 
      # Checking if the given file data has any file locations attached to it
      my $fdquery;
      $fdquery = "SELECT fileLocationID from FileLocations, FileData WHERE FileLocations.fileDataID = FileData.fileDataID AND FileData.fileDataID = $fdata";
      my $stq = $DBH->prepare( $fdquery );
      $stq->execute();
      if ($stq->rows == 0)
	{
	  # This file data has no file locations - delete it (and its trigger compositions)
	  my $fddelete;
	  $fddelete = "DELETE FROM FileData WHERE fileDataID = $fdata";
	  if ($debug > 0) { print "Executing $fddelete\n"; }
	  my $stfdd = $DBH->prepare($fddelete);
	  $stfdd->execute();
	  my $tcdelete = "DELETE FROM TriggerCompositions WHERE fileDataID = $fdata";
	  if ($debug > 0) { print "Executing $tcdelete\n"; }
	  my $sttcd = $DBH->prepare($tcdelete);
	  $sttcd->execute();
	}
      return 1; }
  else
    { return 0; }
}

#============================================
# Bootstraps a table - meaning it checks if all
# the records in this table are connected to some
# child table
# Prams:
# keyowrd - keword for which to check the table
# dodelete - set to 1 to automaticaly delete the offending records
# Returns
# List of records that are not connected
# or 0 if there were errors or no unconnected records
sub bootstrap {
  if ($_[0] =~ m/FileCatalog/) {
    shift @_;
  }
  my ($keyword, $delete) = (@_);
  my $table = get_table_name($keyword);
  if ($table eq "")
    { return 0; }

  my ( $childtable, $linkfield );
  # Check if this really is a dictionary table
  my $refcount = 0;
  foreach (@datastruct)
    {
      my ($mtable, $ctable, $lfield) = split(",");
      if ($ctable eq $table)
	# This table is referencing another one - it is not a dictianry!
	{ 
	  if ( $debug > 0 ) { print "$table is not a dictionary table!\nConnot do bootstrapping\n"; }
	  return 0; 
	}
      if ($mtable eq $table)
	{ 
	  $childtable = $ctable;
	  $linkfield = $lfield;
	  $refcount++; 
	}
    }
  if ($refcount != 1)
    # This table is not refernced by any other table or referenced
    # by more than one - it is not a proper dictionary
    {
      if ( $debug > 0 ) { print "$table is not a dictionary table!\nConnot do bootstrapping\n"; }
      return 0;
    }
  my $dcquery;
  $dcquery = "select $table.$linkfield FROM $table LEFT OUTER JOIN $childtable ON $table.$linkfield = $childtable.$linkfield WHERE $childtable.runTypeID IS NULL";
  my $stq = $DBH->prepare( $dcquery );
  $stq->execute();
  if ($stq->rows > 0)
    {
      my @rows;
      my( $id );
      $stq->bind_columns( \$id );
      
      while ( $stq->fetch() ) {
	push ( @rows, $id );
      }
      if ($delete == 1)
	{
	  # We do a bootstapping with delete
	  my $dcdelete;
	  $dcdelete = "DELETE FROM $table WHERE $linkfield IN (".join(" , ",(@rows)).")";
	  if ($debug > 0) { print "Executing $dcdelete\n"; }
	  my $stfdd = $DBH->prepare($dcdelete);
	  $stfdd->execute();
	}
      return (@rows);
    }
  return 0;
}

#============================================
# Updates the field coresponding to a given keyowrd
# with a new value, replaces the value in the current
# context.The value of the keyword to be modified,
# MUST appear in a previous set_context() statement.
# This is a limitation which has been chosen in
# order to also treat changing values in dictionaries.
#
# Params:
# keyword - the keyword which data is to be updated
# value - new value that should be put into the database
#         instead of the current one
# Returns:
# 1 if update was successfull
# 0 if delete failed
sub update_record {
  if ($_[0] =~ m/FileCatalog/) {
    my $self = shift;
  }
  my @updates;
  
  my ($ukeyword, $newvalue) = (@_);
  
  my $utable = get_table_name($ukeyword);
  my $ufield = get_field_name($ukeyword);
  
  foreach my $key (keys %keywrds) 
    {
      my $field = get_field_name($key);
      my $table = get_table_name($key);
      
      # grab keywords which belongs to the same table 
      # This will be used as the selection WHERE clause.
      # The ufield is excluded because we use it by default
      # in the SET xxx= WHERE xxx= as an extra MANDATORY
      # clause.
      if (($table eq $utable) && ($field ne $ufield))
	{ 
	  if (defined($valuset{$key}))
	    {
	      if (get_field_type($key) eq "text")
		{ push (@updates, "$field = '".$valuset{$key}."'"); }
	      else
		{ push (@updates, "$field = ".$valuset{$key}); }
	    }
	}
    }
  my $whereclause = join(" AND ",(@updates));
  
  if ($utable eq "")
    {
      if ($debug > 0) { print "ERROR: $ukeyword does not have an associated table\nCannot update\n"; }
      return 0;
    }			
  
  
  my $qupdate;
  if (get_field_type($ukeyword) eq "text")
    { $qupdate = "UPDATE $utable SET $ufield = '$newvalue' WHERE $ufield = '".$valuset{$ukeyword}."'"; }
  else
    { $qupdate = "UPDATE $utable SET $ufield = $newvalue WHERE $ufield = ".$valuset{$ukeyword}; }
  if ($whereclause ne "")
    { $qupdate .= " AND $whereclause"; }
  if ($debug > 0)
    {
      print "Executing update: $qupdate\n";
    }

  my $sth = $DBH->prepare( $qupdate );
  if ( $sth->execute() )
    { return 1; }
  else
    { return 0; }
}

#============================================
# Returns the context value assigned to the current field
# Params:
# keyowrd - which context keyword to get
# Return:
# context value for keyword
sub get_context {
  if ($_[0] =~ m/FileCatalog/) {
    shift @_;
  }

  my ($param) = @_;
  return $valuset{$param};
}

#============================================
sub set_field {
}

#============================================
sub debug_on {
  $debug = 1;
}

#============================================
sub debug_off {
  $debug = 0;
}

#============================================
sub destroy {
  my $self = shift;
  clear_context();
  if ( defined($DBH) ) {
    if ( $DBH->disconnect ) {
      return 1;
    } else {
      return 0;
    }
  }
}

#============================================
1;

