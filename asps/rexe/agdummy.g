*************************************************************************
                SUBROUTINE    A G D U M M Y
*  description - a routine to force loading of library routines 	*
*                frequently used in some users codes                    *
Replace[REF#,#;] with [{IF}[EXIST 1]{Call #1;Call CsExt('D',#1);REF #2,;}]
*************************************************************************
Replace[LIST #;] with [ #1 _
 "Geant  "   GfKine,GfVert,GfPart,GfPath,GfHead,GDtoM,GMtoD,GlVolu,Gstpar,
             GpKine,GLast,GsHEAD,GsCKOV,GpIons,Grndm,Granor,Gfnhit,GpRung,
 "vector "   vdist,vscale,vadd,vmod,sortzv,vfill,Ublank,Ufill,Uzero,IuLast,
             SBIT0,SBIT1,SBYT,JBIT,JBYT,UCTOH,UHTOC,PROB,FREQ,
             DENLAN,DSTLAN,DIFLAN,XM1LAN,XM2LAN,RANLAN,
             RNDM,RDMIN,RDMOUT,SORTZV,
             CHISIN,VDOT,VADD,VSCALE,CROSS,
             VMOD,VDIST,VDOTN2,VSUB,VUNIT,
             BESJ0,BESJ1,BESY0,BESY1,BESI0,BESI1,BESK0,BESK1,
             EBESI0,EBESI1,EBESK0,EBESK1,DCAUCH,
             lvmin,lvmina,lvmax,lvmaxa,
 "minuit "   mnseti,mninit,mnstat,mnexcm,mnpout,mnparm,
 "matrix "   Rsinv,Dsinv,Rinv,Dinv,Dmmlt,
 "ffread "   ffinit,ffset,ffkey,ffget,ffgo,
 "random "   poissn,norran,rndm,rnorml,dircos,
 "random "   binomi,rngama,gauss,proxim,
 "comis  "   csjcal,csaddr,jumpad,Jbyt,
 "interp "   fint,polint,lfit,divdif,lfitw,lvsimx,
 "zebra  "   mzvolm,mzdred,zverif,
 "hbook  "   hplfun,hpagsz,hnoent,hnform,hfith, 
             HBOOK1,HBOOK2,HBOOKN,HFILL,HF1,HF1E,HPRINT,HDELET,HRESET,
             HFITHN,HFITGA,HFITPO,HFITEX,HPROJ1,HPROJ2,HFN,HGFIT,HXE,
             HROPEN,PAOPEN,PACLOS,PAREAD,PAWRIT,HCDIR,HGIVEN,
             HBFUN1,HBFUN2,HRNDM1,HRNDM2,HBARX,HBARY,HBAR2,
             HPAK,HPAKE,HUNPAK,HGIVE,HGN,HGNF,HGNPAR,HF2,HFF1,HFF2,
             HRIN,HROUT,HI,HIE,HIX,HIJ,HIF,HIDALL,HRDIR,HNOENT,HX,HXY,
             HTITLE,HCOPY,HSTATI,HBPROF,HOPERA,HIDOPT,HDERIV,
             HMAXIM,HMINIM,HMAX,HMIN,HSUM,HNORMA,HREND,HRENID,
             HEXIST,HRGET,HRPUT,HSCR,HFIND,HCX,HCXY,HLABEL,
             HBPROX,HBPROY,HBANDX,HBANDY,HBSLIX,HBSLIY,
             HBOOKB,HBSTAT,HDIFF,HUNPKE,HREBIN,HERROR,HPROF2,
             HOUTPU,HERMES,HISTDO,HFUNC,HXI,HIJXY,HXYIJ,
             HSPLI1,HSPLI2,HMDIR,HLDIR,HLOCAT,HFITH,HFITV,HFINAM,
             HBNT,HBNAME,HBNAMC,HFNT,HFNTB,HGNT,HGNTF,HGNTV,HBSET,
             HGNTB,HNBENT,HVXIST,HLPOS,HFC1,HSTAF,HKIND,
             HMINUT,HDIFFB,HRENAME,HNTDUP,HIJE,HMCINI,HMCMLL,
 "somesg "   rm48,rm48in,rsfact,rsfinv,rsfeqn,dsfact,rfft,cfft,
 " math  "   sortrq,dgmlt1,dgmlt2,
             dzddiv,
             SBIT0,SBIT1,SBYT,JBIT,JBYT,UCTOH,UHTOC,PROB,FREQ,
             DENLAN,DSTLAN,DIFLAN,XM1LAN,XM2LAN,RANLAN,
             CHISIN,VDOT,VADD,VSCALE,CROSS,
             VMOD,VDIST,VDOTN2,VSUB,VUNIT,
             BESJ0,BESJ1,BESY0,BESY1,BESI0,BESI1,BESK0,BESK1,
             ERF,ERFC,GAMMA
 "tr-prop    trprfn,trprop,trscsp,trspsc,trscsd,trsdsc,Ssmt5t,Xmm55 "
 ]
+CDE,GCFLAG.
  Integer  SystemF,Ix/0/;  Real x/0.0/;  Double Precision d/0.D0/
  list external;
* make sure that real calls will never be done even if this routine is called
  Ix = Ix+1;  if (Ix<=0) Return;
  Ix = Ix+1;  if (Ix>=0) Return;
  ix = SystemF(' ')
  x  = sind(x)+cosd(x)+tand(x)+atand(x)+atan2d(x,x)
  d  = dsind(d)+dcosd(d)+dtand(d)+datand(d)+datan2d(d,d)
* now fake calls to library - no need for arguments
  list ref;
*
  END
 
 
 
 
 
 
