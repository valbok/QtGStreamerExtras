TARGET = QtGstreamerExtras

QT = multimediagsttools_private

PUBLIC_HEADERS += \
    qgstreamerpipeline.h \
    qgstreamerplaybin.h

PRIVATE_HEADERS += \
    qgstreamerpipeline_p.h \
    qgstreamerappsrc_p.h

SOURCES += \
    qgstreamerplaybin.cpp \
    qgstreamerpipeline.cpp \
    qgstreamerappsrc.cpp

CONFIG += simd optimize_full

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

load(qt_module)
