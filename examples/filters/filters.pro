TEMPLATE = app
TARGET = filters
QT += qml quick

SOURCES += main.cpp
RESOURCES += qml.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/filters
INSTALLS += target
