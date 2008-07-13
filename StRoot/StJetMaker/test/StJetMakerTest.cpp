// Copyright (C) 2007 Tai Sakuma <sakuma@mit.edu>
#include "StJetMakerTest.hh"

#include <StJetMaker.h>
#include <StppAnaPars.h>
#include <StConePars.h>
#include <StDefaultJetTreeWriter.h>

using namespace std;
using namespace StSpinJet;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( StJetMakerTest );

void StJetMakerTest::setUp() 
{

}

void StJetMakerTest::tearDown() 
{

}

void StJetMakerTest::testConstruct() 
{
  StJetMaker* jetmaker = new StJetMaker("emcJetMaker", 0, "");
  delete jetmaker;
}

void StJetMakerTest::testMacroInterface()
{
  StJetMaker* jetmaker = new StJetMaker("emcJetMaker", 0, "");

  StppAnaPars* anapars = new StppAnaPars();
  StConePars* cpars = new StConePars();
  jetmaker->addAnalyzer(anapars, cpars, 0, "ConeJets5");
  delete jetmaker;
}

void StJetMakerTest::testTreeWriter()
{
  StJetMaker* jetmaker = new StJetMaker("emcJetMaker", 0, "test.root");

  StppAnaPars* anapars = new StppAnaPars();
  StConePars* cpars = new StConePars();
  jetmaker->addAnalyzer(anapars, cpars, 0, "ConeJets12");

  CPPUNIT_ASSERT(static_cast<StDefaultJetTreeWriter*>(jetmaker->getTreeWriter()));

  jetmaker->Init();
  delete jetmaker;
}

void StJetMakerTest::testInit()
{
  StJetMaker* jetmaker = new StJetMaker("emcJetMaker", 0, "test.root");

  StppAnaPars* anapars = new StppAnaPars();
  StConePars* cpars = new StConePars();
  jetmaker->addAnalyzer(anapars, cpars, 0, "ConeJets12");

  jetmaker->Init();
  delete jetmaker;
}
