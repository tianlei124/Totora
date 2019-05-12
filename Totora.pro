QT      +=  network widgets
SOURCES =   main.cpp \
    customsearchs.cpp \
    searchwindow.cpp
HEADERS  = \
    customsearchs.h \
    searchwindow.h

RESOURCES += \
    resources.qrc

FORMS += \
    customsearchs.ui \
    searchwindow.ui

include(vendor/vendor.pri)
