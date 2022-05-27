QT += widgets core
#QT += widgets core KMime
linux:CONFIG += debug_and_release debug_and_release_target qt x11 build_all c++14 
#INCLUDEPATH += /usr/local/lib/
INCLUDEPATH += /usr/local/include/
#INCLUDEPATH += /usr/include/mimetic/
INCLUDEPATH += /usr/local/include/vmime/
VPATH += /usr/local/lib/
#VPATH += /usr/local/include/
FORMS = wombatmail.ui tagmanager.ui htmlviewer.ui aboutbox.ui
HEADERS = wombatmail.h tagmanager.h htmlviewer.h aboutbox.h cssstrings.h
SOURCES = main.cpp wombatmail.cpp tagmanager.cpp htmlviewer.cpp aboutbox.cpp cssstrings.cpp
RESOURCES += wombatmail.qrc
release: DESTDIR = release
debug:   DESTDIR = debug
linux:LIBS = -lz -lmimetic libpff.a -licuuc -lgsasl -lgnutls libvmime.a

if(!debug_and_release|build_pass):CONFIG(debug, debug|release) {
}
#linux:
#install
target.path = /usr/local/bin
#target.path = ./
INSTALLS += target
