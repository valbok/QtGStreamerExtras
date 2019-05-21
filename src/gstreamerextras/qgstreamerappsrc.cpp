/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
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

#include "qgstreamerappsrc_p.h"
#include <private/qgstutils_p.h>

QT_BEGIN_NAMESPACE

class QGstreamerAppSrcPrivate
{
    Q_DECLARE_PUBLIC(QGstreamerAppSrc)

public:
    QGstreamerAppSrcPrivate(GstElement *src, QGstreamerAppSrc *q)
        : appsrc(src)
        , q_ptr(q)
    {
    }

    bool restart(QVideoFrame &frame);

private:
    GstElement *appsrc = nullptr;
    QGstreamerAppSrc *q_ptr = nullptr;
    int timerMsec = 30;
    int timerId = 0;
    QSize size;
};

bool QGstreamerAppSrcPrivate::restart(QVideoFrame &frame)
{
    QList<QVideoFrame::PixelFormat> cc;
    cc << frame.pixelFormat();
    GstCaps *caps = QGstUtils::capsForFormats(cc);
    if (gst_caps_is_empty(caps)) {
        qWarning() << "Could not create caps for" << frame.pixelFormat();
        gst_caps_unref(caps);
        return false;
    }

    gst_caps_set_simple(
        caps,
        "framerate", GST_TYPE_FRACTION , 30, 1,
        "width", G_TYPE_INT, frame.width(),
        "height", G_TYPE_INT, frame.height(),
        NULL);

    g_object_set(G_OBJECT(appsrc), "caps", caps, NULL);
    gst_caps_unref(caps);
    size = frame.size();
    return true;
}

static void start_feed(GstElement *pipeline, guint size, QGstreamerAppSrc *q)
{
    Q_UNUSED(pipeline);
    Q_UNUSED(size);
    QMetaObject::invokeMethod(q, "startFeed", Qt::QueuedConnection);
}

static void stop_feed(GstElement *pipeline, QGstreamerAppSrc *q)
{
    Q_UNUSED(pipeline);
    QMetaObject::invokeMethod(q, "stopFeed", Qt::QueuedConnection);
}

void QGstreamerAppSrc::startFeed()
{
    Q_D(QGstreamerAppSrc);

    if (d->timerId)
        return;

    d->timerId = startTimer(d->timerMsec);
}

void QGstreamerAppSrc::stopFeed()
{
    Q_D(QGstreamerAppSrc);

    if (!d->timerId)
        return;

    killTimer(d->timerId);
    d->timerId = 0;
}

bool QGstreamerAppSrc::readFrame(QVideoFrame &frame) const
{
    Q_UNUSED(frame);
    return false;
}

void QGstreamerAppSrc::timerEvent(QTimerEvent *event)
{
    Q_D(QGstreamerAppSrc);

    if (event->timerId() != d->timerId)
        return;

    QVideoFrame frame;
    if (!readFrame(frame) || !frame.isValid() || (d->size != frame.size() && !d->restart(frame)))
        return;

    if (!frame.map(QAbstractVideoBuffer::ReadOnly))
        return;

    auto app_buffer = gst_buffer_new();
    int size = frame.bytesPerLine() * frame.height();
    auto mem = gst_allocator_alloc (NULL, size, NULL);

    GstMapInfo info;
    gst_memory_map(mem, &info, GST_MAP_WRITE);
    memcpy(info.data, frame.bits(), size);
    gst_memory_unmap(mem, &info);

    gst_buffer_append_memory(app_buffer, mem);
    gst_buffer_set_size(app_buffer, size);

    GstFlowReturn ret;
    g_signal_emit_by_name(d->appsrc, "push-buffer", app_buffer, &ret);
    if (ret != GST_FLOW_OK) {
        qWarning() << "Error has occurred while pushing a buffer";
        stopFeed();
    }

    gst_buffer_unref(app_buffer);
    frame.unmap();
}

QGstreamerAppSrc::QGstreamerAppSrc(GstElement *appsrc, QObject *parent)
    : QObject(parent)
    , d_ptr(new QGstreamerAppSrcPrivate(appsrc, this))
{
    g_signal_connect(appsrc, "need-data", G_CALLBACK(start_feed), this);
    g_signal_connect(appsrc, "enough-data", G_CALLBACK(stop_feed), this);    
    g_object_set(G_OBJECT(appsrc), "do-timestamp", TRUE, NULL);
    g_object_set(G_OBJECT(appsrc), "duration", GST_CLOCK_TIME_NONE, NULL);
    g_object_set(G_OBJECT(appsrc), "format", GST_FORMAT_TIME, NULL);
    g_object_set(G_OBJECT(appsrc), "is-live", TRUE, NULL);
}

QGstreamerAppSrc::~QGstreamerAppSrc()
{
    delete d_ptr;
}

QT_END_NAMESPACE
