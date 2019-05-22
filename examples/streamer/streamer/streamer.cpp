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

#include "streamer.h"
#include <QQuickItemGrabResult>

void AppSrc::imageReady()
{
    auto img = m_grabResult->image();
    img.reinterpretAsFormat(QImage::Format_ARGB32);
    m_frame = img;
    m_grabResult.reset();
}

bool AppSrc::readFrame(QVideoFrame &frame) const
{
    if (!m_frame.isValid() && m_streamer->item()) {
        m_grabResult = qobject_cast<QQuickItem*>(m_streamer->item())->grabToImage();
        connect(m_grabResult.data(), &QQuickItemGrabResult::ready, this, &AppSrc::imageReady);
        return false;
    }

    frame = m_frame;
    m_frame = QVideoFrame();

    return true;
}

Streamer::Streamer(QObject *parent)
    : QGstreamerPipeline(parent)
{
    connect(this, &QGstreamerPipeline::pipelineChanged,
        this, &Streamer::onPipelineChanged);
}

QObject *Streamer::item() const
{
    return m_item;
}

void Streamer::setItem(QObject *src)
{
    auto item = qobject_cast<QQuickItem*>(src);
    if (m_item == item)
        return;

    m_item = item;
    emit itemChanged();
}

void Streamer::onPipelineChanged()
{
    GstElement *pl = pipeline();
    if (!pl)
        return;

    auto appsrc = gst_bin_get_by_name(GST_BIN(pl), "src");
    if (!appsrc)
        return;

    m_appsrc.reset(new AppSrc(this, appsrc));
}

