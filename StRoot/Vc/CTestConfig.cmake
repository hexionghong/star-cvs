SET(CTEST_DROP_METHOD "http")
SET(CTEST_DROP_SITE "my.cdash.org")
SET(CTEST_DROP_LOCATION "/submit.php?project=Vc")
SET(CTEST_UPDATE_TYPE git)

find_program(GITCOMMAND git)
mark_as_advanced(GITCOMMAND)

SET(UPDATE_COMMAND "${GITCOMMAND}")
SET(UPDATE_OPTIONS pull)
