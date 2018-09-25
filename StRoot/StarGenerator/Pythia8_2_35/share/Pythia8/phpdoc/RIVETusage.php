<html>
<head>
<title>RIVET usage</title>
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

<form method='post' action='RIVETusage.php'>
 
<h2>RIVET usage</h2> 
 
<a href="http://projects.hepforge.org/rivet/">RIVET</a> is a toolkit for 
the validation of Monte Carlo event generators [<a href="Bibliography.php#refBuc10" target="page">Buc10</a>]. It 
contains the results of many experimental analyses, so that generator 
output can easily be compared to data, as well as providing a framework to 
implement your own analyses.  Although using PYTHIA with RIVET is not 
officially supported, some helpful hints are given below. The full RIVET 
manual is available <a href="http://arxiv.org/abs/1003.0694">online</a>. 
 
<br/><br/> 
<h3>Using PYTHIA with RIVET</h3> 
The following assumes that you already have RIVET installed. Instructions 
for this may be found 
<a href="http://projects.hepforge.org/rivet/trac/wiki/GettingStarted"> 
here</a>. 
 
<br/><br/> 
Events are passed from PYTHIA to RIVET using the HepMC format. PYTHIA must 
be compiled with HepMC support, using the same version of HepMC used when 
compiling RIVET. This is setup through the PYTHIA <code>configure</code> 
script e.g. 
<pre> 
  ./configure --with-hepmc=/path/to/HepMC --with-hepmcversion=HepMC.version.number 
</pre> 
The PYTHIA library itself does not need to be recompiled. 
 
<br/><br/> 
The <code>examples/main42.cc</code> sample program can then be used to 
generate events in HepMC format (which <code>examples/main43.cc</code> 
extends by allowing subruns). When in the <code>examples</code> directory, 
the main program can be built and used as follows 
<pre> 
  make main42 
  ./main42 main42.cmnd main42.hepmc 
</pre> 
The first argument is the input file which provides the options for event 
generation, while the second is the output file where the HepMC events 
should be written. 
 
<br/><br/> 
This HepMC file may now be read and processed by RIVET 
<pre> 
  rivet --analysis=ANALYSIS_NAME main42.hepmc 
</pre> 
where <code>ANALYSIS_NAME</code> is a 
<a href="http://projects.hepforge.org/rivet/analyses">built-in RIVET 
analysis</a>, or one you have created yourself. The output of RIVET is in 
the form of <code>.aida</code> files, containing the histograms for the 
analysis, which can be processed further with RIVET (see the 
<a href="http://projects.hepforge.org/rivet/trac/wiki/FirstRivetRun"> 
RIVET documentation</a> for  details). 
 
<br/><br/> 
The above examples requires that (potentially large) HepMC events are stored 
to disk before being read by RIVET. It is possible, instead, to pass the 
events directly to RIVET as they are produced by using a <code>FIFO</code> 
pipe. This is done with the <code>mkfifo</code> command 
<pre> 
  mkfifo my_fifo 
  ./main42.exe main42.cmnd my_fifo & 
  rivet --analysis=ANALYSIS_NAME my_fifo 
</pre> 
Note that <code>main42</code> is run in the background. 
 
<h3>Compiling PYTHIA with RIVET </h3> 
 
It is also possible to compile a PYTHIA main program together with the RIVET 
library. To facilitate this, there is a header file called 
<code>Pythia8Plugins/Pythia8Rivet.h</code> defining a helper class called 
<code>Pythia8::Pythia8Rivet</code>. To use this class, a main program needs to 
be modified as follows: 
 
<pre> 
  #include "Pythia8/Pythia.h" 
 
  // Include the Pythia8Rivet header file. 
  #include "Pythia8Plugins/Pythia8Rivet.h" 
 
  int main() { 
 
    Pythia pythia; 
 
    // Setup the run by reading strings or a command file. 
 
    pythia.init(); 
 
    // Create a Pythia8Rivet object and add (one or several) analyses. 
 
    Pythia8Rivet rivet(pythia, "outputfile.yoda"); 
    rivet.addAnalysis("AnalysisName"); 
    rivet.addAnalysis("AnotherAnalysisName"); 
 
    for (int iEvent = 0; iEvent &lt; 100; ++iEvent) { 
      if (!pythia.next()) continue; 
 
      // Push event to Rivet. 
      rivet(); 
 
      // Maybe do other non-Rivet analysis. 
    } 
 
    // Tell Rivet to finalise the run. 
    rivet.done(); 
 
  } 
</pre> 
To compile the program, information about where Rivet and YODA are installed 
is needed. Hence the compile flag 
<code>-I/path/to/rivet/installation/include</code> is needed, as well as the 
link flag <code>-L/path/to/rivet/installation/lib -lRivet 
-lYODA</code>. (Further information about how to compile and link Rivet can be 
found using the <code>rivet-config</code> script distributed with Rivet.) 
 
<p/> 
The example program <code>main111.cc</code> includes optional analysis with 
<code>Pythia8::Pythia8Rivet</code>. To use it, compile the program with the 
flags above and add <code>-DUSE_PYTHIA8_RIVET</code>. 
 
<p/> 
The most common user case (run PYTHIA with a run card, using one or 
several RIVET analyses) is implemented in the example <code>main93</code>. 
The sample command file <code>main93.cmnd</code> provides a good starting 
point. The lines: 
<pre> 
    Main:runRivet = on 
    Main:analyses = ATLAS_2010_S8817804,ALICE_2010_S8625980,CMS_2011_S8957746 
    Main:writeHepMC = on 
</pre> 
provides the switch to run RIVET, and gives the user the possibility to add 
any number of (installed) RIVET analyses to the run, as a comma separated list. 
The last line is the switch needed to write a HepMC file. 
The example is run with: 
<pre> 
    ./main93 -c main93.cmnd 
</pre> 
and a .yoda file (the RIVET output) is then written. 
 
There are several other useful command line options to <code>main93</code>. 
They are all displayed by running <code>./main93 -h</code>. 
 
</body>
</html>
 
<!-- Copyright (C) 2018 Torbjorn Sjostrand --> 
