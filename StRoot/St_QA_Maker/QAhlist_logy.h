// $Id: QAhlist_logy.h,v 2.13 2015/01/16 21:08:28 genevb Exp $
// $Log: QAhlist_logy.h,v $
// Revision 2.13  2015/01/16 21:08:28  genevb
// Initial versions of HFT histograms
//
// Revision 2.12  2004/01/10 01:10:17  genevb
// Preparations for Year 5, added some svt plots
//
// Revision 2.11  2003/01/24 15:02:41  genevb
// Updates for FTPC histos
//
// Revision 2.10  2002/07/29 18:54:43  genevb
// Some FTPC updates
//
// Revision 2.9  2002/04/23 01:59:55  genevb
// Addition of BBC/FPD histos
//
// Revision 2.8  2002/04/03 21:13:11  lansdell
// primary track first, last point residuals now use outerGeometry() for helix parameters
//
// Revision 2.7  2002/02/12 18:41:59  genevb
// Additional FTPC histograms
//
// Revision 2.6  2002/01/21 22:09:24  genevb
// Include some ftpc histograms from StFtpcClusterMaker
//
// Revision 2.5  2001/08/29 20:45:15  genevb
// Trigger word histos
//
// Revision 2.4  2001/05/23 00:14:52  lansdell
// more changes for qa_shift histograms
//
// Revision 2.3  2001/05/16 20:57:03  lansdell
// new histograms added for qa_shift printlist; some histogram ranges changed; StMcEvent now used in StEventQA
//
// Revision 2.2  2001/04/24 22:53:51  lansdell
// Removed redundant radial position of first hit histograms
//
// Revision 2.1  2000/08/25 16:04:10  genevb
// Introduction of files
//
//
///////////////////////////////////////////////////////////////////////
// Names of histograms to be plotted with log y
///////////////////////////////////////////////////////////////////////
// Note: Editing this file means that StAnalysisUtilities/StHistUtil
// must be recompiled

 "QaInnerSectorDeDx",
 "QaOuterSectorDeDx",
 "QaDedxAllSectors",
 "QaTrigWord",
 "QaTrigBits",
 "QaGtrkFitProb",
 "QaGtrkDetId",
 "QaGtrkFlag",
 "QaGtrkf0",
 "QaGtrkf0TS",
 "QaGtrkXf0",
 "QaGtrkXf0TS",
 "QaGtrkYf0",
 "QaGtrkYf0TS",
 "QaGtrkZf0",
 "QaGtrkZf0TS",
 "QaGtrkRZf0",
 "QaGtrkRZf0TS",
 "QaGtrkRZl0",
 "QaGtrkRZl0TS",
 "QaGtrkImpactT",  
 "QaGtrkImpactTS",
 "QaGtrkImpactTTS",
 "QaGtrkImpactF",
 "QaGtrkNPntT",
 "QaGtrkNPntTS",
 "QaGtrkNPntMaxT",
 "QaGtrkNPntMaxTS",
 "QaGtrkNPntFitT",
 "QaGtrkNPntFitTS",
 "QaGtrkPtT",
 "QaGtrkPtTS",
 "QaGtrkPtTTS",
 "QaGtrkPtF",
 "QaGtrkPtFE",
 "QaGtrkPtFW",
 "QaGtrkPT",
 "QaGtrkPTS",
 "QaGtrkPTTS",
 "QaGtrkPF",
 "QaGtrkPFE",
 "QaGtrkPFW",
 "QaGtrkR0T",
 "QaGtrkR0TS",
 "QaGtrkZ0T",
 "QaGtrkZ0TS",
 "QaGtrkCurvT",
 "QaGtrkCurvTS",
 "QaGtrkPadfT",
 "QaGtrkPadfTEW",
 "QaGtrkXfT",
 "QaGtrkXfTS",
 "QaGtrkXfF",
 "QaGtrkXfFE",
 "QaGtrkXfFW",
 "QaGtrkYfT",
 "QaGtrkYfTS",
 "QaGtrkYfF",
 "QaGtrkYfFE",
 "QaGtrkYfFW",
 "QaGtrkZfT",
 "QaGtrkZfTS",
 "QaGtrkZfF",
 "QaGtrkZfFE",
 "QaGtrkZfFW",
 "QaGtrkRT",
 "QaGtrkRTS",
 "QaGtrkRF",
 "QaGtrkRFE",
 "QaGtrkRFW",
 "QaGtrkRnfT",
 "QaGtrkRnfTS",
 "QaGtrkRnfTTS",
 "QaGtrkRnmT",
 "QaGtrkRnmTS",
 "QaGtrkRnmTTS",
 "QaGtrkTanlT",
 "QaGtrkTanlTS",
 "QaGtrkTanlFE",
 "QaGtrkTanlFW",
 "QaGtrkThetaT",
 "QaGtrkThetaTS",
 "QaGtrkThetaFE",
 "QaGtrkThetaFW",
 "QaGtrkEtaT",
 "QaGtrkEtaTS",
 "QaGtrkEtaTTS",
 "QaGtrkEtaF",
 "QaGtrkEtaFE",
 "QaGtrkEtaFW",
 "QaGtrkLengthT",
 "QaGtrkLengthTS",
 "QaGtrkLengthF",
 "QaGtrkLengthFE",
 "QaGtrkLengthFW",
 "QaGtrkChisq0T",
 "QaGtrkChisq0TS",
 "QaGtrkChisq0TTS",
 "QaGtrkChisq1T",
 "QaGtrkChisq1TS",
 "QaGtrkChisq1TTS",
 "QaGtrkImpactrT",
 "QaGtrkImpactrTS",
 "QaGtrkImpactrTTS",
 "QaGtrkSptsTS",

 "QaPtrkDetId",
 "QaPtrkFlag",
 "QaPtrkf0",
 "QaPtrkf0TS",
 "QaPtrkXf0",
 "QaPtrkXf0TS",
 "QaPtrkYf0",
 "QaPtrkYf0TS",
 "QaPtrkZf0",
 "QaPtrkZf0TS",
 "QaPtrkRZf0",
 "QaPtrkRZf0TS",
 "QaPtrkImpactT",
 "QaPtrkImpactTS",
 "QaPtrkImpactTTS",
 "QaPtrkImpactF",
 "QaPtrkNPntT",
 "QaPtrkNPntTS",
 "QaPtrkNPntMaxT",
 "QaPtrkNPntMaxTS",
 "QaPtrkNPntFitT",
 "QaPtrkNPntFitTS",
 "QaGtrkNPntFitTTS",
 "QaPtrkPtT",
 "QaPtrkPtTS",
 "QaPtrkPtTTS",
 "QaPtrkPtF",
 "QaPtrkPtFE",
 "QaPtrkPtFW",
 "QaPtrkPT",
 "QaPtrkPTS",
 "QaPtrkPTTS",
 "QaPtrkPTF",
 "QaPtrkPTFE",
 "QaPtrkPTFW",
 "QaPtrkR0T",
 "QaPtrkR0TS",
 "QaPtrkZ0T",
 "QaPtrkZ0TS",
 "QaPtrkRZl0",
 "QaPtrkRZl0TS",
 "QaPtrkCurvT",
 "QaPtrkCurvTS",
 "QaPtrkXfT",
 "QaPtrkXfTS",
 "QaPtrkXfF",
 "QaPtrkXfFE",
 "QaPtrkXfFW",
 "QaPtrkYfT",
 "QaPtrkYfTS",
 "QaPtrkYfF",
 "QaPtrkYfFE",
 "QaPtrkYfFW",
 "QaPtrkZfT",
 "QaPtrkZfTS",
 "QaPtrkZfF",
 "QaPtrkZfFE",
 "QaPtrkZfFW",
 "QaPtrkRT",
 "QaPtrkRTS",
 "QaPtrkRF",
 "QaPtrkRFE",
 "QaPtrkRFW",
 "QaPtrkRnfT",
 "QaPtrkRnfTS",
 "QaGtrkRnmfTTS",
 "QaPtrkRnmT",
 "QaPtrkRnmTS",
 "QaPtrkTanlT",
 "QaPtrkTanlTS",
 "QaPtrkThetaT",
 "QaPtrkThetaTS",
 "QaPtrkEtaT",
 "QaPtrkEtaTS",
 "QaPtrkEtaTTS",
 "QaPtrkEtaF",
 "QaPtrkEtaFE",
 "QaPtrkEtaFW",
 "QaPtrkLengthT",
 "QaPtrkLengthTS",
 "QaPtrkLengthF",
 "QaPtrkLengthFE",
 "QaPtrkLengthFW",
 "QaPtrkChisq0T",
 "QaPtrkChisq0TS",
 "QaPtrkChisq0TTS",
 "QaPtrkChisq1T",
 "QaPtrkChisq1TS",
 "QaPtrkChisq1TTS",
 "QaPtrkImpactrT",
 "QaPtrkImpactrTS",
 "QaPtrkImpactrTTS",
 "QaPtrkImpactrF",

/* These are FTPC histograms. Currently, the FTPC doesn't do primary tracking...
 "QaPtrkNPntFE",
 "QaPtrkNPntFW",
 "QaPtrkNPntMaxFE",
 "QaPtrkNPntMaxFW",
 "QaPtrkNPntFitFE",
 "QaPtrkNPntFitFW",
 "QaPtrkPtFE",
 "QaPtrkPtFW",
 "QaPtrkPFE",
 "QaPtrkPFW",
 "QaPtrkXfFE",
 "QaPtrkXfFW",
 "QaPtrkYfFE",
 "QaPtrkYfFW",
 "QaPtrkZfFE",
 "QaPtrkZfFW",
 "QaPtrkRFE",
 "QaPtrkRFW",
 "QaPtrkRnfFE",
 "QaPtrkRnfFW",
 "QaPtrkRnmFE",
 "QaPtrkRnmFW",
 "QaPtrkTanlFE",
 "QaPtrkTanlFW",
 "QaPtrkThetaFE",
 "QaPtrkThetaFW",
 "QaPtrkEtaFE",
 "QaPtrkEtaFW",
 "QaPtrkLengthFE",
 "QaPtrkLengthFW",
*/
 "QaDedxNum",
 "QaDedxDedx0T", 
 "QaDedxDedx1T",
 "QaPointId",
 "QaEvgenPt",
 "QaEvgenVtxX",
 "QaEvgenVtxY",
 "QaEvgenVtxZ",
 "QaVtxX",
 "QaVtxY",
 "QaVtxZ",
 "QaVtxChisq",
 "QaV0VtxZDist",
 "QaV0VtxRDist",

 "bemcAdc",
 "bsmdeAdc",
 "bsmdpAdc",
 "bemcClNum",
 "bemcClEnergy",
 "EmcCat4_Point_Energy",

 "fcl_chargestepW",
 "fcl_chargestepE",

 "QaBbcAdcES",
 "QaBbcAdcEL",
 "QaBbcAdcWS",
 "QaBbcAdcWL",
 "QaBbcTdcES",
 "QaBbcTdcEL",
 "QaBbcTdcWS",
 "QaBbcTdcWL",
 "QaFpdTop0",
 "QaFpdTop1",
 "QaFpdBottom0",
 "QaFpdBottom1",
 "QaFpdSouth0",
 "QaFpdSouth1",
 "QaFpdNorth0",
 "QaFpdNorth1",
 "QaFpdSums0",
 "QaFpdSums1",
 "QaFpdSums2",
 "QaFpdSums3",
 "QaFpdSums4",
 "QaFpdSums5",
 "QaFpdSums6",
 "QaFpdSums7",

 "QaPointSizeSSD"

