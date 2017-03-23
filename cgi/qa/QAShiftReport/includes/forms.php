<?php
# Forms HTML    
				
    function fstart($name,$action="",$target="QArfr",$meth="POST",$auto=1,$onsub="") {
      global $webdir,$refphp;
      print "<form name=\"${name}\" id=\"${name}\" action=\"";
      if ($auto==1) { print $webdir; }
      print (strlen($action) ? $action : "${refphp}.php");
      print "\" method=\"${meth}\" target=${target}";
      if (strlen($onsub)) { print " onSubmit=\"$onsub\""; }
      print ">\n";
    }

    function fstart2($name,$action="",$target="main",$onsub="") {
      global $webdir,$refphp;
      print "<form name=\"${name}\" id=\"${name}\" action=\"";
      print (strlen($action) ? $action : "${refphp}.php");
      print "\" target=${target}";
      print " onSubmit=\"${onsub};return false;\"";
      print ">\n";
    }

    function fend() { print "</form>\n"; }

    function fbutton($name,$val,$oncl) {
      print "<input type=button name=${name} value=\"${val}\"";
      print " onclick=\"${oncl}\">\n";
    }

    function fhidden($name,$val) {
      print "<input type=hidden name=${name} value=\"${val}\">\n";
    }

    function fsubmit($val,$oncl="",$onsub="") {
      print "<input type=submit name=subit value=\"${val}\"";
      if ($oncl != "") { print " onclick=\"${oncl}\""; }
      if ($onsub != "") { print " onsubmit=\"${onsub}\""; }
      print ">\n";
    }

    function freset($val) {
      print "<input type=reset name=reset value=\"${val}\">\n";
    }
    
    function finput($name,$size,$defval="",$onchg="") {
      print "<input name=${name} size=${size} value=\"${defval}\"";
      if (strlen($onchg) > 0) print " onchange=\"${onchg}\"";
      print ">\n";
    }
    
//print a textarea
    function ftext($element_name,$values,$rows=-1,$cols=-1) {
      print "<textarea name=\"${element_name}\"";
      if ($rows >= 0) { print " rows=${rows}"; }
      if ($cols >= 0) { print " cols=${cols}"; }
      print ">" . stripslashes($values[$element_name]) . "</textarea>";
    }

//print a radio button or checkbox
    function fradio($type, $element_name,
                           $default_value, $element_value) {
      print "<input type=\"${type}\" name=\"${element_name}\"" .
            " value=\"${element_value}\" ";
      if ($element_value == $default_value) {
          print " checked=\"checked\"";
      }
      print "/>";
    }

?>
