TEMPLATE = app
CONFIG += warn_on qt uic
# you have to explicitly recompile sctime.cpp whenever you change this value
VERSION = 0.73.6
DEFINES += APP_VERSION=$$VERSION
QT += xml gui core network sql
TARGET = sctime
CODECFORTR= UTF-8
TRANSLATIONS = sctime_de.ts
SOURCES = abteilungsliste.cpp bereitschaftsliste.cpp bereitschaftsmodel.cpp bereitschaftsview.cpp datasource.cpp datedialog.cpp defaultcommentreader.cpp defaulttagreader.cpp descdata.cpp findkontodialog.cpp GetOpt.cpp kontotreeitem.cpp kontotreeview.cpp lock.cpp preferencedialog.cpp qdatepicker.cpp sctime.cpp sctimexmlsettings.cpp setupdsm.cpp timemainwindow.cpp unterkontodialog.cpp
HEADERS = abteilungsliste.h bereitschaftsliste.h bereitschaftsmodel.h bereitschaftsview.h datasource.h datedialog.h defaultcommentreader.h defaulttagreader.h descdata.h eintragsliste.h findkontodialog.h GetOpt.h globals.h kontodateninfo.h kontoliste.h kontotreeitem.h kontotreeview.h lock.h preferencedialog.h qcalendarsystem.h qdatepicker.h sctimexmlsettings.h setupdsm.h statusbar.h timecounter.h timeedit.h timemainwindow.h unterkontodialog.h unterkontoeintrag.h unterkontoliste.h
RESOURCES = ../pics/sctimeImages.qrc ../help/help.qrc
GENERATED_RESOURCES = translations.qrc
FORMS = datedialogbase.ui preferencedialogbase.ui

# just tell qmake that qrc_generated_translations.cpp depends on all
# translations qm files - yes, this is somewhat bulky
qrc_generated_translations_cpp.target = qrc_generated_translations.cpp
qrc_generated_translations_cpp.depends = $$replace(TRANSLATIONS, ".ts", ".qm")
QMAKE_EXTRA_TARGETS = qrc_generated_translations_cpp

# add a custom compiler that copies resource files that depend on generated
# files into the build dir so that the generated files can be included from
# there.
copytobuilddir.input = GENERATED_RESOURCES
copytobuilddir.output = generated_${QMAKE_FILE_BASE}.qrc
copytobuilddir.commands = $$QMAKE_COPY_FILE "${QMAKE_FILE_IN}" "${QMAKE_FILE_OUT}"
copytobuilddir.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += copytobuilddir

# add a custom resource compiler that expects the resource files inside the
# build dir. This way they can include files generated by the build which is
# used for translation files that are lreleased automatically as part of the
# build process.
defined(qtPrepareTool)|load(qt_functions)
qtPrepareTool(QMAKE_RCC, rcc)
for(resource, GENERATED_RESOURCES) {
	# file names in build dir *have* to differ from file names in source
	# dir. Otherwise generatedrcc.input below will find the source dir
	# files and put them into the make rules instead of the generated files
	# in the build dir. So just prepent generated_ to make file names
	# differ. Unfortunately this also causes a warning for earch file:
	# WARNING: Failure to find: generated_translations.qrc
	GENERATED_RESOURCES_IN_BUILD_DIR += generated_$$basename(resource)
}
generatedrcc.input = GENERATED_RESOURCES_IN_BUILD_DIR
generatedrcc.output = qrc_${QMAKE_FILE_BASE}.cpp
generatedrcc.commands = "$$QMAKE_RCC" -name ${QMAKE_FILE_IN_BASE} "${QMAKE_FILE_IN_BASE}.qrc" -o "${QMAKE_FILE_OUT}"
generatedrcc.variable_out = GENERATED_SOURCES
QMAKE_EXTRA_COMPILERS += generatedrcc

# add a custom compiler that creates .qm files from .ts translation files
qtPrepareTool(QMAKE_LRELEASE, lrelease)
lrelease.input = TRANSLATIONS
lrelease.output = ${QMAKE_FILE_BASE}.qm
lrelease.commands = "$$QMAKE_LRELEASE" "${QMAKE_FILE_IN}" -qm "${QMAKE_FILE_OUT}"
lrelease.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += lrelease

win32{
  CONFIG += embed_manifest_exe
  #QMAKE_CXXFLAGS += -EHsc # C++-Ausnahmen
  RC_FILE += sctime.rc
  DEFINES += _CRT_SECURE_NO_WARNINGS
  }
!win32{
  SOURCES += unix/signalhandler.cpp
  HEADERS += unix/signalhandler.h
}
hpux-acc{
  LIBS += -L/opt/graphics/OpenGL/lib $$QMAKE_LIBS_OPENGL
}
solaris-cc{
  QMAKE_CXXFLAGS_RELEASE += -features=conststrings
  LIBS += -ldl $$QMAKE_LIBS_OPENGL
}
solaris-g++{
  LIBS += -ldl
}
irix-cc{
  QMAKE_LIBS_QT += -lGL
}
mac {
  # with Qt-4.8.0 icon and Info.plist rules do not work with out-of-source
  # builds. try again later... sctime-mac-dist does this for now
  #ICON = $$PWD/../pics/scTime.icns
  #QMAKE_INFO_PLIST = $$PWD/../extra/mac/Info.plist
  TARGET = scTime
  QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.5.sdk
  DEFINES += MACOS
  CONFIG += x86_64 x86 ppc
}
