TEMPLATE = app
TARGET = playbin
QT += qml quick multimedia widgets

SOURCES += main.cpp
RESOURCES += qml.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/playbin
INSTALLS += target
