#ifndef StDetectorDefinitions_hh
#define StDetectorDefinitions_hh

/* Numbering scheme for detectors
** TPC             = 1
** SVT             = 2
** RICH            = 3
** FTPC west       = 4
** FTPC east       = 5
** TOF             = 6
** CTB             = 7
** SSD             = 8
** barrel EMC tower= 9
** barrel EMC pre-shower = 10
** barrel SMD eta strip  = 11
** barrel SMD phi strip  = 12
** endcap EMC tower      = 13
** endcap EMC pre-shower = 14
** endcap SMD eta strip  = 15
** endcap SMD phi strip  = 16
** Zero Degree Calo west = 17
** Zero Degree Calo east = 18
** MWPC west       = 19
** MWPC east       = 20
** TPC+SSD         = 21
** TPC+SVT         = 22
** TPC+SSD+SVT     = 23
** SSD+SVT         = 24
** CPV(PMD)        = 25
** PMD             = 26    */




#define kUnknownIdentifier             0
#define kTpcIdentifier                 1
#define kSvtIdentifier                 2
#define kRichIdentifier                3
#define kFtpcWestIdentifier            4
#define kFtpcEastIdentifier            5
#define kTofIdentifier                 6
#define kCtbIdentifier                 7
#define kSsdIdentifier                 8
#define kBarrelEmcTowerIdentifier      9
#define kBarrelEmcPreShowerIdentifier 10
#define kBarrelSmdEtaStripIdentifier  11
#define kBarrelSmdPhiStripIdentifier  12
#define kEndcapEmcTowerIdentifier     13
#define kEndcapEmcPreShowerIdentifier 14
#define kEndcapSmdUStripIdentifier    15
#define kEndcapSmdVStripIdentifier    16
#define kZdcWestIdentifier            17
#define kZdcEastIdentifier            18
#define kMwpcWestIdentifier           19
#define kMwpcEastIdentifier           20
#define kTpcSsdIdentifier             21
#define kTpcSvtIdentifier             22
#define kTpcSsdSvtIdentifier          23
#define kSsdSvtIdentifier             24
#define kPhmdCpvIdentifier            25
#define kPhmdIdentifier               26


/*
**  The following are for the inner and forward
**  tracking upgrades. tu (Oct 11, 2007)
*/
#define kPxlIdentifier                27
#define kIstIdentifier                28
#define kFgtIdentifier                29

 
/*
**  The following are more or less virtual detectors.
**  Depending on funding or policy this stuff might
**  happen or not. tu
*/
/*
#define kHftIdentifier                27
#define kIstIdentifier                28
#define kIgtIdentifier                29
#define kFstIdentifier                30
#define kFgtIdentifier                31
#define kHpdIdentifier                32
*/

#endif /*STDETECTORDEFINITIONS*/

/* $Id: StDetectorDefinitions.h,v 2.6 2007/10/11 21:50:19 ullrich Exp $
**
** $Log: StDetectorDefinitions.h,v $
** Revision 2.6  2007/10/11 21:50:19  ullrich
** Added new enums for PXL and IST detectors.
**
** Revision 2.5  2006/08/15 14:34:02  ullrich
** Added kHpdIdentifier.
**
** Revision 2.4  2006/01/20 15:11:59  jeromel
** ... meant needs to be C style, not C++
**
** Revision 2.3  2006/01/20 15:11:26  jeromel
** Comments need to be FORtran style
**
** Revision 2.2  2006/01/19 21:51:26  ullrich
** Added new RnD detectors.
**
** Revision 2.1  2004/04/26 16:35:19  fisyak
** Move enumerations from pams/global/inc => StEvent
**
** Revision 1.10  2002/12/19 21:52:38  lbarnby
** Corrected CVS tags
**
*/
