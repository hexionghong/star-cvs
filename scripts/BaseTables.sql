#
# $Id: BaseTables.sql,v 1.3 2001/02/16 22:11:22 porter Exp $
#
# Author: R. Jeff Porter
#***************************************************************************
#
# Description:  SQL for installing base set of tables for STAR-API
#               These tables exist in each database
#
#****************************************************************************
# 
# $Log: BaseTables.sql,v $
# Revision 1.3  2001/02/16 22:11:22  porter
# modified for new low-level table structures
#
# Revision 1.2  2000/05/03 19:00:09  porter
# fixed header file output option
#
# Revision 1.1  2000/04/28 14:09:03  porter
# Base tables definition is SQL form
#
#
############################################################
############################################################
#
#  schema & schema-evolution tables:  'schema' & 'structure'
#
############################################################

#
# Table structure for table 'schema'
#
DROP TABLE IF EXISTS schema;
CREATE TABLE schema (
  name varchar(80) DEFAULT '' NOT NULL,
  type varchar(18) DEFAULT '' NOT NULL,
  storeType enum('ascii','bin') DEFAULT 'ascii' NOT NULL,
  length varchar(18) DEFAULT '1',
  units varchar(64) DEFAULT '',
  resolution double(20,10),
  schemaID int(11) DEFAULT '1' NOT NULL,
  ID int(11) NOT NULL auto_increment,
  structName varchar(80) DEFAULT '' NOT NULL,
  structID smallint(6) DEFAULT '0' NOT NULL,
  position smallint(6) DEFAULT '0' NOT NULL,
  Linuxoffset smallint(6) DEFAULT '0' NOT NULL,
  entryTime timestamp(14),
  Comment varchar(255),
  KEY ID (ID),
  PRIMARY KEY (name,ID)
);

#
# Table structure for table 'structure'
#
DROP TABLE IF EXISTS structure;
CREATE TABLE structure (
  name varchar(80) DEFAULT '' NOT NULL,
  lastSchemaID int(11) DEFAULT '1' NOT NULL,
  ID smallint(6) NOT NULL auto_increment,
  entryTime timestamp(14),
  Comment varchar(255),
  KEY ID (ID),
  PRIMARY KEY (name,ID)
);


#############################################################
#
#  Named reference table:  'Nodes' 
#  Hierarchy of named objects: 'NodeRelation'  
#
#############################################################

#
# Table structre for table 'Nodes'
#
DROP TABLE IF EXISTS Nodes;
CREATE TABLE Nodes (
  name varchar(80) DEFAULT '' NOT NULL,
  versionKey varchar(128) DEFAULT 'default' NOT NULL,
  nodeType enum('DB','directory','table','Config') DEFAULT 'directory' NOT NULL,
  structName varchar(80),
  elementID varchar(255) DEFAULT 'None',
  indexName varchar(64) DEFAULT 'None',
  indexVal int(11) DEFAULT '0',
  baseLine enum('Y','N') DEFAULT 'N' NOT NULL,
  isBinary enum('Y','N') DEFAULT 'N' NOT NULL,
  isIndexed enum('Y','N') DEFAULT 'Y' NOT NULL,
  ID int(11) NOT NULL auto_increment,
  entryTime timestamp(14),
  Comment varchar(255),
  KEY ID (ID),
  PRIMARY KEY (name,versionKey)
);

#
# Table structre for table 'NodeRelation'
#
DROP TABLE IF EXISTS NodeRelation;
CREATE TABLE NodeRelation (
  ID int(11) NOT NULL auto_increment,
  ParentID int(11) DEFAULT '0' NOT NULL,
  NodeID int(11) DEFAULT '0' NOT NULL,
  BranchID int(11) DEFAULT '0' NOT NULL,
  ConfigID int(11) DEFAULT '0' NOT NULL,
  entryTime timestamp(14),
  KEY ID (ID),
  PRIMARY KEY (ParentID,NodeID,ConfigID,BranchID)
);

#
# Table structure for table 'dbCollection'
#
DROP TABLE IF EXISTS dbCollection;
CREATE TABLE dbCollection (
  dataID int(11) DEFAULT '0' NOT NULL auto_increment,
  entryTime timestamp(14),
  name varchar(64) NOT NULL,
  KEY dataID (dataID),
  KEY name (name)
);


############################################################
#
#  index & data tables:  'dataIndex' & 'bytes' (for pure binary)
#                        general data tables are created at
#                        time c-struct is loaded into db.
#
#  - dataIndex for backwards compatibility
#
############################################################

#
# Table structure for table 'dataIndex'
#
DROP TABLE IF EXISTS dataIndex;
CREATE TABLE dataIndex (
  count int(11) NOT NULL auto_increment,
  nodeID smallint(6) DEFAULT '0' NOT NULL,
  entryTime timestamp(14),
  schemaID int(11) DEFAULT '1' NOT NULL,
  beginTime datetime DEFAULT '1969-12-31 19:00:00' NOT NULL,
  elementID smallint(6) DEFAULT '0' NOT NULL,
  numRows smallint(6) DEFAULT '1' NOT NULL,
  dataID int(11) DEFAULT '0' NOT NULL,
  flavor char(8) DEFAULT 'ofl' NOT NULL,
  deactive int(10) unsigned DEFAULT '0' NOT NULL,
  version char(32) DEFAULT 'default' NOT NULL,
  KEY count (count),
  KEY dataID (dataID),
  PRIMARY KEY (nodeID,beginTime,flavor,elementID,deactive,version)
);

#
# Table structre for table 'bytes'
#
DROP TABLE IF EXISTS bytes;
CREATE TABLE bytes (
  dataID  int(11) DEFAULT '0' NOT NULL auto_increment,
  entryTime timestamp(14),
  bytes longblob,
  Primary Key (dataID)
);









