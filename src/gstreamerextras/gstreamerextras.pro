TARGET = QtGStreamerExtras

QT = multimediagsttools_private

PUBLIC_HEADERS += \
    qgstpipeline.h \
    qgstplaybin.h

PRIVATE_HEADERS += \
    qgstpipeline_p.h \
    qgstvideoappsrc_p.h

SOURCES += \
    qgstplaybin.cpp \
    qgstpipeline.cpp \
    qgstvideoappsrc.cpp

CONFIG += simd optimize_full

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

load(qt_module)
