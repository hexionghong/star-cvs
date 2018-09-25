<html>
<head>
<title>Weak Showers</title>
<link rel="stylesheet" type="text/css" href="pythia.css"/>
<link rel="shortcut icon" href="pythia32.gif"/>
</head>
<body>

<script language=javascript type=text/javascript>
function stopRKey(evt) {
var evt = (evt) ? evt : ((event) ? event : null);
var node = (evt.target) ? evt.target :((evt.srcElement) ? evt.srcElement : null);
if ((evt.keyCode == 13) && (node.type=="text"))
{return false;}
}

document.onkeypress = stopRKey;
</script>
<?php
if($_POST['saved'] == 1) {
if($_POST['filepath'] != "files/") {
echo "<font color='red'>SETTINGS SAVED TO FILE</font><br/><br/>"; }
else {
echo "<font color='red'>NO FILE SELECTED YET.. PLEASE DO SO </font><a href='SaveSettings.php'>HERE</a><br/><br/>"; }
}
?>

<form method='post' action='WeakShowers.php'>
 
<h2>Weak Showers</h2> 
 
The emission of <i>W^+-</i> and <i>Z^0</i> gauge bosons off fermions 
is intended to be an integrated part of the initial- and final-state 
radiation frameworks, and is fully interleaved with QCD and QED emissions. 
It is a new and still not fully explored feature, however, and therefore 
it is off by default. The weak-emission machinery is described in detail 
in [<a href="Bibliography.php#refChr14" target="page">Chr14</a>]; here we only bring up some of the most relevant 
points for using this machinery. 
 
<p/> 
In QCD and QED showers the real and virtual corrections are directly 
related with each other, which means that the appropriate Sudakov factors 
can be directly obtained as a by-product of the real-emission evolution. 
This does not hold for <i>W^+-</i>, owing to the flavour-changing 
character of emissions, so-called Bloch-Nordsieck violations. These 
effects are not expected to be large, but they are not properly included, 
since our evolution framework makes no distinction in this respect 
between QCD, QED or weak emissions. Another restriction is that there 
is no simulation of the full <i>gamma^*/Z^0</i> interference: at low 
masses the QED shower involves a pure <i>gamma^*</i> component, 
whereas the weak shower generates a pure <i>Z^0</i>. 
 
<p/> 
The non-negligible <i>W/Z</i> masses have a considerable impact 
both on the matrix elements and on the phase space for their emission. 
The shower on its own is not set up to handle those aspects with a 
particularly good accuracy. Therefore the weak shower emissions are 
always matched to the matrix element for emission off a <i>f fbar</i> 
weak dipole, or some other <i>2 &rarr; 3</i> matrix element that resembles 
the topology at hand. Even if the match may not be perfect, at least the 
main features should be caught that way. Notably, the correction procedure 
is used throughout the shower, not only for the emission closest to the 
hard <i>2 &rarr; 2</i> process. In such extended applications, emission 
rates are normalized to the invariant mass of the dipole at 
the time of the weak emission, i.e. discounting the energy change by 
previous QCD/QED emissions. 
 
<p/> 
Also the angular distribution in the 
subsequent <i>V = W^+-/Z^0</i> decay is matched to the matrix element 
expression for <i>f fbar &rarr; f fbar V &rarr; f fbar f' fbar'</i> (FSR) 
and <i>f fbar &rarr; g^* V &rarr; g^* f' fbar'</i> (ISR). Afterwards the 
<i>f' fbar'</i> system undergoes showers and hadronization just like 
any <i>W^+-/Z^0</i> decay products would. 
 
<p/> 
Special for the weak showers is that couplings are different for left- and 
righthanded fermions. With incoming unpolarized beams this should average out, 
at least so long as only one weak emission occurs. In the case of several 
weak emissions off the same fermion the correlation between them will carry 
a memory of the fermion helicity. Such a memory is retained for the 
affected dipole end, and is reflected in the <code>Particle::pol()</code> 
property, it being <i>+1</i> (<i>-1</i>) for fermions considered 
righthanded (lefthanded), and 0 for the bulk where no choice has been 
made. 
 
<p/> Most events will not contain a <i>W^+-/Z^0</i> emission at all, 
which means that dedicated generator studies of weak emissions can 
become quite  inefficient. In a shower framework it is not 
straightforward to force  such emissions to happen without biasing 
the event sample in some respect. An option is available to enhance 
the emission rate artificially, but it is then the responsibility of 
the user to correct the cross section accordingly, and not to pick an 
enhancement so big that the probability for more than one emission is 
non-negligible. (It is not enough to assign an event weight 
<i>1/e^n</i> where <i>e</i> is the enhancement factor 
and <i>n</i> is the number of emitted gauge bosons. This still 
misses to account for the change in phase space for late emissions by 
the effect of earlier ones, or equivalently for the unwanted change in 
the Sudakov form factor. See [<a href="Bibliography.php#refLon13a" target="page">Lon13a</a>] for a detailed discussion 
and possible solutions.) 
 
<p/> 
Another enhancement probability is to only allow some specific 
<i>W^+-/Z^0</i> decay modes. By default the shower is inclusive, 
since it should describe all that can happen with unit probability. 
This also holds even if the <i>W^+-</i> and <i>Z^0</i> produced 
in the hard process have been restricted to specific decay channels. 
The trick that allows this is that two new "aliases" have been produced, 
a <code>Zcopy</code> with identity code 93 and a <code>Wcopy</code> with 
code 94. These copies are used specifically to bookkeep decay channels 
open for <i>W^+-/Z^0</i> bosons produced in the shower. For the rest 
they are invisible, i.e. you will not find these codes in event listings, 
but only the regular 23 and 24 ones. The identity code duplication allows 
the selection of specific decay modes for 93 and 94, i.e. for only the 
gauge bosons produced in the shower. As above it is here up to the user 
to reweight the event to compensate for the bias introduced, and to watch 
out for possible complications. In this case there is no kinematics bias, 
but one would miss out on topologies where a not-selected decay channel 
could be part of the background to the selected one, notably when more 
than one gauge boson is produced. 
 
<p/> 
Note that the common theme is that a bias leads to an event-specific 
weight, since each event is unique. It also means that the cross-section 
information obtained e.g. by <code>Pythia::stat()</code> is misleading, 
since it has not been corrected for such weights. This is different from 
biases in a predetermined hard process, where the net reduction in cross 
section can be calculated once and for all at initialization, and events 
generated with unit weight thereafter. 
 
<p/> 
The weak shower introduces a possible doublecounting problem. Namely that it 
is now possible to produce weak bosons in association with jets from two 
different channels, Drell-Yan weak production with QCD emissions and QCD 
hard process with a weak emission. A method, built on a classification of 
each event with the <i>kT</i> jet algorithm, is used to remove the 
doublecounting. Specifically, consider a tentative final state consisting 
of a <i>W/Z</i> and two jets. Based on the principle that the shower 
emission ought to be softer than the hard emission, the emission of a 
hard <i>W/Z</i> should be vetoed in a QCD event, and that of two hard 
jets in a Drell-Yan event. The dividing criterion is this whether the 
first clustering step involves the <i>W/Z</i> or not. It is suggested 
to turn this method on only if you simulate both Drell-Yan weak production 
and QCD hard production with a weak shower. Do not turn on the veto 
algorithm if you only intend to generate one of the two processes. 
 
<h3>variables</h3> 
 
Below are listed the variables related to the weak shower and common to both 
the initial- and final-state radiation. For variables only related to the 
initial-state radiation (e.g. to turn the weak shower on for ISR) see 
<?php $filepath = $_GET["filepath"];
echo "<a href='SpacelikeShowers.php?filepath=".$filepath."' target='page'>";?>Spacelike Showers</a> and for 
final-state radiation see 
<?php $filepath = $_GET["filepath"];
echo "<a href='TimelikeShowers.php?filepath=".$filepath."' target='page'>";?>Timelike Showers</a>. 
 
<br/><br/><table><tr><td><strong>WeakShower:enhancement </td><td></td><td> <input type="text" name="1" value="1." size="20"/>  &nbsp;&nbsp;(<code>default = <strong>1.</strong></code>; <code>minimum = 1.</code>; <code>maximum = 1000.</code>)</td></tr></table>
Enhancement factor for the weak shower. This is used to increase the 
statistics of weak shower emissions. Remember afterwards to correct for 
the additional weak emissions (i.e. divide the rate of weak emissions by 
the same factor). 
   
 
<br/><br/><strong>WeakShower:singleEmission</strong>  <input type="radio" name="2" value="on"><strong>On</strong>
<input type="radio" name="2" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
This parameter allows to stop the weak shower after a single emission. 
<br/>If on, only a single weak emission is allowed. 
<br/>If off, an unlimited number of weak emissions possible. 
   
 
<br/><br/><strong>WeakShower:vetoWeakJets</strong>  <input type="radio" name="3" value="on"><strong>On</strong>
<input type="radio" name="3" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
There are two ways to produce weak bosons in association with jets, namely 
Drell-Yan weak production with QCD radiation and QCD hard process with weak 
radiation. In order to avoid double counting between the two production 
channels, a veto procedure built on the <i>kT</i> jet algorithm is 
implemented in the evolution starting from a <i>2 &rarr; 2</i> QCD process, 
process codes in the range 111 - 129. The veto algorithm finds the first 
cluster step, and if it does not involve a weak boson the radiation of 
the weak boson is vetoed when <code>WeakShower:vetoWeakJets</code> is on. 
Note that this flag does not affect other internal or external processes, 
only the 111 - 129 ones. For the Drell-Yan process the same veto algorithm 
is used, but this time the event should be vetoed if the first clustering 
does contain a weak boson, see <code>WeakShower:vetoQCDjets</code> below. 
   
 
<br/><br/><strong>WeakShower:vetoQCDjets</strong>  <input type="radio" name="4" value="on"><strong>On</strong>
<input type="radio" name="4" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
This flag vetoes some QCD emission for Drell-Yan weak production to avoid 
doublecounting with weak emission in QCD hard processes. For more 
information see <code>WeakShower:vetoWeakJets</code> above. Note that 
this flag only affects the process codes 221 and 222, i.e. the main 
built-in processes for <i>gamma^*/Z^0/W^+-</i> production, and not 
other internal or external processes. 
   
 
<br/><br/><table><tr><td><strong>WeakShower:vetoWeakDeltaR </td><td></td><td> <input type="text" name="5" value="0.6" size="20"/>  &nbsp;&nbsp;(<code>default = <strong>0.6</strong></code>; <code>minimum = 0.1</code>; <code>maximum = 2.</code>)</td></tr></table>
The <i>delta R</i> parameter used in the <i>kT</i> clustering for 
the veto algorithm used to avoid double counting. Relates to the relative 
importance given to ISR and FSR emissionbs. 
   
 
<br/><br/><strong>WeakShower:externalSetup</strong>  <input type="radio" name="6" value="on"><strong>On</strong>
<input type="radio" name="6" value="off" checked="checked"><strong>Off</strong>
 &nbsp;&nbsp;(<code>default = <strong>off</strong></code>)<br/>
This flags tells the shower to use an external setup stored in the 
info pointer. This is mainly expected to be used in conjunction with the weak 
merging, and has to be switched on when the weak merging is used. 
   
 
<input type="hidden" name="saved" value="1"/>

<?php
echo "<input type='hidden' name='filepath' value='".$_GET["filepath"]."'/>"?>

<table width="100%"><tr><td align="right"><input type="submit" value="Save Settings" /></td></tr></table>
</form>

<?php

if($_POST["saved"] == 1)
{
$filepath = $_POST["filepath"];
$handle = fopen($filepath, 'a');

if($_POST["1"] != "1.")
{
$data = "WeakShower:enhancement = ".$_POST["1"]."\n";
fwrite($handle,$data);
}
if($_POST["2"] != "off")
{
$data = "WeakShower:singleEmission = ".$_POST["2"]."\n";
fwrite($handle,$data);
}
if($_POST["3"] != "off")
{
$data = "WeakShower:vetoWeakJets = ".$_POST["3"]."\n";
fwrite($handle,$data);
}
if($_POST["4"] != "off")
{
$data = "WeakShower:vetoQCDjets = ".$_POST["4"]."\n";
fwrite($handle,$data);
}
if($_POST["5"] != "0.6")
{
$data = "WeakShower:vetoWeakDeltaR = ".$_POST["5"]."\n";
fwrite($handle,$data);
}
if($_POST["6"] != "off")
{
$data = "WeakShower:externalSetup = ".$_POST["6"]."\n";
fwrite($handle,$data);
}
fclose($handle);
}

?>
</body>
</html>
 
<!-- Copyright (C) 2018 Torbjorn Sjostrand --> 
