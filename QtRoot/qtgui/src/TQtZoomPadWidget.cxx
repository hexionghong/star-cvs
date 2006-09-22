// Author: Valeri Fine   16/03/2006
/****************************************************************************
** $Id: TQtZoomPadWidget.cxx,v 1.2 2006/09/22 17:27:11 fine Exp $
**
** Copyright (C) 2006 by Valeri Fine. Brookhaven National Laboratory.
**                                    All rights reserved.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
*****************************************************************************/

#include "TQtZoomPadWidget.h"
#include "TQtWidget.h"
#include "TGQt.h"
#include "TCanvas.h"
#include "qevent.h"
#include "qtooltip.h"
#include "qapplication.h"
#if QT_VERSION >= 0x40000
//Added by qt3to4:
#include <QMouseEvent>
#include <Q3Frame>
#include <QResizeEvent>
#endif /* QT_VERSION */


/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//                                                                                 //
//  TQtZoomPadWidget - create the temporary "splash" TCanvas with the magnified    //
//                     image of the selected TPad  connected to the TQtZoomPadWidget //
//                     via either ROOT or Qt signal/slot                           //
//                     One instance of the class is sufficient to serve            //
//                     the unlimited number of the TPad's                          //
//                                                                                 //
// begin_html <img src="png/ZoomPad.png"> end_html
//                                                                                 //
// For example:                                                                    //
// ------------                                                                    //
//   TQtZoomPadWidget *zoomer = new TQtZoomPadWidget();                            //
//   c1 = new TCanvas("c1","Histogram Drawing Options",200,10,700,900);            //
//   zoomer->Connect(c1);                                                          //
//                                                                                 //
//   c2 = new TCanvas("c2","Geometry objects ",200,10,700,900);                    //
//   zoomer->Connect(c2);                                                          //
//                                                                                 //
//  Note: There is no need to create any special zoomer to connect the TCanvas     //
//  ----- to the global one. It is enough simply select the "zoomer" menu from     //
//        the "View" dropdown TCanvas menu (see picture above)                     //
//                                                                                 //
//  Slots:                                                                         //
//    virtual void Selected(TVirtualPad *pad, TObject *obj, int event);            //
//          This slot is convinient to be connected                                //
//          to the TCanvas::Selected ROOT signal                                   //
//                                                                                 //
//    virtual void RootEventProcessed(TObject *selected, unsigned int event, TCanvas *c) //
//          This slot is convinient to be connected                                //
//          to the TQtWidget::RootEventProcessed  Qt signal                        //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////
#if QT_VERSION < 0x40000
TQtZoomPadWidget::TQtZoomPadWidget(const TQtZoomPadWidget &):QHBox(){}
#else /* QT_VERSION */
TQtZoomPadWidget::TQtZoomPadWidget(const TQtZoomPadWidget &):Q3HBox(){}
#endif

//__________________________________________________________________________________
#if QT_VERSION < 0x40000
TQtZoomPadWidget::TQtZoomPadWidget(TVirtualPad *pad, QWidget *parent, const char *name, WFlags f)
: QHBox(parent, name, f),fSelectingButton(kButton2Down),fSetPadInProgress(false),fSmartZoomFactor(true)
#else /* QT_VERSION */
TQtZoomPadWidget::TQtZoomPadWidget(TVirtualPad *pad, QWidget *parent, const char *name, Qt::WFlags f)
: Q3HBox(parent, name, f),fSelectingButton(kButton2Down),fSetPadInProgress(false),fSmartZoomFactor(true)
#endif /* QT_VERSION */
 , fJustOpen(true), fOldWidth(-1), fOldHieght(-1),fPad(0),fHideOnLeave(true),fZoomFactor(1.8), fSrcWidget(0),fMouseBits(0)
{
    // Create the Embedded TCanvas to draw the zoomed image of the "pad"
    // Other parameteres are passed to QHBox ctor

    TVirtualPad *savePad = gPad;
    TQtWidget *w = new TQtWidget(this);
    connect(w,SIGNAL(RootEventProcessed(TObject *, unsigned int, TCanvas *))
           ,this,SLOT(CanvasEvent(TObject *, unsigned int, TCanvas *)));
    w->EnableSignalEvents(kMousePressEvent);
    fCanvas = w->GetCanvas();
    DefineTooTip(fHideOnLeave);
    SetPad(pad,false);
    w->show();
    // restore the current TPad (just in case)
    if (savePad) savePad->cd();
    SmartZoomEnable (true);
}
//__________________________________________________________________________________
void TQtZoomPadWidget::leaveEvent(QEvent *e)
{
    // Hide the Zoomer  according the "hide on leave" mode

    if (!fSetPadInProgress && fHideOnLeave) 
    {
       Disconnect();
       emit madeHidden();
    }
#if QT_VERSION < 0x40000
    QFrame::leaveEvent(e);
#else /* QT_VERSION */
    Q3Frame::leaveEvent(e);
#endif /* QT_VERSION */
}
//__________________________________________________________________________________
void TQtZoomPadWidget::CanvasEvent(TObject *, unsigned int event, TCanvas *)
{
   // Toggle the "hide on leave" mode on/off by
   // middle mouse click
   
   if (event == kButton2Down) HideOnLeave(!fHideOnLeave);
}
//__________________________________________________________________________________
void TQtZoomPadWidget::mousePressEvent(QMouseEvent *e)
{
   // Toggle the "hide on leave" mode on/off by
   // middle mouse click
   
   if (e->QMouseEvent::button()== Qt::LeftButton) {
      HideOnLeave(!fHideOnLeave); 
      fPad->cd();
   }
}
//__________________________________________________________________________________
void TQtZoomPadWidget::resizeEvent(QResizeEvent *e)
{
   // Change the zoom factor on resize if any;
   if (HasSmartZoom() && !fJustOpen ) {
      if (fOldWidth > 0) {
        // the current factor
        fZoomFactor *= sqrt(e->   size().width() * e->   size().height()
                            /double(fOldWidth * fOldHieght));
      }
      fOldWidth  = e->size().width();
      fOldHieght = e->size().height();
   }
   if (HasSmartZoom()) fJustOpen = false;
#if QT_VERSION < 0x40000
   QHBox::resizeEvent(e);
#else /* QT_VERSION */
   Q3HBox::resizeEvent(e);
#endif /* QT_VERSION */
}
//__________________________________________________________________________________
void TQtZoomPadWidget::Connect(const char *canvasName)
{
    // (Qt/ROOT) SLOT 
    //
    // Connect TCavas with the given name "canvasName" to this zoomer
   if (canvasName && canvasName[0]) {
     TCanvas *canvas = (TCanvas *) gROOT->GetListOfCanvases()->FindObject(canvasName);
     if (canvas) Connect(canvas);
   }
}

//__________________________________________________________________________________
void TQtZoomPadWidget::Connect(TCanvas  *canvas)
{
    // (Qt/ROOT) SLOT 
    //
    // Connect TCavas "canvas" to this zoomer
    //
    // This is an overloaded member function, provided for convenience to connect the TCanvas
    // obejct "canvas" to this zoomer. It behaves essentially like the above function.
    if (canvas) {
      Int_t wid = canvas->GetCanvasID();
      Connect(wid);
    }
}

//__________________________________________________________________________________
void TQtZoomPadWidget::Connect(int  wid)
{
   // (Qt/ROOT) SLOT 
   //
   // wid - an index if the TCanvas implementation
   //
   // This is an overloaded member function, provided for convenience to connect the TCanvases
   //  to this zoomer. It behaves essentially like the above function.
   
   QPaintDevice *dev =  TGQt::iwid(wid);
   if (dev) {
      TQtWidget *widget = dynamic_cast<TQtWidget *>(dev);
      if (widget)  Connect(widget);
   }
}

//__________________________________________________________________________________
void TQtZoomPadWidget::Connect(TQtWidget  *wid)
{
   // (Qt/ROOT) SLOT 
   //
   // TQtWidget - Qt-widget that implement the ROOT TCanvas GUI interface 
   //
   // This is an overloaded member function, provided for convenience to connect the TQtWidget
   //  to this zoomer. It behaves essentially like the above function.
   //
   //  Note: Method has a "side effect". It does the change TQtWidget class object 
   //        to allow  the "mousePressEvent" signal emitting
   
   if (wid) {
      connect(wid,SIGNAL(RootEventProcessed(TObject *, unsigned int, TCanvas *))
           ,this,SLOT(RootEventProcessed(TObject *, unsigned int, TCanvas *)));
      wid->EnableSignalEvents(kMousePressEvent);
   }
}

//__________________________________________________________________________________
void TQtZoomPadWidget::Disconnect()
{
    // (Qt/ROOT) SLOT 
    // One should connect this slot to the ROOT TPad::Closed() SIGNAL
   hide();
   if (fSrcWidget) { fSrcWidget->SetAllBits(fMouseBits); fSrcWidget->ResetBit(kMouseMoveEvent);}
   fPad = 0; fSrcWidget = 0; fMouseBits = 0;
}

//__________________________________________________________________________________
void TQtZoomPadWidget::Disconnect(const char *canvasName)
{
    // (Qt/ROOT) SLOT 
    //
    // Disconnect TCavas with the give name "canvasName" to this zoomer
   if (canvasName && canvasName[0]) {
     TCanvas *canvas = (TCanvas *) gROOT->GetListOfCanvases()->FindObject(canvasName);
     if (canvas) Disconnect(canvas);
   }
}

//__________________________________________________________________________________
void TQtZoomPadWidget::Disconnect(TCanvas  *canvas)
{
    // (Qt/ROOT) SLOT 
    //
    // Disconnect TCavas "canvas" to this zoomer
    //
    // This is an overloaded member function, provided for convenience to connect the TCanvas
    // obejct "canvas" to this zoomer. It behaves essentially like the above function.
    if (canvas) {
      Int_t wid = canvas->GetCanvasID();
      Disconnect(wid);
    }
}

//__________________________________________________________________________________
void TQtZoomPadWidget::Disconnect(int  wid)
{
   // (Qt/ROOT) SLOT 
   //
   // wid - an index if the TCanvas implementation
   //
   // This is an overloaded member function, provided for convenience to connect the TCanvases
   //  to this zoomer. It behaves essentially like the above function.
   
   QPaintDevice *dev =  TGQt::iwid(wid);
   if (dev) {
      TQtWidget *widget = dynamic_cast<TQtWidget *>(dev);
      if (widget)  Disconnect(widget);
   }
}

//__________________________________________________________________________________
void TQtZoomPadWidget::Disconnect(TQtWidget  *wid)
{
   // (Qt/ROOT) SLOT 
   //
   // TQtWidget - Qt-widget that implement the ROOT TCanvas GUI interface 
   //
   // This is an overloaded member function, provided for convenience to connect the TQtWidget
   //  to this zoomer. It behaves essentially like the above function.
   //
   //  Note: Method has a "side effect". It does the change TQtWidget class object 
   //        to allow  the "mousePressEvent" signal emitting
   
   if (wid) {
      if ( fSrcWidget == wid ) Disconnect();
      disconnect(wid,SIGNAL(RootEventProcessed(TObject *, unsigned int, TCanvas *))
           ,this,SLOT(RootEventProcessed(TObject *, unsigned int, TCanvas *)));
   }
}
//__________________________________________________________________________________
void TQtZoomPadWidget::HideOnLeave(bool on)
{
    // Set the "hide on leave" flag
    fHideOnLeave = on; 
    DefineTooTip(fHideOnLeave);
}
//__________________________________________________________________________________
void TQtZoomPadWidget::PadModified(bool on)
{ 
   // (Qt/ROOT) SLOT 
   // One should connect this slot to the ROOT TPad::Modified(bool) SIGNAL
   if (on) {}
   fPad->Modified(); fPad->Update();
}

//__________________________________________________________________________________
void TQtZoomPadWidget::Resize(unsigned int w, unsigned int h)
{   
   // Resize the zoomer and update the TCanvas
   bool smartZoom = HasSmartZoom();
   SmartZoomEnable(false);     // disable the smart zooming
   resize(w,h);
   fCanvas->Resize();
   fOldWidth  = w;
   fOldHieght = h;
   SmartZoomEnable(smartZoom); // restore the smart zooming
}

//__________________________________________________________________________________
void TQtZoomPadWidget::SetPad(TVirtualPad *pad,bool tobeShown )
{
   // (Qt/ROOT) SLOT 
   // One should connect all desired TPad::Selected or TQtWidget signals 
   // to this single slot
   //
   if (fSetPadInProgress) return;
   fSetPadInProgress = true;
   if (pad && (fZoomFactor > 0) && (fPad != pad) && (pad->GetCanvas() != (TCanvas *)pad) ) {
      TVirtualPad *savePad = gPad;
      fCanvas->cd();
      setCaption(pad->GetName());
      fCanvas->Clear();
      if (savePad == fPad) savePad = 0;
      delete fPad;
      fPad = (TPad*)pad->Clone();
      
      //  Restore the mouse event bits
      if (fSrcWidget) { fSrcWidget->SetAllBits(fMouseBits); fSrcWidget->ResetBit(kMouseMoveEvent);}
      // Take the curret TQtWidget
      fSrcWidget = (TQtWidget *)TGQt::iwid(pad->GetCanvas()->GetCanvasID());
      // Save the widget mouse event bits
      fMouseBits = fSrcWidget->GetAllBits();
      fSrcWidget->EnableSignalEvents(kMouseMoveEvent);

      fCanvas->GetListOfPrimitives()->Add(fPad);
      fPad->SetPad(0,0,1,1);
      TObjLink *lnk = fPad->GetListOfPrimitives()->FirstLink();
      while (lnk) {
          TObject *o = lnk->GetObject();
          o->SetBit(kCanDelete);
          lnk = lnk->Next();
      }

      // Copy list of the primitivies
      if (fHideOnLeave && isHidden() ) {
         Resize( (unsigned int)(fZoomFactor*pad->UtoPixel(1))
               ,(unsigned int)(fZoomFactor*pad->VtoPixel(0)));
      }
      if (fHideOnLeave && fSrcWidget) {
         // Define the picture position
         QWidget *parent = fSrcWidget->parentWidget();
         TCanvas *padCanvas = fSrcWidget->GetCanvas();
         if (padCanvas) {
            QPoint padLocation(pad->UtoAbsPixel(0),pad->VtoAbsPixel(1));
            if (parent) padLocation = parent->mapToGlobal(padLocation);
            move(padLocation);
         }
      }
      fCanvas->Modified(); fCanvas->Update();
      if (savePad) savePad->cd();
      if (tobeShown) show();
   }
   fSetPadInProgress = false;

//    if (fPad) PadModified();

}
//__________________________________________________________________________________
void TQtZoomPadWidget::SetSelectingButton(EEventType  button)
{
   // SLOT
   // Set the mouse button to activate the zoomer for the FUTURE connection
   // This method doesn't change the established connection
   fSelectingButton = button;
}
//__________________________________________________________________________________
void TQtZoomPadWidget::Show()
{
   // QWidget::show method RootCint interface
   show();
}
//__________________________________________________________________________________
void TQtZoomPadWidget::Selected(TVirtualPad *pad, TObject *, Int_t event)
{
  // (Qt/ROOT) SLOT 
  // This is an overloaded member function, provided for convenience to receive the ROOT TPad::Selected signal.
  //  It behaves essentially like the above function.
   
  // fprintf(stderr," TQtZoomPadWidget::Selected %p %s\n",pad,pad->GetName());
  if (event == kButton2Down && !fSetPadInProgress) SetPad(pad);

}
//__________________________________________________________________________________
void TQtZoomPadWidget::RootEventProcessed(TObject *, unsigned int event, TCanvas *canvas)
{
  // (Qt/ROOT) SLOT 
  // This is an overloaded member function, provided for convenience to receive 
  // the Qt TQtWidget::RootEventProcessed signal.
  //  It behaves essentially like the above function.

   if (canvas) {
       TVirtualPad *pad = canvas->GetSelectedPad();
       if (pad)  {
          if (event == kMouseMotion) event = kButton2Down;
          Selected(pad,canvas->GetSelected(), event);
       }
   }
}

//__________________________________________________________________________________
void TQtZoomPadWidget::SetZoomFactor(float f)
{
   // Set the zoom factor. it should be positive
   fZoomFactor = f;
   if (fPad && fZoomFactor  > 0)
       Resize((unsigned int)(fZoomFactor*fPad->UtoPixel(1))
             ,(unsigned int)(fZoomFactor*fPad->VtoPixel(0)));
   emit zoomChanged(f);
}
//__________________________________________________________________________________
void TQtZoomPadWidget::DefineTooTip(bool hideOnLeave) 
{
   // Define the current tool tip for thew zoomer
   if (hideOnLeave) 
      QToolTip::add( this,"Click the middle mouse button to keep this widget on the top permanently");
   else
      QToolTip::add( this,"Click the middle mouse button to turn the \"hide on leave\" mode");
}
