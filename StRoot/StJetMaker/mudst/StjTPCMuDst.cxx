// $Id: StjTPCMuDst.cxx,v 1.7 2010/04/24 04:15:39 pibero Exp $
#include "StjTPCMuDst.h"

#include "StEventTypes.h"
#include "StMuDSTMaker/COMMON/StMuTypes.hh"

#include <mudst/StMuEmcPosition.h>
#include <StEmcUtil/geometry/StEmcGeom.h>

#include <TVector3.h>

StjTrackList StjTPCMuDst::getTrackList()
{
  StjTrackList ret;

  int nTracks = StMuDst::numberOfPrimaryTracks();

  double magneticField = StMuDst::event()->magneticField()/10.0; // Tesla
  for(int i = 0; i < nTracks; ++i) {
    const StMuTrack* mutrack = StMuDst::primaryTracks(i);

    if(mutrack->flag() < 0) continue;

    if(mutrack->topologyMap().trackFtpcEast() || mutrack->topologyMap().trackFtpcWest()) continue;

    StjTrack track = createTrack(mutrack, i, magneticField);

    ret.push_back(track);
  }

  return ret;
}

StjTrack StjTPCMuDst::createTrack(const StMuTrack* mutrack, int i, double magneticField)
{
  StjTrack track;

  track.runNumber = StMuDst::event()->runId();
  track.eventId = StMuDst::event()->eventId();
  track.detectorId = kTpcId;

  TVector3 p(mutrack->momentum().x(), mutrack->momentum().y(), mutrack->momentum().z());

  track.pt         = p.Pt();
  track.eta        = p.Eta();
  track.phi        = p.Phi();
  track.flag       = mutrack->flag();
  track.nHits      = mutrack->nHits(); 
  track.charge     = mutrack->charge();
  track.nHitsPoss  = mutrack->nHitsPoss();
  track.nHitsDedx  = mutrack->nHitsDedx();
  track.nHitsFit   = mutrack->nHitsFit();
  track.nSigmaPion = mutrack->nSigmaPion();
  track.nSigmaKaon = mutrack->nSigmaKaon();
  track.nSigmaProton = mutrack->nSigmaProton();
  track.nSigmaElectron = mutrack->nSigmaElectron();
  track.Tdca       = mutrack->dcaGlobal().mag();
  track.dcaX       = mutrack->dcaGlobal().x();
  track.dcaY       = mutrack->dcaGlobal().y();
  track.dcaZ       = mutrack->dcaZ();
  track.dcaD       = mutrack->dcaD();
  track.chi2       = mutrack->chi2();
  track.chi2prob   = mutrack->chi2prob();
  track.BField     = magneticField;

  // The optimum BEMC radius to use in extrapolating the track was determined to be 238.6 cm
  // (slightly behind the shower max plane) in Murad Sarsour's electron jets analysis.
  // http://cyclotron.tamu.edu/star/2006Jets/nov27_2007/details.html

  track.bemcRadius = 238.6;	// cm

  StThreeVectorF vertex = StMuDst::primaryVertex()->position();
  track.vertexZ = vertex.z(); 

  StThreeVectorD momentumAt, positionAt;
  StMuEmcPosition EmcPosition;

  if (EmcPosition.trackOnEmc(&positionAt, &momentumAt, mutrack, track.BField, track.bemcRadius))
    {
      track.exitDetectorId = 9;
      track.exitEta = positionAt.pseudoRapidity();
      track.exitPhi = positionAt.phi();
      int id(0);
      StEmcGeom::instance("bemc")->getId(track.exitPhi, track.exitEta, id);
      track.exitTowerId = id;
    }
  else if(EmcPosition.trackOnEEmc(&positionAt, &momentumAt, mutrack))
    {
      track.exitDetectorId = 13;
      track.exitEta = positionAt.pseudoRapidity();
      track.exitPhi = positionAt.phi();
      track.exitTowerId = 0; // todo 
    }
  else
    {
      track.exitDetectorId = 0;
      track.exitEta = -999;
      track.exitPhi = -999;
      track.exitTowerId = 0;
    }

  track.dEdx = mutrack->dEdx();
  track.beta = mutrack->globalTrack() ? mutrack->globalTrack()->btofPidTraits().beta() : 0;
  track.trackIndex = i;
  track.id = mutrack->id();

  return track;
}
