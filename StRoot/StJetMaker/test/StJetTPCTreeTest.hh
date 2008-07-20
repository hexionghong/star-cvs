// Copyright (C) 2008 Tai Sakuma <sakuma@bnl.gov>
#ifndef STJETTPCTREETEST_HH
#define STJETTPCTREETEST_HH

#include <cppunit/extensions/HelperMacros.h>

class StJetTPCTreeTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( StJetTPCTreeTest );
  CPPUNIT_TEST( testGetEntry );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  void testGetEntry();

private:

};


#endif // STJETTPCTREETEST_HH
