// $Id: StjTowerEnergyToTLorentzVector.cxx,v 1.3 2008/08/03 00:26:35 tai Exp $
// Copyright (C) 2008 Tai Sakuma <sakuma@bnl.gov>
#include "StjTowerEnergyToTLorentzVector.h"

#include "StjTowerEnergyList.h"

TLorentzVector StjTowerEnergyToTLorentzVector::operator()(const StjTowerEnergy& deposit)
{
  TVector3 towerLocation;
  towerLocation.SetPtEtaPhi(deposit.towerR, deposit.towerEta, deposit.towerPhi); 
  TVector3 vertex(deposit.vertexX, deposit.vertexY, deposit.vertexZ);

  TVector3 momentum = towerLocation - vertex;

  double pMag = (deposit.energy > _mass) ? sqrt(deposit.energy*deposit.energy - _mass*_mass) : deposit.energy;

  momentum.SetMag(pMag);
  TLorentzVector ret(momentum.x(), momentum.y(), momentum.z(), deposit.energy);
  return ret;
}
