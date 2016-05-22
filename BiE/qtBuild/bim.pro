TEMPLATE = app
TARGET = BIM

INCLUDEPATH += $$PWD
INCLUDEPATH += ../source
INCLUDEPATH += ../header
INCLUDEPATH += ../../external/Exiv2/include

QT += widgets

QMAKE_CXXFLAGS += $$(CXXFLAGS)
QMAKE_CFLAGS += $$(CFLAGS)
QMAKE_LFLAGS += $$(LDFLAGS)

# Input
HEADERS += \
    ../header/BiManagerWindow.h \
    ../header/BookmarksTree.h \
    ../header/CropToolWidget.h \
    ../header/ViewDialogs.h \
    ../header/ThumbnailView.h \
    ../header/ImageViewer.h \
    ../header/BimModel.h \
    ../header/InfoViewer.h
SOURCES += \
    ../source/main.cpp \
    ../source/BiManagerWindow.cpp \
    ../source/BookmarksTree.cpp \
    ../source/CropToolWidget.cpp \
    ../source/ViewDialogs.cpp \
    ../source/ThumbnailView.cpp \
    ../source/ImageViewer.cpp \
    ../source/BimModel.cpp \
    ../source/InfoViewer.cpp

RESOURCES += bim.qrc

target.path = /usr/bin/

icon.files = ../images/bim.png
icon.path = /usr/share/icons/hicolor/48x48/apps

icon16.files = ../images/icon16/bim.png
icon16.path = /usr/share/icons/hicolor/16x16/apps

iconPixmaps.files = ../images/icon16/bim.png
iconPixmaps.path = /usr/share/pixmaps

desktop.files = bim.desktop
desktop.path = /usr/share/applications

INSTALLS += target icon icon16 iconPixmaps desktop

TRANSLATIONS = 	../translations/bim_en.ts \
                ../translations/bim_pl.ts \
                ../translations/bim_de.ts \
                ../translations/bim_ru.ts \
                ../translations/bim_cs.ts \
                ../translations/bim_fr.ts

INCLUDEPATH += $$PWD/../../external/
DEPENDPATH += $$PWD/../../external/

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../external/Exiv2/lib -lexiv2s
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../external/Exiv2/lib -lexiv2sd

# libreary exiv2
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../external/Exiv2/lib -lexiv2s
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../external/Exiv2/lib -lexiv2sd

# libreary libexpat
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../external/Exiv2/lib -llibexpat
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../external/Exiv2/lib -llibexpatd

# libreary Other
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../external/Exiv2/lib -lxmpsdk
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../external/Exiv2/lib -lxmpsdkd

# libreary Other
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../external/Exiv2/lib -lzlib1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../external/Exiv2/lib -lzlib1d
