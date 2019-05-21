/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtCore/QCommandLineParser>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QCoreApplication::setApplicationName("QtMultimedia GStreamer Playbin Example");
    QCoreApplication::setOrganizationName("QtProject");
    QGuiApplication::setApplicationDisplayName(QCoreApplication::applicationName());
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption uriOpt("uri", "URI of the media to play.", "uri");
    parser.addOption(uriOpt);

    QCommandLineOption videoSinkOpt("video-sink", "The video output element to use.", "video-sink");
    parser.addOption(videoSinkOpt);

    QCommandLineOption showTextOpt("show-text", "Enable a subtitle.");
    parser.addOption(showTextOpt);

    QCommandLineOption textUriOpt("text-uri", "The currently playing URI of a subtitle.", "text-uri");
    parser.addOption(textUriOpt);

    QCommandLineOption fontOpt("text-font", "Pango font description of font to be used for subtitle rendering.", "text-font");
    parser.addOption(fontOpt);

    QCommandLineOption audioOpt("audio-stream", "Currently playing audio stream.", "audio-stream");
    parser.addOption(audioOpt);

    QCommandLineOption videoOpt("video-stream", "Currently playing video stream.", "video-stream");
    parser.addOption(videoOpt);

    QCommandLineOption textOpt("text-stream", "Currently playing text stream.", "text-stream");
    parser.addOption(textOpt);

    parser.process(app);

    QString uri = parser.isSet(uriOpt) ? parser.value(uriOpt) : QString();
    QString videoSink = parser.isSet(videoSinkOpt) ? parser.value(videoSinkOpt) : QString();
    QString textUri = parser.isSet(textUriOpt) ? parser.value(textUriOpt) : QString();
    QString textFont = parser.isSet(fontOpt) ? parser.value(fontOpt) : QString();
    QString audioStream = parser.isSet(audioOpt) ? parser.value(audioOpt) : QString();
    QString videoStream = parser.isSet(videoOpt) ? parser.value(videoOpt) : QString();
    QString textStream = parser.isSet(textOpt) ? parser.value(textOpt) : QString();

    QQuickView viewer;
    viewer.setSource(QUrl("qrc:/main.qml"));
    QObject::connect(viewer.engine(), &QQmlEngine::quit, &viewer, &QQuickView::close);
    QQuickItem *rootObject = viewer.rootObject();

    if (!uri.isEmpty())
        rootObject->setProperty("uri", uri);

    if (!videoSink.isEmpty())
        rootObject->setProperty("videoSink", videoSink);

    if (parser.isSet(showTextOpt))
        rootObject->setProperty("showText", true);

    if (!textUri.isEmpty())
        rootObject->setProperty("textUri", textUri);

    if (!textFont.isEmpty())
        rootObject->setProperty("textFont", textFont);

    if (!audioStream.isEmpty())
        rootObject->setProperty("audioStream", audioStream);

    if (!videoStream.isEmpty())
        rootObject->setProperty("videoStream", videoStream);

    if (!textStream.isEmpty())
        rootObject->setProperty("textStream", textStream);

    QMetaObject::invokeMethod(rootObject, "init");

    viewer.setMinimumSize(QSize(640, 360));
    viewer.show();

    return app.exec();
}
