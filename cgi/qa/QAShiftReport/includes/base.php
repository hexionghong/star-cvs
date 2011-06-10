<?php

###############################
# Basic setup
#
    $QAdebug = 0;
    define("QAnull",".");

    # Base directory for working on reports:
    $bdir = "/home/users/starweb/WWW/tmp/QAShiftReport/";

    # Web directory of web-access scripts
    $webdir = "http://" . $_SERVER["HTTP_HOST"] .
                  dirname($_SERVER["SCRIPT_NAME"]) . "/";
    # File system directory of web-access scripts
    $fsdir = dirname($_SERVER["PATH_TRANSLATED"]) . "/";

    # Log file
    $elog = $bdir . "log/QAlog.txt";

    function incl($file) {
      global $incdir;
      @(include_once $incdir . $file) or die("Problems (2).");
    }
    
    incl("shift.php");
    incl("logit.php");
    

###############################
# DB base needs
#
    $QAdbconn = QAnull;
    function connectedDB() {
      global $QAdbconn;
      return ($QAdbconn != QAnull);
    }
    function setConnectedDB($conn) {
      global $QAdbconn;
      $QAdbconn = $conn;
    }
    function setUnconnectedDB() {
      global $QAdbconn;
      $QAdbconn = QAnull;
    }


###############################
# Page functions
#
    function head($title) {
      incl("head.html");
      print $title;
      incl("head2.html");
    }
    function body() { incl("body.html"); }
    function foot($noClose=0,$numBR=2) {
      for ($nbr=0;$nbr<$numBR;$nbr++) { linebreak(); }
      incl("foot.html");
      if ($noClose!=0 && connectedDB()) closeDB();
    }
    
    
###############################
# Status functions
#
    function died($str) {
      $code = $_SERVER["REQUEST_URI"];
      $str = "*** DIED ***: ${code}: ${str}";
      logit($str);
      die("${str}\n");
    }
    function reloadMenu() {
      jstart();
      print "  parent.QAmfr.location.reload();\n";
      jend();
    }
    function needSesName() {
      global $webdir;
      $ses = GetSesName();
      if (!defd($ses)) {
        header("location: ${webdir}sessions.php");
	exit;
      }
      return $ses;
    }
    
    
###############################
# Parameter parsing functions
#
    function getPassedVar($vname,$noDie=0) {
      $given = 0;
      if (isset($_POST["$vname"])) { $given=1; }
      elseif (isset($_GET["$vname"])) { $given=2; }
      elseif ($noDie==0) { died("${vname} not passed!"); }
      else { return false; }
      global $$vname;
      if ($given==1) { $$vname = $_POST["$vname"]; }
      elseif ($given==2) { $$vname = $_GET["$vname"]; }
      return true;
    }
    function getPassedVarStrict($vname,$noDie=0) {
      if (!getPassedVar($vname,$noDie)) { return false; }
      global $$vname;
      return cleanStrict($$vname);
    }
    function getPassedInt($vname,$noDie=0) {
      if (!getPassedVar($vname,$noDie)) { return false; }
      global $$vname;
      return cleanInt($$vname);
    }
    

###############################
# Scrubber functions
#
    function cleanStrict(&$var) {
      # Allow only alphanumeric or underscore characters
      if (preg_match("/\W/",$var)) {
        $var = "";
        return false;
      }
      return true;
    }
    function cleanInt(&$var) {
      # Allow only integers (with a possible minus sign)
      if (!preg_match("/^-?\d+$/",$var)) {
        $var = intval(0);
        return false;
      }
      return true;
    }
    function cleanFileName(&$var) {
      # Useful for filenames, strip just about all special characters
      # but leave periods and forward slashes
      $var = preg_replace(
        "/[\s\|\>\<\;\*\?\!\@\$\%\^\&\(\)\'\"\:\{\}\[\]\`\~].*/","",$var);
    }
    function cleanAlphaFileName(&$var) {
      # Require filename to start with an alphanumeric or underscore
      # to prevent absolute filenames, or ones that go up dirs (e.g. ../*)
      if (preg_match("/^\W/",$var)) {
        # just to give something back
        $var = preg_replace("/\W/","",$var);
        return false;
      }
      cleanFileName($var);
      return true;
    }


###############################
# Forms HTML
#
incl("forms.php");


###############################
# Javascript HTML
#
    function jstart() {
      print "\n<script type=\"text/javascript\">\n<!--\n";
    }
    function jend() {
      print "\n// -->\n</script>\n\n";
    }
    $jHSdone = 0;
    function jhideshow() {
    global $jHSdone;
    if ($jHSdone) return;
    $jHSdone = 1;
    ?>
    function showElem(ename) {
      elem = eval("document.getElementById('" + ename + "')");
      if (!elem) return 0;
      elem.style.display = 'inline';
      return elem;
    }
    function hideElem(ename) {
      elem = eval("document.getElementById('" + ename + "')");
      if (!elem) return 0;
      elem.style.display = 'none';
      return elem;
    }
<?php
    }
    $jTransDone = 0;
    function jTrans() {
    global $jTransDone;
    if ($jTransDone) return;
    $jTransDone = 1;
    ?>
    function opaqueElem(ename) {
      elem = eval("document.getElementById('" + ename + "')");
      if (!elem) return 0;
      elem.style.opacity = 1.0;
      elem.style.filter = 'alpha(opacity=1.0)';
      return elem;
    }
    function transElem(ename) {
      elem = eval("document.getElementById('" + ename + "')");
      if (!elem) return 0;
      elem.style.opacity = 0.3;
      elem.style.filter = 'alpha(opacity=3)';
      return elem;
    }
<?php
    }


###############################
# General HTML
#
    function linebreak() { print "<br>\n"; }
    function holine($width) { print "\n\n<p>\n<hr width=\"${width}%\">\n<p>\n\n\n"; }
    function holinel($width) { print "\n\n<hr width=\"${width}%\" align=left>\n\n\n"; }
    function holines($size) { print "\n\n<p>\n<hr size=\"${size}\" noshade>\n<p>\n\n\n"; }
    function nDigits($n,$val) {
      $str = strval($val);
      while (strlen($str) < $n) { $str = "0" . $str; }
      return $str;
    }
    function mkhref($ref,$val,$trg="QArfr",$onc="") {
      global $webFullFir,$webdir;
      mkhref2($webdir . $ref,$val,$trg,$onc);
    }
    function mkhref2($ref,$val,$trg="QArfr",$onc="") {
      print "<a href=\"${ref}\" target=\"${trg}\"";
      if ($onc != "") { print " onclick=\"${onc}\""; }
      print ">${val}</a>\n";
    }

    $myCols = array("good" => "#dfec9f",
                    "bad"  => "#ffbc9f",
                    "emph" => "#ffdc9f");

?>
