   subroutine geometry
   Implicit   none
   Logical    cave,pipe,svtt,tpce,ftpc,btof,vpdd,magp,calb,ecal,
              mfld,mwc,pse,tof,four,on/.true./,off/.false./
   real       Par(1000),field,dcay(5)
   Integer    LL,I,IDEB,Nsi
   character  Commands*8,CommandL*12
* - - - - - - - - - - - - - - - - -
+CDE,GCBANK,GCPHYS,GCCUTS,GCFLAG,AGCKINE.
* - - - - - - - - - - - - - - - - -
   replace[<P>] with [; print *,' Commands = ',Commands;]
*
*
   call ASLGETBA ('GEOM','DETP',1000,LL,Par)
   Call AGSFLAG  ('GEOM',1)
   If (JVOLUM>0) call AGDROP ('*')
   IDEB = IDEBUG
*
* -------------------- set GSTAR absolute default ------------------------
*
*  main configuration - everthing on, except for tof
   {cave,pipe,svtt,tpce,ftpc,btof,vpdd,calb,ecal,magp,mfld} = on;
   {mwc,four,pse}=on;  tof=off;
   field=5
   Nsi=7
*
* -------------------- select USERS configuration ------------------------
*
  If LL>1   
  { * set geant flags and cuts only if any detp geometry was issued:
 
   {CUTGAM,CUTELE,CUTNEU,CUTHAD,CUTMUO,BCUTE,BCUTM,DCUTE,DCUTM,PPCUTM} =.001;
   {IDCAY,IANNI,IBREM,ICOMP,IHADR,IMUNU,IPAIR,IPHOT,ILOSS,IDRAY,IMULS} = 1;
   {IRAYL,ISTRA} = 0;
   TOFMAX   = 1.e-4 
   NtrSubEv = 1000
*
   FOR i=2 to LL by 2
   {
      call UHTOC(Par(i),4,CommandL,12);  
      CALL CLTOU(CommandL);  Commands=CommandL;  

          if Commands=='HADR_ON'   
           {  print *,' default Geant Physics On'; }
      elseif Commands=='HADR_OFF'
           {  print *,' Geant Physics on, except for hadronic interactions';
              IHADR=0;
           }
      elseif Commands=='DECAY_ON'
           {  print *,' Some Physics: decays, mult.scat and energy loss';
              {IANNI,IBREM,ICOMP,IHADR,IMUNU,IPAIR,IPHOT,IDRAY}=0; 
              Iloss=2;  i+=1;
           }
      elseif Commands=='TPC_ONLY'  
           {  print *,' Minimal geometry - only TPC '
              {pipe,svtt,ftpc,btof,vpdd,magp,calb,ecal} = off;
           }
      elseif CommandL=='FIELD_ONLY'
           {  print *,' No geometry - only magnetic field '
              {cave,pipe,svtt,tpce,ftpc,btof,vpdd,magp,calb,ecal}=off; i+=1;
           }
      elseif Commands=='COMPLETE'  { print *,' Complete STAR geometry' }
      elseif Commands=='YEAR_1A'   {<p> {vpdd,calb,ecal}=off;  Nsi=0;  }
      elseif Commands=='YEAR_2A'   {<p> {vpdd,ecal}=off;       tof=on; }
      elseif CommandL=='FIELD_OFF' {<p> field=0;               i+=1;   }
      elseif Commands=='FIELD_ON'  {<p> field=5;                       }
      elseif Commands=='FIELD'     {<p> field=Par(i+2);        i+=1;   }
      elseif Commands=='MWC_OFF'   {<p> mwc=off;                       }
      elseif Commands=='PSE_OFF'   {<p> pse=off;                       }
      elseif Commands=='4TH_OFF'   {<p> Nsi=min(Nsi,6);                }
      elseif CommandL=='SPLIT_OFF' {<p> NtrSubEv=0;            i+=1;   }
      elseif Commands=='SPLIT_ON'  {<p> NtrSubEv=10000;                }
      elseif Commands=='DEBUG_ON'  {<p> Idebug=max(Idebug,1); Itest=1; }
      elseif CommandL=='DEBUG_OFF' {<p> {Idebug,Itest}=0;      i+=1;   }
      else { <p>
             If (Commands(1:4)!='HELP') _
             print *,'UNKNOWN GSTAR KEYWORD, YOU BETTER STOP'; 
             print *,'you may select : '
             print *,'---------------:-----------------------------'
             print *,'Configurations : complete, year_1a, year_2a, '
             print *,'               : tpc_only, field_only        '
             print *,'Geant Physics  : Hadr_on, Hadr_off, Decay_Only'
             print *,'Geometry Detail: mwc_off, pse_off, 4th_off   '
             print *,'Auxillary keys : Debug_on/off, Split_on/off  '
             print *,'---------------------------------------------'
             print *,'Default: complete STAR with standard physics '
             print *,'---------------------------------------------'
             if IDEB==0 & IDEBUG==0 { print *,'Nothing done !'; return; }
             i-=1; 
           }  
 } }
*
* -------------------- setup selected configuration ------------------------
*
* - to save secondaries AFTER all decays:      DETP TRAC DCAY 210 210 0.1 0.01
   dcay={210,210,0.1,0.01}
   call AgDETP new ('trac')
   call AgDETP add ('TracDcay',dcay,4)
*
   if (cave) Call cavegeo
   if (pipe) Call pipegeo

   Call AGSFLAG('SIMU',2)
* - to switch off the fourth svt layer:        DETP SVTT SVTG.nlayer=6 
   call AgDETP new ('svtt')
   if (svtt & Nsi < 7) call AgDETP add ('svtg.nlayer=',Nsi,1)
   if (svtt) Call svttgeo
 
* - MWC or pseudo padrows needed ? DETP TPCE TPCG(1).MWCread=0 TPRS(1).super=1
*  CRAY does not accept construction: IF (mwc==off) ... I do it differntly:
   call AgDETP new ('tpce')
   If (tpce &.not.mwc) call AgDETP add ('tpcg(1).MWCread=',0,1)
   If (tpce &.not.pse) call AgDETP add ('tprs(1).super='  ,1,1) 
   if (tpce) Call tpcegeo
   if (ftpc) Call ftpcgeo

* - tof system should be on (for year 2):      DETP BTOF BTOG.choice=2
   call AgDETP new ('btof')
   if (tof)  call AgDETP add ('btog.choice=',2,1)
   if (btof) Call btofgeo
     
   Call AGSFLAG('SIMU',1)
   if (vpdd) Call vpddgeo
   if (calb) Call calbgeo
   if (ecal) Call ecalgeo
   if (magp) Call magpgeo
*
* - reset magnetic field value (default is 5): DETP MFLD MFLG.Bfield=5
   call AgDETP new ('MFLD')
   if (mfld & field!=5) call AgDETP add ('MFLG(1).Bfield=',field,1)
   if (mfld) Call mfldgeo
*
   if JVOLUM>0 
   { Call ggclos
     If IDEBUG>0 { CALL ICLRWK(0,1); Call GDRAWC('CAVE',1,.2,10.,10.,.03,.03)}
   }
   IDEBUG = IDEB
   ITEST  = min(IDEB,1)
   call agphysi
*
   end
*
******************************temporary here***********************************
*
   subroutine  AgDETP add (Cpar,p,N)
+CDE,Typing,GCBANK,SCLINK.
   Character   Cpar*(*),EQ*1/'='/,Cd*4/'none'/
   Integer     LENOCC,Par(1000),p(N),N,L,I,J,LL,Id/0/,Ld
   Real        R
   Equivalence (R,I)
*
    call ASLGETBA (Cd,'DETP',1000,LL,Par)
    L=Lenocc(Cpar)
    Call UCTOH (Cpar,Par(LL+1),4,L);  LL+=(L+3)/4
    do j=1,N  { I=p(j); if (abs(I)<10000) R=p(j);  LL+=1; Par(LL)=I; }   
    call ASLSETBA (Cd,'DETP',LL,Par)
    return
*
    entry AgDETP new (Cpar)
    Cd=Cpar;  Call CLTOU(cd);  call asbdete (Cd,id)
    call ASLDETBA (Cd,'DETP',1,Ld);  If (Ld>0) call MZDROP (IxCons,Ld,' ')
*
   end



