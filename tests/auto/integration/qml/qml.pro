TEMPLATE = app
TARGET = tst_qml
CONFIG += qmltestcase
SOURCES += tst_qml.cpp

importFiles.files = pipeline playbin

importFiles.path = .
DEPLOYMENT += importFiles
