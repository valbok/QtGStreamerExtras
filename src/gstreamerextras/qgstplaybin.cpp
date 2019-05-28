/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgstplaybin.h"
#include "qgstpipeline_p.h"
#include <private/qgstreamervideorendererinterface_p.h>
#include <QtMultimedia/qmediametadata.h>

QT_BEGIN_NAMESPACE

typedef enum {
    GST_PLAY_FLAG_VIDEO         = 0x00000001,
    GST_PLAY_FLAG_AUDIO         = 0x00000002,
    GST_PLAY_FLAG_TEXT          = 0x00000004,
    GST_PLAY_FLAG_VIS           = 0x00000008,
    GST_PLAY_FLAG_SOFT_VOLUME   = 0x00000010,
    GST_PLAY_FLAG_NATIVE_AUDIO  = 0x00000020,
    GST_PLAY_FLAG_NATIVE_VIDEO  = 0x00000040,
    GST_PLAY_FLAG_DOWNLOAD      = 0x00000080,
    GST_PLAY_FLAG_BUFFERING     = 0x000000100
} GstPlayFlags;

class QGstPlaybinPrivate : public QGstPipelinePrivate
{
    Q_DECLARE_PUBLIC(QGstPlaybin)

public:
    QGstPlaybinPrivate(QGstPlaybin *q)
        : QGstPipelinePrivate(q)
    {
    }

    void streamsChanged();
    void mediaPlayerChanged();
    void updatePlaybin();

private:
    QString videoSinkDesc;
    QString pendingVideoSinkDesc;

    bool showText = false;
    bool pendingShowText = showText;
    QUrl textUri;
    QUrl pendingTextUri;
    QString textFont;
    QString pendingTextFont;
    int audioStreamsCount = 0;
    int pendingAudioStream = -1;
    int audioStream = -1;
    QList<QVariantMap> audioStreamProperties;
    int videoStreamsCount = 0;
    int pendingVideoStream = -1;
    int videoStream = -1;
    QList<QVariantMap> videoStreamProperties;
    int textStreamsCount = 0;
    int pendingTextStream = -1;
    int textStream = -1;
    QList<QVariantMap> textStreamProperties;
};

void QGstPlaybinPrivate::mediaPlayerChanged()
{
    Q_Q(QGstPlaybin);

    if (!isReady())
        return;

    QObject::connect(session, &QGstreamerPlayerSession::streamsChanged,
        q, [this, q]() {
            streamsChanged();
            q->setAudioStream(pendingAudioStream);
            q->setVideoStream(pendingVideoStream);
            q->setTextStream(pendingTextStream);
        });
    QObject::connect(session, &QGstreamerPlayerSession::rendererChanged,
        q, [this]() { updatePlaybin(); });

    // In case if these values've been applied
    // before mediaObject is available.
    q->setShowText(pendingShowText);
    q->setTextUri(pendingTextUri);
    q->setTextFont(pendingTextFont);
    q->setAudioStream(pendingAudioStream);
    q->setVideoStream(pendingVideoStream);
    q->setTextStream(pendingTextStream);
}

static GstElement *parseDesc(const QString &name)
{
    GError *error = NULL;
    GstElement *element = gst_parse_launch(name.toLatin1().constData(), &error);
    if (error) {
        g_printerr("ERROR: %s: %s\n", name.toLatin1().constData(), GST_STR_NULL(error->message));
        g_clear_error(&error);
    }

    return element;
}

void QGstPlaybinPrivate::updatePlaybin()
{
    Q_Q(QGstPlaybin);

    auto r = renderer();
    if (!r || !r->isReady() || pendingVideoSinkDesc.isEmpty())
        return;

    GstElement *videoSink = parseDesc(pendingVideoSinkDesc);
    if (videoSink) {
        videoSinkDesc = pendingVideoSinkDesc;
        pendingVideoSinkDesc.clear();
        r->setVideoSink(videoSink);

        emit q->videoSinkChanged();
    }
}

static QVariantMap getStreamProperties(GstElement *pipeline, const char *name, int i)
{
    QVariantMap streamProperties;
    GstTagList *tags = 0;
    g_signal_emit_by_name(G_OBJECT(pipeline), name, i, &tags);

#if GST_CHECK_VERSION(1,0,0)
    if (tags && GST_IS_TAG_LIST(tags)) {
#else
    if (tags && gst_is_tag_list(tags)) {
#endif
        gchar *languageCode = 0;
        if (gst_tag_list_get_string(tags, GST_TAG_LANGUAGE_CODE, &languageCode))
            streamProperties[QMediaMetaData::Language] = QString::fromUtf8(languageCode);

        g_free (languageCode);
        gst_tag_list_free(tags);
    }

    return streamProperties;
}

void QGstPlaybinPrivate::streamsChanged()
{
    Q_Q(QGstPlaybin);

    GstElement *playbin = q->pipeline();
    if (!playbin)
        return;

    gint audioCount = 0;
    gint videoCount = 0;
    gint textCount = 0;

    g_object_get(G_OBJECT(playbin), "n-audio", &audioCount, NULL);
    g_object_get(G_OBJECT(playbin), "n-video", &videoCount, NULL);
    g_object_get(G_OBJECT(playbin), "n-text", &textCount, NULL);

    QList<QVariantMap> audioProperties, videoProperties, textProperties;
    for (int i = 0; i < audioCount; ++i)
        audioProperties.append(getStreamProperties(playbin, "get-audio-tags", i));

    for (int i = 0; i < videoCount; ++i)
        videoProperties.append(getStreamProperties(playbin, "get-video-tags", i));

    for (int i = 0; i < textCount; ++i)
        textProperties.append(getStreamProperties(playbin, "get-text-tags", i));

    bool audioPropertiesChanged = audioStreamProperties != audioProperties;
    if (audioPropertiesChanged)
        audioStreamProperties = audioProperties;

    bool videoPropertiesChanged = videoStreamProperties != videoProperties;
    if (videoPropertiesChanged)
        videoStreamProperties = videoProperties;

    bool textPropertiesChanged = textStreamProperties != textProperties;
    if (textPropertiesChanged)
        textStreamProperties = textProperties;

    if (audioStreamsCount != audioCount) {
        audioStreamsCount = audioCount;
        emit q->audioStreamsCountChanged();
    }

    if (audioPropertiesChanged)
        emit q->audioStreamPropertiesChanged();

    if (videoStreamsCount != videoCount) {
        videoStreamsCount = videoCount;
        emit q->videoStreamsCountChanged();
    }

    if (videoPropertiesChanged)
        emit q->videoStreamPropertiesChanged();

    if (textStreamsCount != textCount) {
        textStreamsCount = textCount;
        emit q->textStreamsCountChanged();
    }

    if (textPropertiesChanged)
        emit q->textStreamPropertiesChanged();
}

QGstPlaybin::QGstPlaybin(QMediaPlayer *media, QObject *parent)
    : QGstPipeline(new QGstPlaybinPrivate(this), media, parent)
{
    Q_D(QGstPlaybin);

    connect(this, &QGstPlaybin::mediaPlayerChanged,
        this, [d]() { d->mediaPlayerChanged(); } );

    d->mediaPlayerChanged();
}

QGstPlaybin::QGstPlaybin(QObject *parent)
    : QGstPlaybin(nullptr, parent)
{
}

QGstPlaybin::~QGstPlaybin()
{
}

QString QGstPlaybin::videoSink() const
{
    return d_func()->videoSinkDesc;
}

void QGstPlaybin::setVideoSink(const QString &desc)
{
    Q_D(QGstPlaybin);

    d->pendingVideoSinkDesc = desc;
    if (!d->isReady() || d->videoSinkDesc == desc)
        return;

    d->updatePlaybin();
}

bool QGstPlaybin::showText() const
{
    return d_func()->showText;
}

void QGstPlaybin::setShowText(bool show)
{
    Q_D(QGstPlaybin);

    d->pendingShowText = show;
    GstElement *playbin = pipeline();
    if (!playbin || d->showText == show)
        return;

    d->showText = show;
    int flags = 0;
    g_object_get(G_OBJECT(playbin), "flags", &flags, NULL);

    if (show)
        flags |= GST_PLAY_FLAG_TEXT;
    else
        flags &= ~GST_PLAY_FLAG_TEXT;

    g_object_set(G_OBJECT(playbin), "flags", flags, NULL);
    emit showTextChanged();
}

QUrl QGstPlaybin::textUri() const
{
    return d_func()->textUri;
}

void QGstPlaybin::setTextUri(const QUrl &uri)
{
    Q_D(QGstPlaybin);

    d->pendingTextUri = uri;
    GstElement *playbin = pipeline();
    if (!playbin || d->textUri == uri)
        return;

    d->textUri = uri;
    setShowText(true);

    g_object_set(G_OBJECT(playbin), "suburi", uri.toEncoded().constData(), NULL);
    emit textUriChanged();
}

QString QGstPlaybin::textFont() const
{
    return d_func()->textFont;
}

void QGstPlaybin::setTextFont(const QString &str)
{
    Q_D(QGstPlaybin);

    d->pendingTextFont = str;
    GstElement *playbin = pipeline();
    if (!playbin || d->textFont == str)
        return;

    d->textFont = str;
    g_object_set(G_OBJECT(playbin), "subtitle-font-desc", str.toUtf8().constData(), NULL);
    emit textFontChanged();
}

int QGstPlaybin::audioStreamsCount() const
{
    return d_func()->audioStreamsCount;
}

int QGstPlaybin::audioStream() const
{
    return d_func()->audioStream;
}

void QGstPlaybin::setAudioStream(int i)
{
    Q_D(QGstPlaybin);

    d->pendingAudioStream = i;
    GstElement *playbin = pipeline();
    if (!playbin || d->audioStream == i || i >= d->audioStreamsCount)
        return;

    d->audioStream = i;
    g_object_set(G_OBJECT(playbin), "current-audio", i, NULL);
    emit audioStreamChanged();
}

QVariantMap QGstPlaybin::audioStreamProperties(int i)
{
    return d_func()->audioStreamProperties[i];
}

int QGstPlaybin::videoStreamsCount() const
{
    return d_func()->videoStreamsCount;
}

int QGstPlaybin::videoStream() const
{
    return d_func()->videoStream;
}

void QGstPlaybin::setVideoStream(int i)
{
    Q_D(QGstPlaybin);

    d->pendingVideoStream = i;
    GstElement *playbin = pipeline();
    if (!playbin || d->videoStream == i || i >= d->videoStreamsCount)
        return;

    d->videoStream = i;
    g_object_set(G_OBJECT(playbin), "current-video", i, NULL);
    emit videoStreamChanged();
}

QVariantMap QGstPlaybin::videoStreamProperties(int i)
{
    return d_func()->videoStreamProperties[i];
}

int QGstPlaybin::textStreamsCount() const
{
    return d_func()->textStreamsCount;
}

int QGstPlaybin::textStream() const
{
    return d_func()->textStream;
}

void QGstPlaybin::setTextStream(int i)
{
    Q_D(QGstPlaybin);

    d->pendingTextStream = i;
    GstElement *playbin = pipeline();
    if (!playbin || d->textStream == i || i >= d->textStreamsCount)
        return;

    d->textStream = i;
    g_object_set(G_OBJECT(playbin), "current-text", i, NULL);
    emit textStreamChanged();
}

QVariantMap QGstPlaybin::textStreamProperties(int i)
{
    return d_func()->textStreamProperties[i];
}

QT_END_NAMESPACE
