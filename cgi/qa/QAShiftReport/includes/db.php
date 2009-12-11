<?php

global $QAdbhost,$QAdbname;
#$QAdbhost = "duvall.star.bnl.gov";
$QAdbhost = "db09.star.bnl.gov";
$QAdbname = "OfflineQA";

###############################
# DB Generic functions
#

function connectDB() {
  global $QAdbhost,$QAdbname;
  if (connectedDB()) return;
  @($conn = mysql_connect($QAdbhost)) or died("Could not connect to the DB");
  mysql_select_db($QAdbname);
  setConnectedDB($conn);
}
function closeDB() {
  global $QAdbconn;
  if (! connectedDB()) return;
  @(mysql_close($QAdbconn)) or died("Could not close DB connection");
  setUnconnectedDB();
}
function queryDB($str) {
  global $QAdebug,$QAdbconn;
  connectDB();
  if ($QAdebug) logit("QUERY###\n$str\n###QUERY");
  @($result = mysql_query($str,$QAdbconn)) or
    died("Could not query the DB: " . ($QAdebug ? mysql_error() : "please report"));
  return $result;
}
function nextDBrow($result) {
  return mysql_fetch_assoc($result);
}
function queryDBfirst($str) {
  # returns the first such row from the DB
  $result = queryDB($str);
  if (mysql_num_rows($result)<1) return 0;
  return nextDBrow($result);
}
function queryDBarray($str,$col) {
  # returns an array of column $col from the DB
  $res = queryDB($str);
  $list = array();
  while ($row = nextDBrow($res)) { $list[] = $row["$col"]; }
  return $list;
}
function escapeDB($str) {
  if (get_magic_quotes_gpc()) { $str = stripslashes($str); }
  connectDB();
  return mysql_real_escape_string($str);
}
function optimizeTable($str) {
  $query = "ANALYZE TABLE $str;";
  queryDB($query);
  $query = "OPTIMIZE TABLE $str;";
  queryDB($query);
}


?>
