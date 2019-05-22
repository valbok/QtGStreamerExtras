TEMPLATE = app
TARGET = receiver
INCLUDEPATH += .
QT += qml quick multimedia widgets

SOURCES += main.cpp
RESOURCES += qml.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/streamer/receiver
INSTALLS += target
