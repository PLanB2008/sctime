# File generated by kdevelop's qmake manager.
# -------------------------------------------
# Subdir relative project main directory: ./src
# Target is an application:  ../bin/sctime

TEMPLATE = app
VERSIONSTR = '"0.61"'
CONFIG += warn_on qt
CONFIG += uic debug
OBJECTS_DIR = ../obj
QT += xml sql gui core
TARGET = ../bin/sctime
!win32{
  BUILDDATESTR = '"`date`"'
}
isEmpty(BUILDDATESTR){
  BUILDDATESTR = '"unknown"'
}
#VERSIONSTR="\"0.54-pre\""
#BUILDDATESTR="\"unknown\""
DEFINES += BUILDDATESTR=$$BUILDDATESTR VERSIONSTR=$$VERSIONSTR
SOURCES += abteilungsliste.cpp \
           datedialog.cpp \
           unterkontodialog.cpp \
           kontotreeview.cpp \
           findkontodialog.cpp \
           timemainwindow.cpp \
           sctimexmlsettings.cpp \
           sctime.cpp \
           preferencedialog.cpp \
           defaultcommentreader.cpp \
           qdatepicker.cpp \
           utils.cpp \
           defaulttagreader.cpp \
           kontodateninfozeit.cpp \
           kontotreeitem.cpp \
           GetOpt.cpp \
           descdata.cpp \
           smallfontdialog.cpp \
           bereitschaftsdateninfozeit.cpp \
           bereitschaftsliste.cpp \
           bereitschaftsview.cpp \
           bereitschaftsmodel.cpp \
           colorchooser.cpp
HEADERS += abteilungsliste.h \
           kontoliste.h \
           statusbar.h \
           datedialog.h \
           kontotreeitem.h \
           timecounter.h \
           eintragsliste.h \
           timeedit.h \
           errorapp.h \
           kontotreeview.h \
           findkontodialog.h \
           timemainwindow.h \
           globals.h \
           toolbar.h \
           sctimeapp.h \
           unterkontodialog.h \
           kontodateninfo.h \
           sctimehelp.h \
           unterkontoeintrag.h \
           sctimexmlsettings.h \
           unterkontoliste.h \
           preferencedialog.h \
           defaultcommentreader.h \
           utils.h \
           qdatepicker.h \
           defaulttagreader.h \
           kontodateninfozeit.h \
           GetOpt.h \
           descdata.h \
           smallfontdialog.h \
           bereitschaftsdateninfo.h \
           bereitschaftsdateninfozeit.h \
           bereitschaftsliste.h \
           bereitschaftsview.h \
           bereitschaftsmodel.h \
           colorchooser.h
RESOURCES= ../pics/sctimeImages.qrc					 
#IMAGES += ../pics/hi16_action_apply.xpm \
#          ../pics/hi22_action_1downarrow.xpm \
#          ../pics/hi22_action_1uparrow.xpm \
#          ../pics/hi22_action_2downarrow.xpm \
#          ../pics/hi22_action_2uparrow.xpm \
#          ../pics/hi22_action_attach.xpm \
#          ../pics/hi22_action_edit.xpm \
#          ../pics/hi22_action_filesave.xpm \
#          ../pics/hi22_action_player_pause_half.xpm \
#          ../pics/hi22_action_player_pause.xpm \
#          ../pics/hi22_action_queue.xpm \
#          ../pics/scLogo_15Farben.xpm \
#          ../pics/sc_logo.xpm
FORMS += datedialogbase.ui \
         preferencedialogbase.ui \
         colorchooserbase.ui
target.path = /$(prefix)/bin
INSTALLS += target
linux-g++-static{
  LIBS += -ldl
}
linux-g++-static-debug{
  LIBS += -ldl
}
linux-g++{
  LIBS += -ldl
}
win32{
  DEFINES += WIN32
  QMAKE_CXXFLAGS += -GX
  SOURCES += kontodateninfodatabase.cpp bereitschaftsdateninfodatabase.cpp DBConnector.cpp
  HEADERS += kontodateninfodatabase.h bereitschaftsdateninfodatabase.h DBConnector.h
  RC_FILE += sctime.rc
}
hpux-acc{
  DEFINES += HPUX
  LIBS += -L/opt/graphics/OpenGL/lib $$QMAKE_LIBS_OPENGL
}
solaris-cc{
  DEFINES += SUN
  QMAKE_CXXFLAGS_RELEASE += -features=conststrings
  LIBS += -ldl $$QMAKE_LIBS_OPENGL
}
solaris-g++{
  DEFINES += SUN
  LIBS += -ldl
}
irix-cc{
  DEFINES += IRIX
  QMAKE_LIBS_QT += -lGL
}
irix-g++{
  DEFINES += IRIX
}
mac {
  ICON = ../pics/scTime.icns
  QMAKE_INFO_PLIST = ../extra/mac/Info.plist
  TARGET = ../bin/scTime
  QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.5.sdk
  DEFINES += MACOS
  CONFIG += x86 ppc
}
#LIBS+=-L../../gdbmacros -lgdbmacros
