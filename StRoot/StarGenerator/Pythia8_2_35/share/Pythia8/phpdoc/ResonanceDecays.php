<html>
<head>
<title>Resonance Decays</title>
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

<form method='post' action='ResonanceDecays.php'>
 
<h2>Resonance Decays</h2> 
 
The <code>ResonanceDecays</code> class performs the sequential decays of 
all resonances formed in the hard process. Note the important distinction 
between "resonances" and other "particles" made in PYTHIA. 
<ul> 
<li> 
The list of resonances contains <i>gamma^*/Z^0</i>, <i>W^+-</i>, top, 
the Higgs, and essentially all new particles of Beyond-the-Standard-Model 
physics: further Higgs bosons, sfermions, gauginos, techniparticles, and 
so on. The partial widths to different decay channels are perturbatively 
calculable, given the parameters of the respective model, and branching 
ratios may be allowed to vary across a (reasonably broad) resonance peak. 
Usually resonances are short-lived, and therefore it makes sense to consider 
their decays immediately after the primary hard process has been set up. 
Furthermore, in several cases the decay angular distributions are encoded 
as part of the specific process, e.g. the <i>W</i> decays differently in 
<i>f fbar &rarr; W^+-</i>, <i>f fbar &rarr; W^+ W^-</i> and 
<i>h^0 &rarr; W^+ W^- </i>. All of these particles are (in PYTHIA) only 
produced as part of the hard process itself, i.e. they are not produced 
in showers or hadronization processes. Therefore the restriction to 
specific decay channels can be consistently taken into account as a 
corresponding reduction in the cross section of a process. Finally, note 
that all of these resonances have an on-shell mass above 20 GeV, with the 
exception of some hypothetical weakly interacting and stable particles 
such as the gravitino. 
</li> 
<li> 
The other particles include normal hadrons and the Standard-Model leptons, 
including the <i>tau^+-</i>. These can be produced in the normal 
hadronization and decay description, which involve unknown nonperturbative 
parameters and multistep chains that cannot be predicted beforehand: 
a hard process like <i>g g &rarr; g g</i> can develop a shower with a 
<i>g &rarr; b bbar</i> branching, where the <i>b</i> hadronizes to a 
<i>B^0bar</i> that oscillates to a <i>B^0</i> that decays to a 
<i>tau^+</i>. Therefore any change of branching ratios - most of which 
are determined from data rather than from first principles anyway - 
will not be taken into account in the cross section of a process. 
Exceptions exist, but most particles in this class are made to decay 
isotropically. Finally, note that all of these particles have a mass 
below 20 GeV. 
</li> 
</ul> 
 
There is one ambiguous case in this classification, namely the photon. 
The <i>gamma^*/Z^0</i> combination contains a low-mass peak when 
produced in a hard process. On the other hand, photons can participate 
in shower evolution, and therefore a photon originally assumed 
massless can be assigned an arbitrarily high mass when it is allowed 
to branch into a fermion pair. In some cases this could lead to 
double-counting, e.g. between processes such as 
<i>f fbar &rarr; (gamma^*/Z^0) (gamma^*/Z^0)</i>, 
<i>f fbar &rarr; (gamma^*/Z^0) gamma</i> and 
<i>f fbar &rarr; gamma gamma</i>. Here it make sense to limit the 
lower mass allowed for the <i>gamma^*/Z^0</i> combination, 
in <code>23:mMin</code>, to be the same as the upper limit allowed 
for an off-shell photon in the shower evolution, in 
<code>TimeShower:mMaxGamma</code>. By default this matching is done 
at 10 GeV. 
 
<p/> 
In spite of the above-mentioned differences, the resonances and the 
other particles are all stored in one common 
<?php $filepath = $_GET["filepath"];
echo "<a href='ParticleData.php?filepath=".$filepath."' target='page'>";?>particle data table</a>, so as to offer a 
uniform interface to <?php $filepath = $_GET["filepath"];
echo "<a href='ParticleDataScheme.php?filepath=".$filepath."' target='page'>";?>setting and 
getting</a> properties such as name, mass, charge and decay modes, 
also for the <?php $filepath = $_GET["filepath"];
echo "<a href='ParticleProperties.php?filepath=".$filepath."' target='page'>";?>particle properties</a> 
in the event record. Some methods are specific to resonances, however, 
in particular for the calculation of partial widths and thereby of 
branching ratio. For resonances these can be calculated dynamically, 
set up at initialization for the nominal mass and then updated to the 
current mass when these are picked according to a Breit-Wigner resonance 
shape. 
 
<h3>Resonance Decays and Cross Sections</h3> 
 
As already hinted above, you have the possibility to set the allowed 
decay channels of resonances, see 
<?php $filepath = $_GET["filepath"];
echo "<a href='ParticleDataScheme.php?filepath=".$filepath."' target='page'>";?>Particle Data Scheme</a> description. 
For instance, if you study the process <i>q qbar &rarr; H^0 Z^0</i> 
you could specify that the <i>Z^0</i> should decay only to 
lepton pairs, the <i>H^0</i> only to <i>W^+ W^-</i>, the 
<i>W^+</i> only to a muon and a neutrino, while the <i>W^-</i> 
can decay to anything. Unfortunately there are limits to the 
flexibility: you cannot set a resonance to have different properties 
in different places of a process, e.g. if instead 
<i>H^0 &rarr; Z^0 Z^0</i> in the above process then the three 
<i>Z^0</i>'s would all obey the same rules. 
 
<p/> 
The restrictions on the allowed final states of a process is directly 
reflected in the cross section of it. That is, if some final states 
are excluded then the cross section is reduced accordingly. Such 
restrictions are built up recursively in cases of sequential decay 
chains. The restrictions are also reflected in the compositions of 
those events that actually do get to be generated. For instance, 
the relative rates of <i>H^0 &rarr; W^+ W^-</i> and 
<i>H^0 &rarr; Z^0 Z^0</i> are shifted when the allowed sets of 
<i>W^+-</i> and <i>Z^0</i> decay channels are changed. 
 
<p/> 
We remind that only those particles that Pythia treat as resonances 
enjoy this property, and only those that are considered as part of the 
hard process and its associated resonance decays. 
 
<p/> 
There is one key restriction on resonances: 
<br/><br/><table><tr><td><strong>ResonanceWidths:minWidth </td><td></td><td> <input type="text" name="1" value="1e-20" size="20"/>  &nbsp;&nbsp;(<code>default = <strong>1e-20</strong></code>; <code>minimum = 1e-30</code>)</td></tr></table>
Minimal allowed width of a resonance, in GeV. If the width falls below 
this number the resonance is considered stable and will not be allowed 
to decay. This is mainly intended as a technical parameter, to avoid 
disasters in cases where no open decay channels exists at all. It could 
be used for real-life decisions as well, however, but then typically 
would have to be much bigger than the default value. Special caution 
would be needed if coloured resonance particles were made stable, since 
the program would not necessarily know how to hadronize them, and 
therefore fail at that stage. 
   
 
<p/> 
In spite of this technical parameter choice, it is possible to set 
a lifetime for a resonance, and thereby to obtain displaced vertices. 
If a resonance is allowed to decay it will do so, irrespective of 
the location of the decay vertex. This is unlike 
<?php $filepath = $_GET["filepath"];
echo "<a href='ParticleDecays.php?filepath=".$filepath."' target='page'>";?>normal particle decays</a>, 
where it is possible to define some region around the primary 
vertex within which all decays should happen, with particles 
leaving that region considered stable. The logic is that resonances 
as a rule are too short-lived for secondary vertices, 
so if you pick a scenario with a long-lived but unstable resonance 
it is because you <i>want</i> to study secondary vertices. 
How to interface those decays to a detector simulation program then 
is another story, to be solved separately. Do note that a special 
treatment is needed for coloured long-lived resonances, that form 
<?php $filepath = $_GET["filepath"];
echo "<a href='Rhadrons.php?filepath=".$filepath."' target='page'>";?>R-hadrons</a>, and where charge and flavour 
may change between the production and decay vertices. 
 
<h3>Special properties and methods for resonances</h3> 
 
The method <code>ParticleData::isResonance(id)</code> allows you to 
query whether a given particle species is considered a resonance or not. 
You can also change the default value of this flag in the normal way, 
e.g. <code>pythia.readString("id:isResonance = true")</code>. 
 
<p/> 
Resonances come in two kinds. 
<ul> 
<li> 
The standard built-in ones include <i>Z^0</i>, <i>W^+-</i>, 
<i>t</i>, <i>h^0</i>, and many . These have explicit decay-widths 
formulae encoded, in classes derived from the 
<code><?php $filepath = $_GET["filepath"];
echo "<a href='SemiInternalResonances.php?filepath=".$filepath."' target='page'>";?>ResonanceWidths</a></code> 
base class. The formulae are used, e.g., to calculate all the partial 
widths as a function of the resonance masses you choose, and at 
initialization the existing total width values are overwritten. 
This is especially convenient for hypothetical states, like a <i>Z'</i>, 
where the mass is not known and therefore routinely changed. Often the 
partial widths are associated with parameters that can be changed by the 
user, e.g. for MSSM Higgs states. 
</li> 
<li> 
If a resonance does not have a class of its own, with hardcoded equations 
for all relevant partial widths, then a simpler object will be created 
at initialization. This object will take the total width and branching 
ratios as is (with the optional variations explained in the next section). 
When you set a particle to be a resonance, and do not provide any 
class to go with its width calculations, this is where it will end up. 
</li> 
</ul> 
 
Sometimes experimentalists want to modify the physical width of a 
resonance, to understand how sensitive analyses are to this width, 
if at all. For the second, simpler kind of resonances, the 
<code>id:mWidth</code> can be changed right away, but for the first 
kind any change will be overwritten at initialization. To circumvent 
this problem, the <code>id:doForceWidth</code> flag can be changed from 
the default <code>off</code> to <code>on</code>. Then the width stored 
in <code>id:mWidth</code> is strictly used to describe the Breit-Wigner 
of the resonance. Partial widths are still recalculated to set the 
mass-dependent branching ratios, but then uniformly rescaled to the 
requested total width. The width can also run across the lineshape, 
so that it deviates from the nominal one in the wings of the 
Breit-Wigner. 
 
<p/> 
For processes that contain interference terms between resonances, 
notably <i>gamma^*/Z^0</i> or <i>gamma^*/Z^0/Z'^0</i>, it is not 
obvious how these contributions should be modified consistently. 
Therefore it is necessary to set <code>WeakZ0:gmZmode = 2</code> 
or <code>Zprime:gmZmode = 3</code>, to have a pure <i>Z^0</i> or 
<i>Z'^0</i>, respectively, for width forcing to be allowed in 
these cases. 
 
<p/> 
A warning is that the different processes have cross sections 
that rescale in different ways when the resonance width is varied. 
This depends on them not having been implemented in a guaranteed 
uniform way. To illustrate the point, consider the case of an 
<i>s</i>-channel resonance, where the cross section dependence 
on the width can be written as 
<br/><i> 
sigmaHat(sHat) = constant * Gamma_in * Gamma_out 
/ ((sHat - m^2)^2 + m^2 * Gamma^2) 
</i><br/> 
Here the <code>doForceWidth = on</code> option ensures that the 
<i>Gamma</i> in the denominator is rescaled by some factor 
<i>k</i> relative to the natural width, but does not guarantee 
that <i>Gamma_in</i> and <i>Gamma_out</i> are rescaled as well. 
If all three are rescaled by the same factor <i>k</i>, as they 
should, then the integrated cross section also scales like <i>k</i>, 
assuming that the peak is reasonably narrow, so that the variation 
of PDF's across the Breit-Wigner can be neglected. This is the case 
for some processes. But in others either or both of the production 
and decay vertices can have been hardcoded, based on the coupling 
structure, and thus not scale with <i>k</i>. If only 
<i>Gamma_out</i> scales with <i>k</i>, say,  the cross section 
remains (approximately) constant, and if neither scales the cross 
section will even go like <i>1/k</i>. Such obvious normalization 
imperfections have to be corrected by hand. 
 
<p/> 
Mainly for internal usage, the 
<code><?php $filepath = $_GET["filepath"];
echo "<a href='ParticleDataScheme.php?filepath=".$filepath."' target='page'>";?>ParticleData</a></code> contain 
some special methods that are only meaningful for resonances: 
<ul> 
<li><code>resInit(...)</code> to initialize a resonance, possibly 
including a recalculation of the nominal width to match the nominal 
mass;</li> 
<li><code>resWidth(...)</code> to calculate the partial and total widths 
at the currently selected mass;</li> 
<li><code>resWidthOpen(...)</code> to calculate the partial and total 
widths of those channels left open by user switches, at the currently 
selected mass;</li> 
<li><code>resWidthStore(...)</code> to calculate the partial and total 
widths of those channels left open by user switches, at the currently 
selected mass, and store those as input for a subsequent selection of 
decay channel;</li> 
<li><code>resOpenFrac(...)</code> to return the fraction of the total 
width that is open by the decay channel selection made by users (based on 
the choice of <code><?php $filepath = $_GET["filepath"];
echo "<a href='ParticleDataScheme.php?filepath=".$filepath."' target='page'>";?>onMode</a></code> 
for the various decay channels, recursively calculated for sequential 
decays);</li> 
<li><code>resWidthRescaleFactor(...)</code> returns the factor by which 
the internally calculated PYTHIA width has to be rescaled to give the 
user-enforced width;</li> 
<li><code>resWidthChan(...)</code> to return the width for one particular 
channel (currently only used for Higgs decays, to obtain instate coupling 
from outstate width).</li> 
</ul> 
These methods actually provide an interface to the classes derived from 
the <code>ResonanceWidths</code> base class, to describe various 
resonances. 
 
<h3>Modes for Matrix Element Processing</h3> 
 
The <code>meMode()</code> value for a decay mode is used to specify 
<?php $filepath = $_GET["filepath"];
echo "<a href='ParticleDecays.php?filepath=".$filepath."' target='page'>";?>nonisotropic decays or the conversion of 
a parton list into a set of hadrons</a> in some channels of normal 
particles. For resonances it can also take a third function, namely 
to describe how the branching ratios and widths of a resonance should 
be rescaled as a function of the current mass of the decaying resonance. 
The rules are especially useful when new channels are added to an 
existing particle, or a completely new resonance added. 
 
<ul> 
<li>0 : channels for which hardcoded partial-width expressions are 
expected to exist in the derived class of the respective resonance. 
Should no such code exist then the partial width defaults to zero. 
</li> 
<li>1 - 99 : same as 0, but normally not used for resonances.</li> 
<li>100 : calculate the partial width of the channel from its stored 
branching ratio times the stored total width. This value remains unchanged 
when the resonance fluctuates in mass. Specifically there are no 
threshold corrections. That is, if the resonance fluctuates down in 
mass, to below the nominal threshold, it is assumed that one of the 
daughters could also fluctuate down to keep the channel open. (If not, 
there may be problems later on.) 
</li> 
<li>101 : calculate the partial width of the channel from its stored 
branching ratio times the stored total width. Multiply by a step threshold, 
i.e. the channel is switched off when the sum of the daughter on-shell 
masses is above the current mother mass.</li> 
<li>102 : calculate the partial width of the channel from its stored 
branching ratio times the stored total width. Multiply by a smooth 
threshold factor 
<i>beta = sqrt( (1 - m_1^2/m_2 - m_2^2/m^2)^2 - 4 m_1^2 m_2^2/m^4)</i> 
for two-body decays and <i>sqrt(1 - Sum_i m_i / m)</i> for multibody 
ones. The former correctly encodes the size of the phase space but 
misses out on any nontrivial matrix-element behaviour, while the latter 
obviously is a very crude simplification of the correct phase-space 
expression. Specifically, it is thereby assumed that the stored branching 
ratio and total width did not take into account such a factor.</li> 
<li>103 : use the same kind of behaviour and threshold factor as for 
102 above, but assume that such a threshold factor has been used when 
the default branching ratio and total width were calculated, so that one 
should additionally divide by the on-shell threshold factor. Specifically, 
this will give back the stored branching ratios for on-shell mass, 
unlike the 102 option. To avoid division by zero, or in general 
unreasonably big rescaling factors, a lower limit 
<code>minThreshold</code> (see below) on the value of the on-shell 
threshold factor is imposed. (In cases where a big rescaling is 
intentional, code 102 would be  appropriate.) </li> 
</ul> 
 
<br/><br/><table><tr><td><strong>ResonanceWidths:minThreshold </td><td></td><td> <input type="text" name="2" value="0.1" size="20"/>  &nbsp;&nbsp;(<code>default = <strong>0.1</strong></code>; <code>minimum = 0.01</code>)</td></tr></table>
Used uniquely for <code>meMode = 103</code> to set the minimal value 
assumed for the threshold factor, 
<i>sqrt( (1 - m_1^2/m_2 - m_2^2/m^2)^2 - 4 m_1^2 m_2^2/m^4)</i> 
for two-body decays and <i>sqrt(1 - Sum_i m_i / m)</i> for multibody 
ones. Thus the inverse of this number sets an upper limit for how 
much the partial width of a channel can increase from the on-shell 
value to the value for asymptotically large resonance masses. Is mainly 
intended as a safety measure, to avoid unintentionally large rescalings. 
   
 
<p/> 
All of these <code>meMode</code>'s may coexist for the same resonance. 
This would be the case e.g. if you want to add a few new channels to an 
already existing resonance, where the old partial widths come hardcoded 
while the new ones are read in from an external file. The typical example 
would be an MSSM Higgs sector, where partial widths to SM particles are 
already encoded, <code>meMode = 0</code>, while decay rates to sparticles 
are read in from some external calculation and maybe would be best 
approximated by using <code>meMode = 103</code>. Indeed the default 
particle table in PYTHIA uses 103 for all channels that are expected 
to be provided by external input. 
 
<p/> 
Some further clarification may be useful. At initialization the existing 
total width and on-shell branching ratios will be updated. For channels 
with <code>meMode &lt; 100</code> the originally stored branching ratios 
are irrelevant, since the existing code will anyway be used to calculate 
the partial widths from scratch. For channels with <code>meMode = 100</code> 
or bigger, instead the stored branching ratio is used together with the 
originally stored total width to define the correct on-shell partial width. 
The sum of partial widths then gives the new total width, and from there 
new branching ratios are defined. 
 
<p/> 
In these operations the original sum of branching ratios need not be 
normalized to unity. For instance, you may at input have a stored total 
width of 1 GeV and a sum of branching ratios of 2. After initialization 
the width will then have been changed to 2 GeV and the sum of branching 
ratios rescaled to unity. This might happen e.g. if you add a few channels 
to an existing resonance, without changing the branching ratios of the 
existing channels or the total width of the resonance. 
 
<p/> 
In order to simulate the Breit-Wigner shape correctly, it is important 
that all channels that contribute to the total width are included in the 
above operations. This must be kept separate from the issue of which 
channels you want to have switched on for a particular study, to be 
considered next. 
 
<p/> 
 
In the event-generation process, when an off-shell resonance mass has been 
selected, the width and branching ratios are re-evaluated for this new mass. 
At this stage also the effects of restrictions on allowed decay modes are 
taken into account, as set by the <code>onMode</code> switch for each 
separate decay channel. Thus a channel may be on or off, with different 
choices of open channels between the particle and its antiparticle. 
In addition, even when a channel is on, the decay may be into another 
resonance with its selection of allowed channels. It is these kinds of 
restrictions that lead to the <i>Gamma_out</i> possibly being 
smaller than <i>Gamma_tot</i>.  As a reminder, the Breit-Wigner for 
decays behaves like <i>Gamma_out / ((s - m^2)^2 + s * Gamma_tot^2)</i>, 
where the width in the numerator is only to those channels being studied, 
but the one in the denominator to all channels of the particle. These 
ever-changing numbers are not directly visible to the user, but are only 
stored in a work area. 
 
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

if($_POST["1"] != "1e-20")
{
$data = "ResonanceWidths:minWidth = ".$_POST["1"]."\n";
fwrite($handle,$data);
}
if($_POST["2"] != "0.1")
{
$data = "ResonanceWidths:minThreshold = ".$_POST["2"]."\n";
fwrite($handle,$data);
}
fclose($handle);
}

?>
</body>
</html>
 
<!-- Copyright (C) 2018 Torbjorn Sjostrand --> 
