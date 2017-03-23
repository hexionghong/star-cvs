<?php

#############################
# Frame for Table of Contents
#

@(include "setup.php") or die("Problems (0).");
incl("entrytypes.php");

$work = getSesName();
$play = isPlaySes($work);

head("STAR QA Shift Report Menu");
?>

    <style type="text/css">
    <!--
      a.items:link {color: navy; text-decoration: none; }
      a.items:visited {color: navy; text-decoration: none; }
      a.items:active {color: tomato; text-decoration: none; }
      a.items:hover {color: maroon; text-decoration: none; }
    -->
    </style>

<?php
jstart();
?>
    function eraseSession() {
      cString = "Do you really want to erase this session?\n\n";
      cString += "Session name: <?php print $work; ?>";
      if ( confirm(cString) ) { return true; }
      return false;
    }
    function showSubMenu() {
      document.getElementById('Tsubmenu').style.display = 'block';
      return false;
    }
    function hideSubMenu() {
      document.getElementById('Tsubmenu').style.display = 'none';
      return false;
    }
<?php
jend();
body();

function mymkhref($ref,$val,$trg="QArfr",$onc="") {
  print "    ";
  mkhref($ref . "\" class=\"items",$val,$trg,$onc);
}
function mymkhref2($ref,$val,$trg="QArfr",$onc="") {
  print "    ";
  mkhref2($ref . "\" class=\"items",$val,$trg,$onc);
}
$defCol = $myCols["emph"];
function BeginRow($col) { print "  <tr><td bgcolor=\"${col}\">\n"; }
function EndRow() { print "  </td></tr>\n"; }
function NewRow($col) { EndRow(); BeginRow($col); }

print "<b>Menu</b>\n\n";
print "<table border=0 cellpadding=1 cellspacing=1 width=\"100%\">";
NewRow($myCols["good"]);
mymkhref2("http://drupal.star.bnl.gov/STAR/comp/qa/offline/QAShiftReportInstructions",
  "<b>QA Shift Report Instructions</b>","QASRinstruct");
BeginRow($myCols["alt1"]);

print "\n  <table cellpadding=0 cellspacing=0 border=0 width=\"100%\"><tr><td>\n";
mymkhref("sessions.php?erase=0","Session");
print "  </td><td align=right>";
if ($play) { print "<font size=-1><i>(play)</i></font>&nbsp;"; }
print "<b>$work</b></td></tr></table>\n\n";

# Include the following only if session is defined
if (defd($work)) {

NewRow($defCol);
print "Manage Contents:\n";
NewRow($defCol);
mymkhref("contents.php?mode=View","&#149; View Current Contents");
NewRow($defCol);
mymkhref("info.php?work=${work}&mode=Edit","&#149; (Re)Enter Shift Info");
EndRow();
  # GVB: to show FRP only...
  # 1) no menu hiding (display:none switched to block)
  # 2) if ($k === "FRP") {}
  # 3) <tr onmouseover="showSubMenu()" onmouseout="hideSubMenu()">
?>
  <tr>
  <td bgcolor="<?php print $myCols["emph"]; ?>">
    &#149; <font color=navy>Add A Data Entry For...</font>

    <table id ="Tsubmenu" border=0 cellpadding=1 cellspacing=1
           style="display:block ;z-index:2">
<?php
foreach ($ents as $k => $v) {
if ($k === "FRP") {
  print "    <tr><td bgcolor=cornsilk>\n      ";
  mkhref("formData.php?editit=no&type=$k\" class=\"items",$v);
  print "    </td></tr>\n";
}
}
?>
    </table>


<?php

Newrow($defCol);
mymkhref("wrapup.php","&#149; Finish &amp; Submit Report");

}
# end of "if defined session"

Newrow($myCols["alt2"]);
mymkhref("issueEditor.php","Open Issue Browser/Editor","QAifr");
Newrow($myCols["alt2"]);
mymkhref2("showRun.php","Open Report Archive","QAafr");

if (defd($work)) {
Newrow($myCols["bad"]);
mymkhref("sessions.php?erase=1","Erase Session &amp; Start Over",
       "QArfr","return eraseSession()");
}

Endrow();
print "</table>\n\n";

foot(); ?>
