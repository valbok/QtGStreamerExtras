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

#include "qgstreamerpipeline.h"
#include "qgstreamerpipeline_p.h"
#include <private/qgstreamervideorenderer_p.h>
#include <QMediaService>

QT_BEGIN_NAMESPACE

QGstreamerVideoRendererInterface *QGstreamerPipelinePrivate::renderer() const
{
    return session ? session->renderer() : nullptr;
}

QGstreamerPipeline::QGstreamerPipeline(QObject *parent)
    : QGstreamerPipeline(new QGstreamerPipelinePrivate(this), nullptr, parent)
{
}

QGstreamerPipeline::QGstreamerPipeline(QMediaPlayer *player, QObject *parent)
    : QGstreamerPipeline(new QGstreamerPipelinePrivate(this), player, parent)
{
}

QGstreamerPipeline::QGstreamerPipeline(QGstreamerPipelinePrivate *d, QMediaPlayer *player, QObject *parent)
    : QObject(parent)
    , d_ptr(d)
{
    setMediaPlayer(player);
}

QGstreamerPipeline::~QGstreamerPipeline()
{
    delete d_ptr;
}

QObject *QGstreamerPipeline::source() const
{
    return d_func()->source.data();
}

void QGstreamerPipeline::setSource(QObject *src)
{
    Q_D(QGstreamerPipeline);

    if (d->source == src)
        return;

    if (d->source)
        disconnect(d->source.data(), 0, this, SLOT(updateMediaObject()));

    d->source = src;
    if (d->source) {
        const auto *metaObj = d->source->metaObject();
        int propId = metaObj->indexOfProperty("mediaObject");
        if (propId != -1) {
            const auto prop = metaObj->property(propId);
            if (prop.hasNotifySignal()) {
                QMetaMethod method = prop.notifySignal();
                QMetaObject::connect(d->source.data(), method.methodIndex(),
                    this, metaObject()->indexOfSlot("updateMediaPlayer()"),
                    Qt::DirectConnection, 0);
            }
        }
    }

    updateMediaPlayer();
    emit sourceChanged();
}

void QGstreamerPipeline::updateMediaPlayer()
{
    QMediaPlayer *obj = qobject_cast<QMediaPlayer*>(
        d_func()->source->property("mediaObject").value<QObject*>());
    setMediaPlayer(obj);
}

QMediaPlayer *QGstreamerPipeline::mediaPlayer() const
{
    return d_func()->mediaPlayer.data();
}

void QGstreamerPipeline::setMediaPlayer(QMediaPlayer *src)
{
    Q_D(QGstreamerPipeline);

    auto service = src ? src->service() : nullptr;
    if (!service)
        return;

    auto control = qobject_cast<QGstreamerPlayerControl *>(service->requestControl(QMediaPlayerControl_iid));
    d->session = control ? control->session() : nullptr;
    if (!d->session)
        return;

    connect(d->session, &QGstreamerPlayerSession::pipelineChanged,
        this, &QGstreamerPipeline::pipelineChanged);

    d->mediaPlayer = src;
    emit mediaPlayerChanged();

    setPipeline(d->pendingPipelineDesc);
}

QString QGstreamerPipeline::pipelineDesc() const
{
    return d_func()->pipelineDesc;
}

void QGstreamerPipeline::setPipeline(const QString &desc)
{
    Q_D(QGstreamerPipeline);

    d->pendingPipelineDesc = desc;
    if (!d->mediaPlayer || d->pipelineDesc == desc)
        return;

    QString pl = QLatin1String("gst-pipeline:") + desc;
    if (d->source)
        d->source->setProperty("source", QVariant::fromValue<QUrl>(pl));
    else
        d->mediaPlayer->setMedia(QMediaContent(pl));

    d->pipelineDesc = d->pendingPipelineDesc;
    d->pendingPipelineDesc.clear();
}

GstElement *QGstreamerPipeline::pipeline() const
{
    return d_ptr->session ? d_ptr->session->pipeline() : nullptr;
}

template <class T>
static void setGstProperty(GstElement *element, const QString &name, T v)
{
    g_object_set(G_OBJECT(element), name.toLatin1().constData(), v, NULL);
}

bool QGstreamerPipeline::set(const QString &elementName, const QVariantMap &map)
{
    Q_D(QGstreamerPipeline);

    GstElement *element = nullptr;
    if (d->session) {
        if (elementName == QLatin1String("videoSink"))
            element = d->session->renderer() ? d->session->renderer()->videoSink() : nullptr;
        else if (d->session->pipeline())
            element = gst_bin_get_by_name(GST_BIN(d->session->pipeline()), elementName.toLatin1().constData());
    }

     if (!element) {
        qWarning() << "Could not find element by name:" << elementName;
        if (d->session->pipeline()) {
            qWarning() << "Available elements:";
            qWarning() << " " << "videoSink";
            GstIterator *children = gst_bin_iterate_recurse(GST_BIN(d->session->pipeline()));
            gst_iterator_foreach(children, [](const GValue *item, gpointer) {
                GstElement *element = GST_ELEMENT(g_value_get_object(item));
                qWarning() << " " << GST_ELEMENT_NAME(element);
            }, nullptr);
            gst_iterator_free(children);
        } else {
            qWarning() << "No pipeline available";
        }

        return false;
    }

    for (auto it = map.begin(); it != map.end(); ++it) {
        QString name = it.key();
        QVariant value = it.value();

        switch (value.type()) {
        case QMetaType::Int:
            setGstProperty(element, name, value.toInt());
            break;
        case QMetaType::Bool:
            setGstProperty(element, name, value.toBool());
            break;
        case QMetaType::Double:
            setGstProperty(element, name, value.toDouble());
            break;
        case QMetaType::QString:
            setGstProperty(element, name, value.toString().toUtf8().constData());
            break;
        default:
            break;
        }
    }

    return true;
}

QT_END_NAMESPACE


