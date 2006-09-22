#ifndef QTROOT_CUSTOMIZECANVASMENU
#define QTROOT_CUSTOMIZECANVASMENU
// @(#)root/qt:$Name:  $:$Id: TQtCustomizeCanvasMenu.h,v 1.2 2006/09/22 17:27:10 fine Exp $
// Author: Valeri Fine   12/12/2005
/****************************************************************************
**
** Copyright (C) 2005 by Valeri Fine. BNL.  All rights reserved.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
*****************************************************************************/

#include <qevent.h>
#include <qobject.h>
#include <qpoint.h> 
#if QT_VERSION >= 0x40000
//MOC_SKIP_BEGIN
//Added by qt3to4:
#  include <Q3PopupMenu>
//MOC_SKIP_END
#endif /* QT_VERSION */

/////////////////////////////////////////////////////////////////////////////////////
//
// TQtCustomizeCanvasMenu class is a Qt event filter.
//
// It catches two events sent to the embedded TCanvas
//
//     QEvent::MouseButtonPress
//     QEvent::ContextMenu
//
//  and emit AboutToShow(QPopupMenu &contextMenu, TContextMenu *rootContextMenu) signal
//
//  This Qt signal can be connected to user's Qt slot to customize 
//  the ROOT Context menu if any
//
// Usage:
//   1. Instantiate and attach the event filter to the TCanvas or TQtWidget object:
//      TQtCustomizeCanvasMenu *eventFilter =
//                TQtCustomizeCanvas::installCustomMenu(tQtWidget1);
//
//      where   tQtWidget1 is a TQtWidget pointer
//
//  2. Connect the AboutToShow signal of the TQtCustomizeCanvasMenu object with your
//     Qt Slot
//     connect( eventFilter,SIGNAL(AboutToShow(QPopupMenu *,TContextMenu *))
//            , myMainSteeringObject,SLOT(CustomizeIt(QPopupMenu *,TContextMenu *)));
//
//  3. Provide the method to change the "standard" ROOT Context menu:
//       void CustomizeIt(QPopupMenu *contextMenu,TContextMenu *rootContyextMenu) 
//       {
//           // Second parameter is optional and may be disregarded.
//           // One can use to garther an extra information about the context
//           contextMenu->insertSeparator();
//           QPopupMenu *customMenu = new QPopupMenu();
//           contextMenu->insertItem("&AtlasDAQ",propertiesMenu);
//           propertiesMenu->insertItem("Idea to customize ROOT menu belongs Andrea Dotti");
//       }
//
//    See: QtRoot/qtExamples/CustomCanvasMenu example foer the working example.
//
/////////////////////////////////////////////////////////////////////////////////////

// ROOT classes:
class TContextMenu;
class TCanvas;

// Qt/ROOT classes:
class TQtWidget;
class TQtContextMenuImp;

// Qt classes:
#if QT_VERSION < 0x40000
  class QPopupMenu;
#else /* QT_VERSION */
//MOC_SKIP_BEGIN
  class Q3PopupMenu;
//MOC_SKIP_END
#endif /* QT_VERSION */

class TQtCustomizeCanvasMenu : public QObject {
Q_OBJECT
      
 private:
   TContextMenu  *fContextMenu;
   TQtWidget     *fCanvasWidget;
   QPoint         fPosition;
 protected:
   bool MakeContextMenu(TQtWidget *canvas);

 public:
    TQtCustomizeCanvasMenu() : fContextMenu(0), fCanvasWidget(0){;}
    virtual ~TQtCustomizeCanvasMenu();
    bool eventFilter( QObject *o, QEvent *e );
    static TQtCustomizeCanvasMenu *installCustomMenu(TQtWidget *canvas);
    static TQtCustomizeCanvasMenu *installCustomMenu(TCanvas *canvas);

signals:
#if QT_VERSION < 0x40000
    void AboutToShow(QPopupMenu *,TContextMenu *);
#else /* QT_VERSION */
//MOC_SKIP_BEGIN
    void AboutToShow(Q3PopupMenu *,TContextMenu *);
//MOC_SKIP_END
#endif /* QT_VERSION */
};

#endif
