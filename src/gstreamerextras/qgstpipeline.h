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

#ifndef QGSTPIPELINE_H
#define QGSTPIPELINE_H

#include <QtMultimedia/qtmultimediaglobal.h>
#include <QVariantMap>
#include <QUrl>
#include <gst/gst.h>

QT_BEGIN_NAMESPACE

class QMediaPlayer;
class QGstPipelinePrivate;
class Q_MULTIMEDIA_EXPORT QGstPipeline : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QMediaPlayer* mediaPlayer READ mediaPlayer NOTIFY mediaPlayerChanged SCRIPTABLE false DESIGNABLE false)
    Q_PROPERTY(QString pipeline READ pipelineDesc WRITE setPipeline NOTIFY pipelineChanged)

public:
    QGstPipeline(QObject *parent = nullptr);
    QGstPipeline(QMediaPlayer *player, QObject *parent = nullptr);
    ~QGstPipeline();

    QObject *source() const;
    void setSource(QObject *source);

    QMediaPlayer *mediaPlayer() const;
    void setMediaPlayer(QMediaPlayer *source);

    QString pipelineDesc() const;
    void setPipeline(const QString &desc);
    GstElement *pipeline() const;

    Q_INVOKABLE bool set(const QString &elementName, const QVariantMap &map);

Q_SIGNALS:
    void sourceChanged();
    void mediaPlayerChanged();
    void pipelineChanged();

private Q_SLOTS:
    void updateMediaPlayer();

protected:
    QGstPipeline(QGstPipelinePrivate *d,
        QMediaPlayer *player = nullptr, QObject *parent = nullptr);

    QGstPipelinePrivate *d_ptr = nullptr;

private:
    Q_DISABLE_COPY(QGstPipeline)
    Q_DECLARE_PRIVATE(QGstPipeline)
};

QT_END_NAMESPACE

#endif
