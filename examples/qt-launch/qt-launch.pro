TEMPLATE = app
TARGET = qt-launch
QT += multimedia multimediawidgets gstreamerextras

# Input
SOURCES = main.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/qt-launch
INSTALLS += target
