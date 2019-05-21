TEMPLATE = app
TARGET = filters
QT += qml quick

# Input
SOURCES += main.cpp
RESOURCES += qml.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/filters
INSTALLS += target
