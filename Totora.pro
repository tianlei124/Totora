QT      +=  network widgets
SOURCES =   main.cpp \
    customsearchs.cpp \
    localsearchwindow.cpp \
    searchwindow.cpp
HEADERS  = \
    customsearchs.h \
    searchwindow.h \
    localsearchwindow.h

RESOURCES += \
    resources.qrc

FORMS += \
    customsearchs.ui \
    searchwindow.ui

include(QHotkey/qhotkey.pri)
DEPENDPATH  += QHotkey/QHotkey
INCLUDEPATH += QHotkey/QHotkey
