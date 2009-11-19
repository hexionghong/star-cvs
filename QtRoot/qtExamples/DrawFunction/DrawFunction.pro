# #####################################################################
# Automatically generated by qmake (1.07a) Fri Sep 7 09:16:46 2007
# #####################################################################
TEMPLATE = app
QT += xml opengl
    
unix: QMAKE_RPATH =

# Input
HEADERS += TQtFunViewer.h TGLFun.h\
           mywidget.h
           
SOURCES += TQtFunViewer.cxx  TGLFun.cxx\
           main.cxx   \
           mywidget.cxx
                                          
incFile = $(QTROOTSYSDIR)/include/rootcint.pri
exists ($$incFile):include ($$incFile)
!exists ($$incFile) { 
    incFile = $(ROOTSYS)/include/rootcint.pri
    exists ($$incFile):include ($$incFile)
    !exists ($$incFile) { 
        message (" ")
        message ("WARNING: The $$inlcudeFile was not found !!!")
        message ("Please update your Qt layer version from http://root.bnl.gov ")
        message (" ")
        LIBS += $$system(root-config --glibs) \
            -lGQt
        INCLUDEPATH += $(ROOTSYS)/include
    }
}

LIB_NAME = QGLViewer

# take in account the new QGLViewer.pro rules

CONFIG(debug, debug|release) {
     unix: LIB_NAME = $${LIB_NAME}
     else: LIB_NAME = $${LIB_NAME}
#     unix: LIB_NAME = $${LIB_NAME}_debug
#     else: LIB_NAME = d$${LIB_NAME}
}

win32:  LIBS += libMathCore.lib $${LIB_NAME}.lib
unix:   LIBS += -l$${LIB_NAME}

FORMS += mywidget.ui
