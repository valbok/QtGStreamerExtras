CXX_MODULE = gstreamerextras
TARGET  = declarative_gstreamer
TARGETPATH = QtGStreamerExtras

QT += gstreamerextras qtmultimediaquicktools-private multimedia-private quick-private

HEADERS += \
        qtransparentrectangle_p.h

SOURCES += \
        gstreamerextras.cpp \
        qtransparentrectangle.cpp

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0

load(qml_plugin)
