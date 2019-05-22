TEMPLATE = app
TARGET = streamer
INCLUDEPATH += .
QT += qml quick multimedia widgets gstreamerextras-private

HEADERS = streamer.h
SOURCES += main.cpp streamer.cpp
RESOURCES += qml.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/streamer/streamer
INSTALLS += target
