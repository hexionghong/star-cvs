// @(#)root/g3d:$Name:  $:$Id: TQtCoinWidget.h,v 1.12 2007/02/07 16:01:09 fine Exp $
// Author: Valery Fine      23/05/97

/****************************************************************************
** $Id: TQtCoinWidget.h,v 1.12 2007/02/07 16:01:09 fine Exp $
**
** Copyright (C) 2002 by Valeri Fine. Brookhaven National Laboratory.
**                                    All rights reserved.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
*****************************************************************************/

#ifndef ROOT_TQtCoinWidget
#define ROOT_TQtCoinWidget

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TQtCoinWidget                                                        //
//                                                                      //
// Second ABC TQtCoinWidget specifies Window system independent openGL  //
// interface. This class uses the GL includes and isn't passed to CINT  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RVersion.h"
#if ROOT_VERSION_CODE >= 262400
// ROOT_VERSION(4,01,00)
#  include "TQGLViewerImp.h"
#else
#  include "TGLViewerImp.h"
#endif

#include "TString.h"

#ifndef __CINT__
#  include <qglobal.h>
#  if QT_VERSION < 0x40000
#    include <qframe.h>
#    include <qptrvector.h>
#  else /* QT_VERSION */
#    include <QFrame>
#    include <q3ptrvector.h>
#  endif /* QT_VERSION */
#  include <qlabel.h>
#else
#  if QT_VERSION < 0x40000
     class QFrame;
#  else /* QT_VERSION */
     class QFrame;
#  endif /* QT_VERSION */
   class QString;
#endif

//====================
class SoSeparator;
class SoNode;
class SoMaterial;
class SoCallback;
class SoQtViewer;
class SoPerspectiveCamera;
class SoCamera;
class SoSelection;
class SmAxisDisplayKit;
class SmAxisKit;
class SoClipPlaneManip;
class TCoinAxisSeparator;
class SoFieldSensor;
class SoClipPlane;
class SoCallback;
 
class QTabWidget;

//#include <qintdict.h>
//class TQtRootAction;
//====================   
   
#include <vector>

class TQt3DClipEditor;

class QAction;
class QWidget;
class TVirtualPad;
class SoQtExaminerViewer;
class TObject3DView;
class TContextMenu;
class SoGLRenderAction;
class TSimageMovie;

#ifdef __CINT__
#  define COINWIDGETFLAGSTYPE  UInt_t
#else
#  if QT_VERSION < 0x40000
#    define COINWIDGETFLAGSTYPE  Qt::WFlags
#  else
#    define COINWIDGETFLAGSTYPE  Qt::WindowFlags
#  endif
#endif     

#if QT_VERSION < 0x40000
  class TQtCoinWidget :public QFrame, public TGLViewerImp {
#else /* QT_VERSION */
//MOC_SKIP_BEGIN
  class TQtCoinWidget :public QFrame, public TGLViewerImp {
//MOC_SKIP_END
#endif /* QT_VERSION */
Q_OBJECT	  
private:
	
   SoQtViewer             *fInventorViewer;
   SoSeparator            *fRootNode;
   SoSeparator            *fShapeNode;
   SoSeparator            *fWiredShapeNode;
   SoSeparator            *fSolidShapeNode;
   SoSeparator            *fFileNode;
   SoSelection            *fSelNode;
   SoCamera               *fCamera;
   SmAxisDisplayKit       *fAxes;
   std::vector<int>        flist[3];
   TCoinAxisSeparator     *fXAxis;
   SmAxisKit              *fYAxis;
   SmAxisKit              *fZAxis;
   SoFieldSensor          *fCameraSensor;
   void                   *fPickedObject;
   
   TQtCoinWidget(const TQtCoinWidget&);
   void operator=(const TQtCoinWidget&)  {}
protected:
   QString         fSaveFile;           // the file name to save the pixmap to
   QString         fSaveType;           // the image format type name
   QString         fSaveFileMoviePattern; // the file name to save the pixmap to, frame by frame
   Int_t           fMaxSnapFileCounter; // The max number of the difffrent "snapshot files" (The length of the cyclic buffer)
   static Int_t    gfDefaultMaxSnapFileCounter; // the default max number of the different "snapshot files" (The length of the cyclic bugger)
   //QGLWidget      *fGLWidget;           // QT GL widget to render the view
   TVirtualPad    *fPad;                // For forward compatibility with the new viewer
   TContextMenu   *fContextMenu;        // ROOT Context menu for the 3D widget
   static Bool_t   fgCoinInitialized;
   TObject        *fSelectedObject;     // The last selected TObject


#ifndef __CINT__
#if QT_VERSION < 0x40000
   QPtrVector<QLabel> fStatusBar;
#else
//MOC_SKIP_BEGIN
   Q3PtrVector<QLabel> fStatusBar;
//MOC_SKIP_END
#endif
#endif

   //TQtCoinWidget *fSelectedView;        // extra viewer to show the selected object only
   //Bool_t          fSelectedViewActive;  // the flag to activate the "Selected view"
   //Bool_t          fSelectionViewer;     // Flag to create the slave viewer with no own layout
   //Bool_t          fSelectionHighlight;  // Flag to highlight the selection object in place
   //Bool_t          fShowSelectionGlobal; // Show the selected object in the global coordinate
   Bool_t          fWantRootContextMenu; // Create "ROOT Context menu" for the seelcted ROOT objects
   QAction        *fSnapShotAction;      // QAction to toglle the snap shot file saving
   SoGLRenderAction *fBoxHighlightAction;
   SoGLRenderAction *fLineHighlightAction;
   Bool_t          fWantClipPlane;       //
   SoClipPlaneManip *fClipPlaneMan; 
   SoClipPlane      *fClipPlane; 
   QTabWidget       *fHelpWidget;
   Bool_t            fRecord;
   SoCallback       *fMovie;
   TSimageMovie     *fMPegMovie;
   TQt3DClipEditor  *fPlaneEditor;

   
protected:
   friend class TQtCoinViewerImp;
   void CopyFile(const QString &fileName2Copy,Int_t counter);
   void CreateViewer(const char *name="qcoinviewer");
   // void CreateViewer(QGLWidget *share, const char *name="qglviewershared"){;}
   //void SaveHtml(Int_t counter);
   //void SaveHtml(QString &fileName, Int_t counter);
   //void CreateSelectionViewer();
   static int CreateSnapShotCounter();

   //TQtCoinWidget(TQtCoinWidget &);
   SoGLRenderAction &BoxHighlightAction();
   SoGLRenderAction &LineHighlightAction();
   QTabWidget* HelpWidget()   { return fHelpWidget; }
            void SetCliPlaneMan(Bool_t on=kTRUE);
   virtual  void SetPad(TVirtualPad *pad);  
public:
   enum {kStatusPopIn, kStatusNoBorders, kStatusOwn, kStatusPopOut};
   TQtCoinWidget(QWidget *parent=0, COINWIDGETFLAGSTYPE f=0); 
   TQtCoinWidget(TVirtualPad *pad, const char *title="Coin Viewer", UInt_t width=400, UInt_t height=300);

   virtual ~TQtCoinWidget();
   void AddRootChild(ULong_t id, EObject3DType type=kSolid);
   virtual void   Clear(const char *opt=0);
   virtual void   CreateStatusBar(Int_t nparts=1){;}
   virtual void   CreateStatusBar(Int_t *parts, Int_t nparts=1);
   virtual TContextMenu &ContextMenu(); 
   //virtual void   DeleteContext() { }
   //virtual void   MakeCurrent();
   //virtual void   Paint(Option_t *opt="");

   //virtual void   SetStatusText(const char *text, Int_t partidx=0,Int_t stype=-1);
   //virtual void   ShowStatusBar(Bool_t show = kTRUE);

   //virtual void   SwapBuffers() { };
   SoCamera *GetCamera() const;
   SoCamera &Camera() const;
   SmAxisDisplayKit *GetAxis() const { return fAxes;}
   std::vector<int> GetMyGLList1() { return flist[0]; }
   std::vector<int> GetMyGLList2() { return flist[1]; }
   std::vector<int> GetMyGLList3() { return flist[2]; }
   //virtual void   SetDrawList(UInt_t list) { fDrawList = list; }

   //virtual void   Iconify() { hide(); };
   //virtual void   Show() { show(); };
   virtual void   Update();
   
   virtual ULong_t GetViewerID() const;   
   //virtual void    SetPadSynchronize(Bool_t on=kTRUE);
   // static void    SetDefaultFileCounter(int counter);
   virtual void SetUpdatesEnabled(const bool&);
   virtual void    DisconnectPad();
   //QGLWidget *GLWidget() const { return fGLWidget;}
   virtual TVirtualPad *GetPad();
   void CreateViewer(const int){;}
   void EmitSelectSignal(TObject3DView * view);
   Bool_t Recording()  const { return fRecord;}
   void SetBoxSelection();
   void SetLineSelection();
   TObject     *GetSelected()         const { return fSelectedObject;     }
   SoQtViewer  *GetCoinViewer()       const { return fInventorViewer;     }
   Bool_t       WantRootContextMenu() const { return fWantRootContextMenu;}
   Bool_t       WasPicked(void *p) { 
      Bool_t res = (p != fPickedObject); if (res) fPickedObject = p; 
      return res; 
   }
   const QString &SaveType() const        { return fSaveType; }
   const QString &SaveFile() const        { return fSaveFile; }
   const QString &SaveFilePattern() const { return fSaveFileMoviePattern; }
#ifndef __CINT__
  public slots:
     //virtual void ActivateSelectorWidgetCB(bool);
     //virtual void ActivateSelectionHighlighCB(bool);
     //virtual void ActivateSelectionGlobalCB(bool);
     //virtual void DisconnectSelectorWidgetCB();
     virtual void AddGLList(unsigned int list, EObject3DType type=kSolid);
     virtual void RemoveGLList(unsigned int list);
     virtual void FrameAxisActionCB(bool);
      //virtual void NewViewer();
     virtual void PrintCB();
     virtual void CopyCB();
     virtual void CopyFrameCB();
     virtual void ReadInputFile(const char *fileName);
     virtual void ReadInputFile(QString fileName);
     virtual void Save(QString fileName,QString type="png");
     virtual void SetFileName(const QString &fileName);     
     virtual void SetFileType(const QString &fileType);     
     virtual void ClearCB();
     virtual void SaveAsCB();
     //virtual void SelectEventCB(bool on);
     //virtual void SelectDetectorCB(bool on);
     virtual void SetBackgroundColor(Color_t color);
     //virtual void SetBackgroundColor(const TColor *color);
     virtual void ShowObjectInfo(TObject *, const QPoint&);
     virtual void SnapShotSaveCB(bool);
     virtual void SaveMpegShot(bool);
     virtual void SaveSnapShot(bool on=TRUE);
     virtual void SmallAxesActionCB(bool on);
     virtual void ShowFrameAxisCB(bool);
     virtual void ShowLightsCB(bool);
     virtual void SynchTPadCB(bool);
     virtual void StartRecordingCB(bool on=kTRUE);
     virtual void StopRecordingCB(bool on=kTRUE);
     virtual void SetRotationAxisAngle(const float  x, const float  y, const float  z, const float a);
     virtual void SetSnapFileCounter(int counter);
     //virtual void SetFooter(QString &text);
     virtual void WantRootContextMenuCB(bool on);
     virtual void ViewAll();
     virtual void AboutCB();
     virtual void HelpCB();
  signals:
       void ObjectSelected(TObject *, const QPoint&);
       void NextFrameReady(bool on=TRUE);
#endif

//   ClassDef(TQtCoinWidget,0)  //ROOT OpenGL viewer implementation
};

// $log$
#endif
