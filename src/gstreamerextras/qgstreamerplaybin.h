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

#ifndef QGSTREAMERPLAYBIN_H
#define QGSTREAMERPLAYBIN_H

#include <QtGstreamerExtras/QGstreamerPipeline>
#include <QVariantMap>
#include <QUrl>

QT_BEGIN_NAMESPACE

class QGstreamerPlaybinPrivate;
class Q_MULTIMEDIA_EXPORT QGstreamerPlaybin : public QGstreamerPipeline
{
    Q_OBJECT
    Q_PROPERTY(QString videoSink READ videoSink WRITE setVideoSink NOTIFY videoSinkChanged)
    Q_PROPERTY(bool showText READ showText WRITE setShowText NOTIFY showTextChanged)
    Q_PROPERTY(QUrl textUri READ textUri WRITE setTextUri NOTIFY textUriChanged)
    Q_PROPERTY(QString textFont READ textFont WRITE setTextFont NOTIFY textFontChanged)
    Q_PROPERTY(int audioStreamsCount READ audioStreamsCount NOTIFY audioStreamsCountChanged)
    Q_PROPERTY(int audioStream READ audioStream WRITE setAudioStream NOTIFY audioStreamChanged)
    Q_PROPERTY(int videoStreamsCount READ videoStreamsCount NOTIFY videoStreamsCountChanged)
    Q_PROPERTY(int videoStream READ videoStream WRITE setVideoStream NOTIFY videoStreamChanged)
    Q_PROPERTY(int textStreamsCount READ textStreamsCount NOTIFY textStreamsCountChanged)
    Q_PROPERTY(int textStream READ textStream WRITE setTextStream NOTIFY textStreamChanged)

public:
    QGstreamerPlaybin(QMediaPlayer *player, QObject *parent = nullptr);
    QGstreamerPlaybin(QObject *parent = nullptr);
    ~QGstreamerPlaybin();

    QString videoSink() const;
    void setVideoSink(const QString &name);

    bool showText() const;
    void setShowText(bool show);

    QUrl textUri() const;
    void setTextUri(const QUrl &url);

    QString textFont() const;
    void setTextFont(const QString &str);

    int audioStreamsCount() const;
    int audioStream() const;
    void setAudioStream(int i);
    Q_INVOKABLE QVariantMap audioStreamProperties(int i);

    int videoStreamsCount() const;
    int videoStream() const;
    void setVideoStream(int i);
    Q_INVOKABLE QVariantMap videoStreamProperties(int i);

    int textStreamsCount() const;
    int textStream() const;
    void setTextStream(int i);
    Q_INVOKABLE QVariantMap textStreamProperties(int i);

Q_SIGNALS:
    void videoSinkChanged();
    void showTextChanged();
    void textUriChanged();
    void textFontChanged();

    void audioStreamsCountChanged();
    void audioStreamChanged();
    void audioStreamPropertiesChanged();

    void videoStreamsCountChanged();
    void videoStreamChanged();
    void videoStreamPropertiesChanged();

    void textStreamsCountChanged();
    void textStreamChanged();
    void textStreamPropertiesChanged();

private:
    Q_DISABLE_COPY(QGstreamerPlaybin)
    Q_DECLARE_PRIVATE(QGstreamerPlaybin)
};

QT_END_NAMESPACE

#endif
