// $Id: QAhlist_EventQA_qa_shift.h,v 2.3 2001/05/16 20:57:02 lansdell Exp $
// $Log: QAhlist_EventQA_qa_shift.h,v $
// Revision 2.3  2001/05/16 20:57:02  lansdell
// new histograms added for qa_shift printlist; some histogram ranges changed; StMcEvent now used in StEventQA
//
// Revision 2.2  2001/04/24 22:53:50  lansdell
// Removed redundant radial position of first hit histograms
//
// Revision 2.1  2000/08/25 16:04:09  genevb
// Introduction of files
//
//
///////////////////////////////////////////////////////////////////////
// Names of histograms to be plotted for dir=EventQA, analType=qa_shift
///////////////////////////////////////////////////////////////////////
// Note: Editing this file means that StAnalysisUtilities/StHistUtil
// must be recompiled

  "StEQaMultClass",
  "StEQaGtrkNPntT",
  "StEQaGtrkNPntTS",
  "StEQaGtrkNPntF",
  "StEQaPointZhits",
  "StEQaPointPhiT",
  "StEQaPointZhitsS",
  "StEQaPointPhiS",
  "StEQaPointPadrowT",
  "StEQaPointBarrelS",
  "StEQaPointPlaneF",
  "StEQaGtrkZfTS",
  "StEQaGtrkPhifTS",
  "StEQaGtrkZfT",
  "StEQaGtrkPhifT",
  "StEQaGtrkXfYfFE",
  "StEQaGtrkXfYfFW",
  "StEQaGtrkPadfTEW",
  "StEQaGtrkRTS",
  "StEQaGtrkZfF",
  "StEQaGtrkRnfTTS",
  "StEQaGtrkRnmfTTS",
  "StEQaGtrkRnmF",
  "StEQaGtrkPsiTTS",
  "StEQaGtrkPtTTS",
  "StEQaGtrkEtaTTS",
  "StEQaGtrkPsiF",
  "StEQaGtrkPtF",
  "StEQaGtrkEtaF",
  "StEQaGtrkPF",
  "StEQaGtrkChisq0T",
  "StEQaGtrkChisq1T",
  "StEQaGtrkFlag",
  "StEQaGtrkGoodTot",
  "StEQaGtrkNPntFitTTS",
  "StEQaGtrkNPntF",
  "StEQaGtrkGoodTTS",
  "StEQaGtrkGoodF",
  "StEQaGtrkFitPntLTS",
  "StEQaGtrkImpactTTS",
  "StEQaGtrkImpactrTTS",
  "StEQaGtrkImpactF",
  "StEQaGtrkImpactrF",
  "StEQaGtrkDetId",
  "StEQaPtrkGlobFit",
  "StEQaPtrkTanlzf",
  "StEQaPtrkTanlzfTS",
  "StEQaPtrkPsiTTS",
  "StEQaPtrkPtTTS",
  "StEQaPtrkEtaTTS",
  "StEQaPtrkPsiF",
  "StEQaPtrkPtF",
  "StEQaPtrkEtaF",
  "StEQaPtrkMeanPtTTS",
  "StEQaPtrkMeanEtaTTS",
  "StEQaPtrkMeanPtF",
  "StEQaPtrkMeanEtaF",
  "StEQaPtrkGoodTTS",
  "StEQaPtrkGoodF",
  "StEQaPtrkChisq0TTS",
  "StEQaPtrkChisq1TTS",
  "StEQaPtrkFlag",
  "StEQaPtrkImpactTTS",
  "StEQaPtrkImpactrTTS",
  "StEQaGtrkImpactF",
  "StEQaGtrkImpactrF",
  "StEQaPtrkGlob",
  "StEQaPtrkFitPntLTS",
  "StEQaNullPrimVtx",
  "StEQaVtxPrXY",
  "StEQaVtxPrZ",
  "StEQaGtrkDcaBeamZ1",
  "StEQaGtrkDcaBeamZ2",
  "StEQaGtrkZdcaZf",
  "StEQaGtrkZdcaPsi",
  "QaDedxAllSectors",
  "StEQaPidGlobtrkDstdedxPVsDedx",
  "StEQaDedxBBTTS",
  "StEQaEvsumTotChg",
  "StEQaEvsumTotChgF",
  "StEQaV0Vtx",
  "StEQaV0VtxRDist",
  "StEQaV0VtxZDist",
  "StEQaV0VtxPhiDist",
  "StEQaV0LambdaMass",
  "StEQaV0K0Mass",
  "StEQaXiVtxTot",
  "StEQaKinkTot",
  "StEQaGtrkRZf0",

  "StELMQaGtrkNPntT",
  "StELMQaGtrkNPntTS",
  "StELMQaGtrkNPntF",
  "StELMQaPointZhits",
  "StELMQaPointPhiT",
  "StELMQaPointZhitsS",
  "StELMQaPointPhiS",
  "StELMQaPointPadrowT",
  "StELMQaPointBarrelS",
  "StELMQaPointPlaneF",
  "StELMQaGtrkZfTS",
  "StELMQaGtrkPhifTS",
  "StELMQaGtrkZfT",
  "StELMQaGtrkPhifT",
  "StELMQaGtrkXfYfFE",
  "StELMQaGtrkXfYfFW",
  "StELMQaGtrkPadfTEW",
  "StELMQaGtrkRTS",
  "StELMQaGtrkZfF",
  "StELMQaGtrkRnfTTS",
  "StELMQaGtrkRnmfTTS",
  "StELMQaGtrkRnmF",
  "StELMQaGtrkPsiTTS",
  "StELMQaGtrkPtTTS",
  "StELMQaGtrkEtaTTS",
  "StELMQaGtrkPsiF",
  "StELMQaGtrkPtF",
  "StELMQaGtrkEtaF",
  "StELMQaGtrkPF",
  "StELMQaGtrkChisq0T",
  "StELMQaGtrkChisq1T",
  "StELMQaGtrkFlag",
  "StELMQaGtrkGoodTot",
  "StELMQaGtrkNPntFitTTS",
  "StELMQaGtrkNPntF",
  "StELMQaGtrkGoodTTS",
  "StELMQaGtrkGoodF",
  "StELMQaGtrkFitPntLTS",
  "StELMQaGtrkImpactTTS",
  "StELMQaGtrkImpactrTTS",
  "StELMQaGtrkImpactF",
  "StELMQaGtrkImpactrF",
  "StELMQaGtrkDetId",
  "StELMQaPtrkGlobFit",
  "StELMQaPtrkTanlzf",
  "StELMQaPtrkTanlzfTS",
  "StELMQaPtrkPsiTTS",
  "StELMQaPtrkPtTTS",
  "StELMQaPtrkEtaTTS",
  "StELMQaPtrkPsiF",
  "StELMQaPtrkPtF",
  "StELMQaPtrkEtaF",
  "StELMQaPtrkMeanPtTTS",
  "StELMQaPtrkMeanEtaTTS",
  "StELMQaPtrkMeanPtF",
  "StELMQaPtrkMeanEtaF",
  "StELMQaPtrkGoodTTS",
  "StELMQaPtrkGoodF",
  "StELMQaPtrkChisq0TTS",
  "StELMQaPtrkChisq1TTS",
  "StELMQaPtrkFlag",
  "StELMQaPtrkImpactTTS",
  "StELMQaPtrkImpactrTTS",
  "StELMQaGtrkImpactF",
  "StELMQaGtrkImpactrF",
  "StELMQaPtrkGlob",
  "StELMQaPtrkFitPntLTS",
  "StELMQaNullPrimVtx",
  "StELMQaVtxPrXY",
  "StELMQaVtxPrZ",
  "StELMQaGtrkDcaBeamZ1",
  "StELMQaGtrkDcaBeamZ2",
  "StELMQaGtrkZdcaZf",
  "StELMQaGtrkZdcaPsi",
  "StELMQaPidGlobtrkDstdedxPVsDedx",
  "StELMQaDedxBBTTS",
  "StELMQaEvsumTotChg",
  "StELMQaEvsumTotChgF",
  "StELMQaV0Vtx",
  "StELMQaV0VtxRDist",
  "StELMQaV0VtxZDist",
  "StELMQaV0VtxPhiDist",
  "StELMQaV0LambdaMass",
  "StELMQaV0K0Mass",
  "StELMQaXiVtxTot",
  "StELMQaKinkTot",
  "StELMQaGtrkRZf0",

  "StEMMQaGtrkNPntT",
  "StEMMQaGtrkNPntTS",
  "StEMMQaGtrkNPntF",
  "StEMMQaPointZhits",
  "StEMMQaPointPhiT",
  "StEMMQaPointZhitsS",
  "StEMMQaPointPhiS",
  "StEMMQaPointPadrowT",
  "StEMMQaPointBarrelS",
  "StEMMQaPointPlaneF",
  "StEMMQaGtrkZfTS",
  "StEMMQaGtrkPhifTS",
  "StEMMQaGtrkZfT",
  "StEMMQaGtrkPhifT",
  "StEMMQaGtrkXfYfFE",
  "StEMMQaGtrkXfYfFW",
  "StEMMQaGtrkPadfTEW",
  "StEMMQaGtrkRTS",
  "StEMMQaGtrkZfF",
  "StEMMQaGtrkRnfTTS",
  "StEMMQaGtrkRnmfTTS",
  "StEMMQaGtrkRnmF",
  "StEMMQaGtrkPsiTTS",
  "StEMMQaGtrkPtTTS",
  "StEMMQaGtrkEtaTTS",
  "StEMMQaGtrkPsiF",
  "StEMMQaGtrkPtF",
  "StEMMQaGtrkEtaF",
  "StEMMQaGtrkPF",
  "StEMMQaGtrkChisq0T",
  "StEMMQaGtrkChisq1T",
  "StEMMQaGtrkFlag",
  "StEMMQaGtrkGoodTot",
  "StEMMQaGtrkNPntFitTTS",
  "StEMMQaGtrkNPntF",
  "StEMMQaGtrkGoodTTS",
  "StEMMQaGtrkGoodF",
  "StEMMQaGtrkFitPntLTS",
  "StEMMQaGtrkImpactTTS",
  "StEMMQaGtrkImpactrTTS",
  "StEMMQaGtrkImpactF",
  "StEMMQaGtrkImpactrF",
  "StEMMQaGtrkDetId",
  "StEMMQaPtrkGlobFit",
  "StEMMQaPtrkTanlzf",
  "StEMMQaPtrkTanlzfTS",
  "StEMMQaPtrkPsiTTS",
  "StEMMQaPtrkPtTTS",
  "StEMMQaPtrkEtaTTS",
  "StEMMQaPtrkPsiF",
  "StEMMQaPtrkPtF",
  "StEMMQaPtrkEtaF",
  "StEMMQaPtrkMeanPtTTS",
  "StEMMQaPtrkMeanEtaTTS",
  "StEMMQaPtrkMeanPtF",
  "StEMMQaPtrkMeanEtaF",
  "StEMMQaPtrkGoodTTS",
  "StEMMQaPtrkGoodF",
  "StEMMQaPtrkChisq0TTS",
  "StEMMQaPtrkChisq1TTS",
  "StEMMQaPtrkFlag",
  "StEMMQaPtrkImpactTTS",
  "StEMMQaPtrkImpactrTTS",
  "StEMMQaGtrkImpactF",
  "StEMMQaGtrkImpactrF",
  "StEMMQaPtrkGlob",
  "StEMMQaPtrkFitPntLTS",
  "StEMMQaNullPrimVtx",
  "StEMMQaVtxPrXY",
  "StEMMQaVtxPrZ",
  "StEMMQaGtrkDcaBeamZ1",
  "StEMMQaGtrkDcaBeamZ2",
  "StEMMQaGtrkZdcaZf",
  "StEMMQaGtrkZdcaPsi",
  "StEMMQaPidGlobtrkDstdedxPVsDedx",
  "StEMMQaDedxBBTTS",
  "StEMMQaEvsumTotChg",
  "StEMMQaEvsumTotChgF",
  "StEMMQaV0Vtx",
  "StEMMQaV0VtxRDist",
  "StEMMQaV0VtxZDist",
  "StEMMQaV0VtxPhiDist",
  "StEMMQaV0LambdaMass",
  "StEMMQaV0K0Mass",
  "StEMMQaXiVtxTot",
  "StEMMQaKinkTot",
  "StEMMQaGtrkRZf0",

  "StEHMQaGtrkNPntT",
  "StEHMQaGtrkNPntTS",
  "StEHMQaGtrkNPntF",
  "StEHMQaPointZhits",
  "StEHMQaPointPhiT",
  "StEHMQaPointZhitsS",
  "StEHMQaPointPhiS",
  "StEHMQaPointPadrowT",
  "StEHMQaPointBarrelS",
  "StEHMQaPointPlaneF",
  "StEHMQaGtrkZfTS",
  "StEHMQaGtrkPhifTS",
  "StEHMQaGtrkZfT",
  "StEHMQaGtrkPhifT",
  "StEHMQaGtrkXfYfFE",
  "StEHMQaGtrkXfYfFW",
  "StEHMQaGtrkPadfTEW",
  "StEHMQaGtrkRTS",
  "StEHMQaGtrkZfF",
  "StEHMQaGtrkRnfTTS",
  "StEHMQaGtrkRnmfTTS",
  "StEHMQaGtrkRnmF",
  "StEHMQaGtrkPsiTTS",
  "StEHMQaGtrkPtTTS",
  "StEHMQaGtrkEtaTTS",
  "StEHMQaGtrkPsiF",
  "StEHMQaGtrkPtF",
  "StEHMQaGtrkEtaF",
  "StEHMQaGtrkPF",
  "StEHMQaGtrkChisq0T",
  "StEHMQaGtrkChisq1T",
  "StEHMQaGtrkFlag",
  "StEHMQaGtrkGoodTot",
  "StEHMQaGtrkNPntFitTTS",
  "StEHMQaGtrkNPntF",
  "StEHMQaGtrkGoodTTS",
  "StEHMQaGtrkGoodF",
  "StEHMQaGtrkFitPntLTS",
  "StEHMQaGtrkImpactTTS",
  "StEHMQaGtrkImpactrTTS",
  "StEHMQaGtrkImpactF",
  "StEHMQaGtrkImpactrF",
  "StEHMQaGtrkDetId",
  "StEHMQaPtrkGlobFit",
  "StEHMQaPtrkTanlzf",
  "StEHMQaPtrkTanlzfTS",
  "StEHMQaPtrkPsiTTS",
  "StEHMQaPtrkPtTTS",
  "StEHMQaPtrkEtaTTS",
  "StEHMQaPtrkPsiF",
  "StEHMQaPtrkPtF",
  "StEHMQaPtrkEtaF",
  "StEHMQaPtrkMeanPtTTS",
  "StEHMQaPtrkMeanEtaTTS",
  "StEHMQaPtrkMeanPtF",
  "StEHMQaPtrkMeanEtaF",
  "StEHMQaPtrkGoodTTS",
  "StEHMQaPtrkGoodF",
  "StEHMQaPtrkChisq0TTS",
  "StEHMQaPtrkChisq1TTS",
  "StEHMQaPtrkFlag",
  "StEHMQaPtrkImpactTTS",
  "StEHMQaPtrkImpactrTTS",
  "StEHMQaGtrkImpactF",
  "StEHMQaGtrkImpactrF",
  "StEHMQaPtrkGlob",
  "StEHMQaPtrkFitPntLTS",
  "StEHMQaNullPrimVtx",
  "StEHMQaVtxPrXY",
  "StEHMQaVtxPrZ",
  "StEHMQaGtrkDcaBeamZ1",
  "StEHMQaGtrkDcaBeamZ2",
  "StEHMQaGtrkZdcaZf",
  "StEHMQaGtrkZdcaPsi",
  "StEHMQaPidGlobtrkDstdedxPVsDedx",
  "StEHMQaDedxBBTTS",
  "StEHMQaEvsumTotChg",
  "StEHMQaEvsumTotChgF",
  "StEHMQaV0Vtx",
  "StEHMQaV0VtxRDist",
  "StEHMQaV0VtxZDist",
  "StEHMQaV0VtxPhiDist",
  "StEHMQaV0LambdaMass",
  "StEHMQaV0K0Mass",
  "StEHMQaXiVtxTot",
  "StEHMQaKinkTot",
  "StEHMQaGtrkRZf0"
