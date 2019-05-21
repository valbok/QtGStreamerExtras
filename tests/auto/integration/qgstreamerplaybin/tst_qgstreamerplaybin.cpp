/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QGstreamerPlaybin>
#include <QMediaPlayer>
#include <QAbstractVideoSurface>
#include <QtTest/QtTest>

QT_USE_NAMESPACE

class tst_QGstreamerPlaybin: public QObject
{
    Q_OBJECT

public slots:
    void init();
    void cleanup();

private slots:
    void testVideoSink();
    void testShowText();
    void testTextUri();
    void testTextFont();
    void testAudioStreams();
    void testVideoStreams();
    void testTextStreams();
    void testSetTextStream();
    void testTextStreamProperties();
    void testTextUriStreamProperties();

private:
    QMediaPlayer *m_mediaPlayer = nullptr;
    QGstreamerPlaybin *m_gst = nullptr;
};

struct Surface : QAbstractVideoSurface
{
    Surface(QObject *parent = nullptr)
        : QAbstractVideoSurface(parent)
    {
    }

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
        QAbstractVideoBuffer::HandleType) const override
    {
        return QList<QVideoFrame::PixelFormat>() << QVideoFrame::Format_ARGB32;
    }

    bool isFormatSupported(const QVideoSurfaceFormat &) const override { return true; }
    bool start(const QVideoSurfaceFormat &) override { return true; }
    void stop() override { }
    bool present(const QVideoFrame &) override { return true; }
};

void tst_QGstreamerPlaybin::init()
{
    m_mediaPlayer = new QMediaPlayer;
    m_mediaPlayer->setMedia(QUrl("qrc:/testdata/colors.mp4"));
    m_gst = new QGstreamerPlaybin(m_mediaPlayer, m_mediaPlayer);
    m_mediaPlayer->setVideoOutput(new Surface(m_mediaPlayer));
}

void tst_QGstreamerPlaybin::cleanup()
{
    delete m_mediaPlayer;
}

void tst_QGstreamerPlaybin::testVideoSink()
{
    QSignalSpy spy(m_gst, &QGstreamerPlaybin::videoSinkChanged);

    QVERIFY(m_gst->videoSink().isEmpty());
    QString sink = "xvimagesink hue=100";
    m_gst->setVideoSink(sink);
    QTRY_COMPARE(spy.count(), 1);
    QCOMPARE(sink, m_gst->videoSink());
}

void tst_QGstreamerPlaybin::testShowText()
{
    QSignalSpy spy(m_gst, &QGstreamerPlaybin::showTextChanged);

    QVERIFY(!m_gst->showText());
    m_gst->setShowText(true);
    QVERIFY(m_gst->showText());
    QCOMPARE(spy.count(), 1);
    m_gst->setShowText(true);
    QVERIFY(m_gst->showText());
    QCOMPARE(spy.count(), 1);
    m_gst->setShowText(false);
    QVERIFY(!m_gst->showText());
    QCOMPARE(spy.count(), 2);
}

void tst_QGstreamerPlaybin::testTextUri()
{
    QSignalSpy spy(m_gst, &QGstreamerPlaybin::textUriChanged);

    QCOMPARE(m_gst->textUri(), QUrl());
    QUrl text("text.srt");
    m_gst->setTextUri(text);
    QVERIFY(m_gst->showText());
    QCOMPARE(m_gst->textUri(), text);
    QCOMPARE(spy.count(), 1);
}

void tst_QGstreamerPlaybin::testTextFont()
{
    QSignalSpy spy(m_gst, &QGstreamerPlaybin::textFontChanged);

    QCOMPARE(m_gst->textFont(), QString());
    QString font = "Sans, 18";
    m_gst->setTextFont(font);
    QCOMPARE(m_gst->textFont(), font);
    QCOMPARE(spy.count(), 1);
}

void tst_QGstreamerPlaybin::testAudioStreams()
{
    QSignalSpy spy(m_gst, &QGstreamerPlaybin::audioStreamsCountChanged);

    QCOMPARE(m_gst->audioStreamsCount(), 0);
    QTRY_VERIFY(spy.count() > 0);
    QCOMPARE(m_gst->audioStreamsCount(), 1);
}

void tst_QGstreamerPlaybin::testVideoStreams()
{
    QSignalSpy spy(m_gst, &QGstreamerPlaybin::videoStreamsCountChanged);

    QCOMPARE(m_gst->videoStreamsCount(), 0);
    QTRY_VERIFY(spy.count() > 0);
    QCOMPARE(m_gst->videoStreamsCount(), 1);
}

void tst_QGstreamerPlaybin::testTextStreams()
{
    QSignalSpy spy(m_gst, &QGstreamerPlaybin::textStreamsCountChanged);

    QCOMPARE(m_gst->textStreamsCount(), 0);
    QTRY_VERIFY(spy.count() > 0);
    QCOMPARE(m_gst->textStreamsCount(), 2);
}

void tst_QGstreamerPlaybin::testSetTextStream()
{
    QSignalSpy spy(m_gst, &QGstreamerPlaybin::textStreamChanged);

    QCOMPARE(m_gst->textStream(), -1);
    QTRY_COMPARE(m_gst->textStreamsCount(), 2);
    m_gst->setTextStream(0);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(m_gst->textStream(), 0);
    m_gst->setTextStream(1);
    QCOMPARE(spy.count(), 2);
    QTRY_COMPARE(m_gst->textStream(), 1);
}

void tst_QGstreamerPlaybin::testTextStreamProperties()
{
    QSignalSpy spy(m_gst, &QGstreamerPlaybin::textStreamPropertiesChanged);

    QCOMPARE(m_gst->textStreamsCount(), 0);
    QTRY_COMPARE(m_gst->textStreamsCount(), 2);
    QTRY_COMPARE(m_gst->textStreamProperties(0).size(), 1);
    QCOMPARE(m_gst->textStreamProperties(0)["Language"], QVariant("en"));
    QCOMPARE(m_gst->textStreamProperties(1).size(), 1);
    QCOMPARE(m_gst->textStreamProperties(1)["Language"], QVariant("no"));
}

void tst_QGstreamerPlaybin::testTextUriStreamProperties()
{
    QSignalSpy spy(m_gst, &QGstreamerPlaybin::textStreamPropertiesChanged);

    m_mediaPlayer->setMedia(QUrl());
    m_gst->setTextUri(QUrl::fromLocalFile(QFINDTESTDATA("testdata/text.srt")));
    m_mediaPlayer->setMedia(QUrl::fromLocalFile(QFINDTESTDATA("testdata/colors.mp4")));

    QTRY_COMPARE(m_gst->textStreamsCount(), 3);
    QCOMPARE(m_gst->textStreamProperties(0).size(), 0);
    QTRY_COMPARE(m_gst->textStreamProperties(1).size(), 1);
    QCOMPARE(m_gst->textStreamProperties(1)["Language"], QVariant("en"));
    QTRY_COMPARE(m_gst->textStreamProperties(2).size(), 1);
    QCOMPARE(m_gst->textStreamProperties(2)["Language"], QVariant("no"));
}

QTEST_GUILESS_MAIN(tst_QGstreamerPlaybin)
#include "tst_qgstreamerplaybin.moc"
