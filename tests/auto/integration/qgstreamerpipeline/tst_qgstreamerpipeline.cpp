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

#include <QGstreamerPipeline>
#include <QMediaPlayer>
#include <QAbstractVideoSurface>
#include <QVideoWidget>
#include <QtTest/QtTest>

QT_USE_NAMESPACE

class tst_QGstreamerPipeline: public QObject
{
    Q_OBJECT
private slots:
    void testMediaPlayer();
    void testPipeline();
    void testPipelineSurface();
    void testPipelineWidget();
    void testSetProperty();

private:
    QMediaPlayer *mediaPlayer = nullptr;
    QGstreamerPipeline *control = nullptr;
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

void tst_QGstreamerPipeline::testMediaPlayer()
{
    QMediaPlayer mediaPlayer;
    QGstreamerPipeline p(&mediaPlayer);

    QVERIFY(!p.source());
    QCOMPARE(p.mediaPlayer(), &mediaPlayer);
}

void tst_QGstreamerPipeline::testPipeline()
{
    QMediaPlayer mediaPlayer;
    QGstreamerPipeline p(&mediaPlayer);
    QSignalSpy spy(&p, &QGstreamerPipeline::pipelineChanged);

    QString pl = "videotestsrc ! autovideosink";
    p.setPipeline(pl);
    QTRY_COMPARE(spy.count(), 1);
    QCOMPARE(mediaPlayer.error(), QMediaPlayer::NoError);
    QCOMPARE(p.pipelineDesc(), pl);
    QVERIFY(p.pipeline());
}

void tst_QGstreamerPipeline::testPipelineSurface()
{
    QMediaPlayer mediaPlayer;
    Surface *s = new Surface(this);
    mediaPlayer.setVideoOutput(s);

    QGstreamerPipeline p(&mediaPlayer);
    QSignalSpy spy(&p, &QGstreamerPipeline::pipelineChanged);

    QString pl = "videotestsrc ! xvimagesink name=qtvideosink";
    p.setPipeline(pl);
    mediaPlayer.play();

    QTRY_COMPARE(spy.count(), 1);
    QCOMPARE(mediaPlayer.error(), QMediaPlayer::NoError);
    QCOMPARE(p.pipelineDesc(), pl);
    QVERIFY(p.pipeline());
}

void tst_QGstreamerPipeline::testPipelineWidget()
{
    QMediaPlayer mediaPlayer;
    QVideoWidget w;
    mediaPlayer.setVideoOutput(&w);

    QGstreamerPipeline p(&mediaPlayer);
    QSignalSpy spy(&p, &QGstreamerPipeline::pipelineChanged);

    QString pl = "videotestsrc ! xvimagesink name=qtvideosink";
    p.setPipeline(pl);
    mediaPlayer.play();

    QTRY_COMPARE(spy.count(), 1);
    QCOMPARE(mediaPlayer.error(), QMediaPlayer::NoError);
    QCOMPARE(p.pipelineDesc(), pl);
    QVERIFY(p.pipeline());
}

void tst_QGstreamerPipeline::testSetProperty()
{
    QMediaPlayer mediaPlayer;
    QGstreamerPipeline p(&mediaPlayer);
    p.setPipeline(QLatin1String("videotestsrc name=src ! autovideosink"));

    QVariantMap m;
    m["name"] = "new_name";
    QVERIFY(p.set("src", m));
    QCOMPARE(mediaPlayer.error(), QMediaPlayer::NoError);
}

QTEST_MAIN(tst_QGstreamerPipeline)
#include "tst_qgstreamerpipeline.moc"
