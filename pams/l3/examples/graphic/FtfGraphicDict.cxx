//
// File generated by rootcint at Wed Oct  6 15:16:48 1999.
// Do NOT change. Changes will be lost next time file is generated
//
#include "FtfGraphicDict.h"

#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TError.h"

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, FtfGraphic *&obj)
{
   // Read a pointer to an object of class FtfGraphic.

   ::Error("FtfGraphic::operator>>", "objects not inheriting from TObject need a specialized operator>> function"); if (obj) { }
   return buf;
}

//______________________________________________________________________________
void FtfGraphic::Streamer(TBuffer &R__b)
{
   // Stream an object of class FtfGraphic.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      FtfSl3::Streamer(R__b);
      R__b >> ftfCanvas;
      R__b >> bField;
      R__b >> phi;
      R__b >> theta;
      R__b >> psi;
      R__b >> phiMin;
      R__b >> phiMax;
      R__b >> etaMin;
      R__b >> etaMax;
      R__b >> xMin;
      R__b >> xMax;
      R__b >> yMin;
      R__b >> yMax;
      R__b >> zMin;
      R__b >> zMax;
      R__b >> hitColor;
      R__b >> hitMarker;
      R__b >> trackColor;
      R__b >> trackWidth;
      R__b >> fitColor;
      R__b >> fitWidth;
   } else {
      R__b.WriteVersion(FtfGraphic::IsA());
      FtfSl3::Streamer(R__b);
      R__b << ftfCanvas;
      R__b << bField;
      R__b << phi;
      R__b << theta;
      R__b << psi;
      R__b << phiMin;
      R__b << phiMax;
      R__b << etaMin;
      R__b << etaMax;
      R__b << xMin;
      R__b << xMax;
      R__b << yMin;
      R__b << yMax;
      R__b << zMin;
      R__b << zMax;
      R__b << hitColor;
      R__b << hitMarker;
      R__b << trackColor;
      R__b << trackWidth;
      R__b << fitColor;
      R__b << fitWidth;
   }
}

//______________________________________________________________________________
void FtfGraphic::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class FtfGraphic.

   TClass *R__cl  = FtfGraphic::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "*ftfCanvas", &ftfCanvas);
   R__insp.Inspect(R__cl, R__parent, "bField", &bField);
   R__insp.Inspect(R__cl, R__parent, "phi", &phi);
   R__insp.Inspect(R__cl, R__parent, "theta", &theta);
   R__insp.Inspect(R__cl, R__parent, "psi", &psi);
   R__insp.Inspect(R__cl, R__parent, "phiMin", &phiMin);
   R__insp.Inspect(R__cl, R__parent, "phiMax", &phiMax);
   R__insp.Inspect(R__cl, R__parent, "etaMin", &etaMin);
   R__insp.Inspect(R__cl, R__parent, "etaMax", &etaMax);
   R__insp.Inspect(R__cl, R__parent, "xMin", &xMin);
   R__insp.Inspect(R__cl, R__parent, "xMax", &xMax);
   R__insp.Inspect(R__cl, R__parent, "yMin", &yMin);
   R__insp.Inspect(R__cl, R__parent, "yMax", &yMax);
   R__insp.Inspect(R__cl, R__parent, "zMin", &zMin);
   R__insp.Inspect(R__cl, R__parent, "zMax", &zMax);
   R__insp.Inspect(R__cl, R__parent, "hitColor", &hitColor);
   R__insp.Inspect(R__cl, R__parent, "hitMarker", &hitMarker);
   R__insp.Inspect(R__cl, R__parent, "trackColor", &trackColor);
   R__insp.Inspect(R__cl, R__parent, "trackWidth", &trackWidth);
   R__insp.Inspect(R__cl, R__parent, "fitColor", &fitColor);
   R__insp.Inspect(R__cl, R__parent, "fitWidth", &fitWidth);
   FtfSl3::ShowMembers(R__insp, R__parent);
}

/********************************************************
* FtfGraphicDict.cxx
********************************************************/

#ifdef G__MEMTEST
#undef malloc
#endif

extern "C" void G__cpp_reset_tagtableFtfGraphicDict();

extern "C" void G__set_cpp_environmentFtfGraphicDict() {
  G__add_compiledheader("TROOT.h");
  G__add_compiledheader("TMemberInspector.h");
  G__add_compiledheader("FtfGraphic.h");
  G__cpp_reset_tagtableFtfGraphicDict();
}
extern "C" int G__cpp_dllrevFtfGraphicDict() { return(51111); }

/*********************************************************
* Member function Interface Method
*********************************************************/

/* FtfGraphic */
static int G__FtfGraphic_FtfGraphic_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   FtfGraphic *p=NULL;
   if(G__getaryconstruct()) p=new FtfGraphic[G__getaryconstruct()];
   else                    p=new FtfGraphic;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__FtfGraphicDictLN_FtfGraphic);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfGraphic_plotDetector_2_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((FtfGraphic*)(G__getstructoffset()))->plotDetector());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfGraphic_plotHits_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((FtfGraphic*)(G__getstructoffset()))->plotHits());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfGraphic_plotTracks_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((FtfGraphic*)(G__getstructoffset()))->plotTracks());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfGraphic_plotTracks_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((FtfGraphic*)(G__getstructoffset()))->plotTracks((int)G__int(libp->para[0])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfGraphic_plotTracks_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((FtfGraphic*)(G__getstructoffset()))->plotTracks((int)G__int(libp->para[0]),(int)G__int(libp->para[1])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfGraphic_plotFits_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((FtfGraphic*)(G__getstructoffset()))->plotFits());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfGraphic_plotFits_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((FtfGraphic*)(G__getstructoffset()))->plotFits((int)G__int(libp->para[0])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfGraphic_plotFits_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((FtfGraphic*)(G__getstructoffset()))->plotFits((int)G__int(libp->para[0]),(int)G__int(libp->para[1])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfGraphic_plotFitsa_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((FtfGraphic*)(G__getstructoffset()))->plotFitsa((int)G__int(libp->para[0])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfGraphic_plotFitsa_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((FtfGraphic*)(G__getstructoffset()))->plotFitsa((int)G__int(libp->para[0]),(int)G__int(libp->para[1])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfGraphic_plotFit_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((FtfGraphic*)(G__getstructoffset()))->plotFit((FtfTrack*)G__int(libp->para[0]),(float)G__double(libp->para[1])
,(float)G__double(libp->para[2])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfGraphic_setDefaults_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,108,(long)((FtfGraphic*)(G__getstructoffset()))->setDefaults());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfGraphic_setXy_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((FtfGraphic*)(G__getstructoffset()))->setXy();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfGraphic_setYz_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((FtfGraphic*)(G__getstructoffset()))->setYz();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfGraphic_DeclFileName_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((FtfGraphic*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfGraphic_DeclFileLine_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((FtfGraphic*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfGraphic_ImplFileName_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((FtfGraphic*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfGraphic_ImplFileLine_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((FtfGraphic*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfGraphic_Class_Version_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((FtfGraphic*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfGraphic_Class_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((FtfGraphic*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfGraphic_IsA_2_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((FtfGraphic*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfGraphic_ShowMembers_3_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((FtfGraphic*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfGraphic_Streamer_4_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((FtfGraphic*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__FtfGraphic_Dictionary_5_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((FtfGraphic*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__FtfGraphic_FtfGraphic_6_2(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   FtfGraphic *p;
   if(1!=libp->paran) ;
   p=new FtfGraphic(*(FtfGraphic*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__FtfGraphicDictLN_FtfGraphic);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__FtfGraphic_wAFtfGraphic_7_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (FtfGraphic *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         delete (FtfGraphic *)((G__getstructoffset())+sizeof(FtfGraphic)*i);
   else  delete (FtfGraphic *)(G__getstructoffset());
      G__setnull(result7);
   return(1 || funcname || hash || result7 || libp) ;
}


/* Setting up global function */

/*********************************************************
* Member function Stub
*********************************************************/

/* FtfGraphic */

/*********************************************************
* Global function Stub
*********************************************************/

/*********************************************************
* Get size of pointer to member function
*********************************************************/
class G__Sizep2memfuncFtfGraphicDict {
 public:
  G__Sizep2memfuncFtfGraphicDict() {p=&G__Sizep2memfuncFtfGraphicDict::sizep2memfunc;}
    size_t sizep2memfunc() { return(sizeof(p)); }
  private:
    size_t (G__Sizep2memfuncFtfGraphicDict::*p)();
};

size_t G__get_sizep2memfuncFtfGraphicDict()
{
  G__Sizep2memfuncFtfGraphicDict a;
  G__setsizep2memfunc((int)a.sizep2memfunc());
  return((size_t)a.sizep2memfunc());
}


/*********************************************************
* virtual base class offset calculation interface
*********************************************************/

   /* Setting up class inheritance */

/*********************************************************
* Inheritance information setup/
*********************************************************/
extern "C" void G__cpp_setup_inheritanceFtfGraphicDict() {

   /* Setting up class inheritance */
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__FtfGraphicDictLN_FtfGraphic))) {
     FtfGraphic *G__Lderived;
     G__Lderived=(FtfGraphic*)0x1000;
     {
       FtfSl3 *G__Lpbase=(FtfSl3*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__FtfGraphicDictLN_FtfGraphic),G__get_linked_tagnum(&G__FtfGraphicDictLN_FtfSl3),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       FtfFinder *G__Lpbase=(FtfFinder*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__FtfGraphicDictLN_FtfGraphic),G__get_linked_tagnum(&G__FtfGraphicDictLN_FtfFinder),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
}

/*********************************************************
* typedef information setup/
*********************************************************/
extern "C" void G__cpp_setup_typetableFtfGraphicDict() {

   /* Setting up typedef entry */
   G__search_typename2("Char_t",99,-1,0,
-1);
   G__setnewtype(-1,"Signed Character 1 byte",0);
   G__search_typename2("UChar_t",98,-1,0,
-1);
   G__setnewtype(-1,"Unsigned Character 1 byte",0);
   G__search_typename2("Short_t",115,-1,0,
-1);
   G__setnewtype(-1,"Signed Short integer 2 bytes",0);
   G__search_typename2("UShort_t",114,-1,0,
-1);
   G__setnewtype(-1,"Unsigned Short integer 2 bytes",0);
   G__search_typename2("Int_t",105,-1,0,
-1);
   G__setnewtype(-1,"Signed integer 4 bytes",0);
   G__search_typename2("UInt_t",104,-1,0,
-1);
   G__setnewtype(-1,"Unsigned integer 4 bytes",0);
   G__search_typename2("Seek_t",105,-1,0,
-1);
   G__setnewtype(-1,"File pointer",0);
   G__search_typename2("Long_t",108,-1,0,
-1);
   G__setnewtype(-1,"Signed long integer 8 bytes",0);
   G__search_typename2("ULong_t",107,-1,0,
-1);
   G__setnewtype(-1,"Unsigned long integer 8 bytes",0);
   G__search_typename2("Float_t",102,-1,0,
-1);
   G__setnewtype(-1,"Float 4 bytes",0);
   G__search_typename2("Double_t",100,-1,0,
-1);
   G__setnewtype(-1,"Float 8 bytes",0);
   G__search_typename2("Text_t",99,-1,0,
-1);
   G__setnewtype(-1,"General string",0);
   G__search_typename2("Bool_t",98,-1,0,
-1);
   G__setnewtype(-1,"Boolean (0=false, 1=true)",0);
   G__search_typename2("Byte_t",98,-1,0,
-1);
   G__setnewtype(-1,"Byte (8 bits)",0);
   G__search_typename2("Version_t",115,-1,0,
-1);
   G__setnewtype(-1,"Class version identifier",0);
   G__search_typename2("Option_t",99,-1,0,
-1);
   G__setnewtype(-1,"Option string",0);
   G__search_typename2("Ssiz_t",105,-1,0,
-1);
   G__setnewtype(-1,"String size",0);
   G__search_typename2("Real_t",102,-1,0,
-1);
   G__setnewtype(-1,"TVector and TMatrix element type",0);
   G__search_typename2("VoidFuncPtr_t",89,-1,0,
-1);
   G__setnewtype(-1,"pointer to void function",0);
   G__search_typename2("FreeHookFun_t",89,-1,0,
-1);
   G__setnewtype(-1,NULL,0);
   G__search_typename2("ReAllocFun_t",81,-1,0,
-1);
   G__setnewtype(-1,NULL,0);
   G__search_typename2("ReAllocCFun_t",81,-1,0,
-1);
   G__setnewtype(-1,NULL,0);
   G__search_typename2("Axis_t",102,-1,0,
-1);
   G__setnewtype(-1,"Axis values type",0);
   G__search_typename2("Stat_t",100,-1,0,
-1);
   G__setnewtype(-1,"Statistics type",0);
   G__search_typename2("mword",102,-1,0,
-1);
   G__setnewtype(-1,NULL,0);
   G__search_typename2("UINT32",104,-1,0,
-1);
   G__setnewtype(-1,NULL,0);
   G__search_typename2("UINT16",114,-1,0,
-1);
   G__setnewtype(-1,NULL,0);
   G__search_typename2("UINT8",98,-1,0,
-1);
   G__setnewtype(-1,NULL,0);
   G__search_typename2("Font_t",115,-1,0,
-1);
   G__setnewtype(-1,"Font number",0);
   G__search_typename2("Style_t",115,-1,0,
-1);
   G__setnewtype(-1,"Style number",0);
   G__search_typename2("Marker_t",115,-1,0,
-1);
   G__setnewtype(-1,"Marker number",0);
   G__search_typename2("Width_t",115,-1,0,
-1);
   G__setnewtype(-1,"Line width",0);
   G__search_typename2("Color_t",115,-1,0,
-1);
   G__setnewtype(-1,"Color number",0);
   G__search_typename2("SCoord_t",115,-1,0,
-1);
   G__setnewtype(-1,"Screen coordinates",0);
   G__search_typename2("Coord_t",102,-1,0,
-1);
   G__setnewtype(-1,"Pad world coordinates",0);
   G__search_typename2("Angle_t",102,-1,0,
-1);
   G__setnewtype(-1,"Graphics angle",0);
   G__search_typename2("Size_t",102,-1,0,
-1);
   G__setnewtype(-1,"Attribute size",0);
   G__search_typename2("Handle_t",107,-1,0,
-1);
   G__setnewtype(-1,"Generic resource handle",0);
   G__search_typename2("Display_t",107,-1,0,
-1);
   G__setnewtype(-1,"Display handle",0);
   G__search_typename2("Window_t",107,-1,0,
-1);
   G__setnewtype(-1,"Window handle",0);
   G__search_typename2("Pixmap_t",107,-1,0,
-1);
   G__setnewtype(-1,"Pixmap handle",0);
   G__search_typename2("Drawable_t",107,-1,0,
-1);
   G__setnewtype(-1,"Drawable handle",0);
   G__search_typename2("Colormap_t",107,-1,0,
-1);
   G__setnewtype(-1,"Colormap handle",0);
   G__search_typename2("Cursor_t",107,-1,0,
-1);
   G__setnewtype(-1,"Cursor handle",0);
   G__search_typename2("FontH_t",107,-1,0,
-1);
   G__setnewtype(-1,"Font handle (as opposed to Font_t which is an index)",0);
   G__search_typename2("KeySym_t",107,-1,0,
-1);
   G__setnewtype(-1,"Key symbol handle",0);
   G__search_typename2("Atom_t",107,-1,0,
-1);
   G__setnewtype(-1,"WM token",0);
   G__search_typename2("GContext_t",107,-1,0,
-1);
   G__setnewtype(-1,"Graphics context handle (or pointer, needs to be long)",0);
   G__search_typename2("FontStruct_t",107,-1,0,
-1);
   G__setnewtype(-1,"Pointer to font structure",0);
   G__search_typename2("Mask_t",104,-1,0,
-1);
   G__setnewtype(-1,"Structure mask type",0);
   G__search_typename2("Time_t",107,-1,0,
-1);
   G__setnewtype(-1,"Event time",0);
}

/*********************************************************
* Data Member information setup/
*********************************************************/

   /* Setting up class,struct,union tag member variable */

   /* FtfGraphic */
static void G__setup_memvarFtfGraphic(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__FtfGraphicDictLN_FtfGraphic));
   { FtfGraphic *p; p=(FtfGraphic*)0x1000; if (p) { }
   G__memvar_setup((void*)((long)(&p->ftfCanvas)-(long)(p)),85,0,0,G__get_linked_tagnum(&G__FtfGraphicDictLN_TCanvas),-1,-1,1,"ftfCanvas=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->bField)-(long)(p)),102,0,0,-1,-1,-1,1,"bField=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->phi)-(long)(p)),102,0,0,-1,-1,-1,1,"phi=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->theta)-(long)(p)),102,0,0,-1,-1,-1,1,"theta=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->psi)-(long)(p)),102,0,0,-1,-1,-1,1,"psi=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->phiMin)-(long)(p)),102,0,0,-1,-1,-1,1,"phiMin=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->phiMax)-(long)(p)),102,0,0,-1,-1,-1,1,"phiMax=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->etaMin)-(long)(p)),102,0,0,-1,-1,-1,1,"etaMin=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->etaMax)-(long)(p)),102,0,0,-1,-1,-1,1,"etaMax=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->xMin)-(long)(p)),102,0,0,-1,-1,-1,1,"xMin=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->xMax)-(long)(p)),102,0,0,-1,-1,-1,1,"xMax=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->yMin)-(long)(p)),102,0,0,-1,-1,-1,1,"yMin=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->yMax)-(long)(p)),102,0,0,-1,-1,-1,1,"yMax=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->zMin)-(long)(p)),102,0,0,-1,-1,-1,1,"zMin=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->zMax)-(long)(p)),102,0,0,-1,-1,-1,1,"zMax=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->hitColor)-(long)(p)),105,0,0,-1,-1,-1,1,"hitColor=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->hitMarker)-(long)(p)),105,0,0,-1,-1,-1,1,"hitMarker=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->trackColor)-(long)(p)),105,0,0,-1,-1,-1,1,"trackColor=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->trackWidth)-(long)(p)),105,0,0,-1,-1,-1,1,"trackWidth=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->fitColor)-(long)(p)),105,0,0,-1,-1,-1,1,"fitColor=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->fitWidth)-(long)(p)),105,0,0,-1,-1,-1,1,"fitWidth=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__FtfGraphicDictLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}

extern "C" void G__cpp_setup_memvarFtfGraphicDict() {
}
/***********************************************************
************************************************************
************************************************************
************************************************************
************************************************************
************************************************************
************************************************************
***********************************************************/

/*********************************************************
* Member function information setup for each class
*********************************************************/
static void G__setup_memfuncFtfGraphic(void) {
   /* FtfGraphic */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__FtfGraphicDictLN_FtfGraphic));
   G__memfunc_setup("FtfGraphic",990,G__FtfGraphic_FtfGraphic_0_0,105,G__get_linked_tagnum(&G__FtfGraphicDictLN_FtfGraphic),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("plotDetector",1273,G__FtfGraphic_plotDetector_2_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("plotHits",855,G__FtfGraphic_plotHits_3_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("plotTracks",1063,G__FtfGraphic_plotTracks_4_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("plotTracks",1063,G__FtfGraphic_plotTracks_5_0,105,-1,-1,0,1,1,1,0,"i - - 0 - thisTrack",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("plotTracks",1063,G__FtfGraphic_plotTracks_6_0,105,-1,-1,0,2,1,1,0,
"i - - 0 - firstTrack i - - 0 - lastTrack",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("plotFits",853,G__FtfGraphic_plotFits_7_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("plotFits",853,G__FtfGraphic_plotFits_8_0,105,-1,-1,0,1,1,1,0,"i - - 0 - thisTrack",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("plotFits",853,G__FtfGraphic_plotFits_9_0,105,-1,-1,0,2,1,1,0,
"i - - 0 - firstTrack i - - 0 - lastTrack",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("plotFitsa",950,G__FtfGraphic_plotFitsa_0_1,105,-1,-1,0,1,1,1,0,"i - - 0 - thisTrack",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("plotFitsa",950,G__FtfGraphic_plotFitsa_1_1,105,-1,-1,0,2,1,1,0,
"i - - 0 - firstTrack i - - 0 - lastTrack",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("plotFit",738,G__FtfGraphic_plotFit_2_1,105,-1,-1,0,3,1,1,0,
"U 'FtfTrack' - 0 - lTrack f - - 0 - rMin "
"f - - 0 - rMax",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("setDefaults",1156,G__FtfGraphic_setDefaults_3_1,108,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("setXy",541,G__FtfGraphic_setXy_4_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("setYz",543,G__FtfGraphic_setYz_5_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__FtfGraphic_DeclFileName_6_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__FtfGraphic_DeclFileLine_7_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__FtfGraphic_ImplFileName_8_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__FtfGraphic_ImplFileLine_9_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__FtfGraphic_Class_Version_0_2,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__FtfGraphic_Class_1_2,85,G__get_linked_tagnum(&G__FtfGraphicDictLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__FtfGraphic_IsA_2_2,85,G__get_linked_tagnum(&G__FtfGraphicDictLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__FtfGraphic_ShowMembers_3_2,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__FtfGraphic_Streamer_4_2,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Dictionary",1046,G__FtfGraphic_Dictionary_5_2,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   // automatic copy constructor
   G__memfunc_setup("FtfGraphic",990,G__FtfGraphic_FtfGraphic_6_2,(int)('i'),G__get_linked_tagnum(&G__FtfGraphicDictLN_FtfGraphic),-1,0,1,1,1,0,"u 'FtfGraphic' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~FtfGraphic",1116,G__FtfGraphic_wAFtfGraphic_7_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}


/*********************************************************
* Member function information setup
*********************************************************/
extern "C" void G__cpp_setup_memfuncFtfGraphicDict() {
}

/*********************************************************
* Global variable information setup for each class
*********************************************************/
extern "C" void G__cpp_setup_globalFtfGraphicDict() {

   /* Setting up global variables */
   G__resetplocal();


   G__resetglobalenv();
}

/*********************************************************
* Global function information setup for each class
*********************************************************/
extern "C" void G__cpp_setup_funcFtfGraphicDict() {
   G__lastifuncposition();


   G__resetifuncposition();
}

/*********************************************************
* Class,struct,union,enum tag information setup
*********************************************************/
/* Setup class/struct taginfo */
G__linked_taginfo G__FtfGraphicDictLN_TClass = { "TClass" , 99 , -1 };
G__linked_taginfo G__FtfGraphicDictLN_FtfFinder = { "FtfFinder" , 99 , -1 };
G__linked_taginfo G__FtfGraphicDictLN_FtfSl3 = { "FtfSl3" , 99 , -1 };
G__linked_taginfo G__FtfGraphicDictLN_TCanvas = { "TCanvas" , 99 , -1 };
G__linked_taginfo G__FtfGraphicDictLN_FtfGraphic = { "FtfGraphic" , 99 , -1 };

/* Reset class/struct taginfo */
extern "C" void G__cpp_reset_tagtableFtfGraphicDict() {
  G__FtfGraphicDictLN_TClass.tagnum = -1 ;
  G__FtfGraphicDictLN_FtfFinder.tagnum = -1 ;
  G__FtfGraphicDictLN_FtfSl3.tagnum = -1 ;
  G__FtfGraphicDictLN_TCanvas.tagnum = -1 ;
  G__FtfGraphicDictLN_FtfGraphic.tagnum = -1 ;
}

extern "C" void G__cpp_setup_tagtableFtfGraphicDict() {

   /* Setting up class,struct,union tag entry */
   G__tagtable_setup(G__get_linked_tagnum(&G__FtfGraphicDictLN_FtfGraphic),sizeof(FtfGraphic),-1,0,(char*)NULL,G__setup_memvarFtfGraphic,G__setup_memfuncFtfGraphic);
}
extern "C" void G__cpp_setupFtfGraphicDict() {
  G__check_setup_version(51111,"G__cpp_setupFtfGraphicDict()");
  G__set_cpp_environmentFtfGraphicDict();
  G__cpp_setup_tagtableFtfGraphicDict();

  G__cpp_setup_inheritanceFtfGraphicDict();

  G__cpp_setup_typetableFtfGraphicDict();

  G__cpp_setup_memvarFtfGraphicDict();

  G__cpp_setup_memfuncFtfGraphicDict();
  G__cpp_setup_globalFtfGraphicDict();
  G__cpp_setup_funcFtfGraphicDict();

   if(0==G__getsizep2memfunc()) G__get_sizep2memfuncFtfGraphicDict();
  return;
}
class G__cpp_setup_initFtfGraphicDict {
  public:
    G__cpp_setup_initFtfGraphicDict() { G__add_setup_func("FtfGraphicDict",&G__cpp_setupFtfGraphicDict); }
   ~G__cpp_setup_initFtfGraphicDict() { G__remove_setup_func("FtfGraphicDict"); }
};
G__cpp_setup_initFtfGraphicDict G__cpp_setup_initializerFtfGraphicDict;

