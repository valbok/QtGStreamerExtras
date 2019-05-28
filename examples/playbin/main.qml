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

import QtQuick 2.7
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.12
import QtMultimedia 5.11
import QtGStreamerExtras 5.12

Rectangle {
    id: root
    anchors.fill: parent
    color: "black"

    property string uri
    property string pipeline
    property string videoSink
    property bool showText: false
    property string textUri
    property string textFont
    property int audioStream: -1
    property int videoStream: -1
    property int textStream: -1

    Component {
        id: comp
        Item {
            anchors.fill: root

            VideoOutput {
                id: videoOut
                anchors.fill: parent
                source: player
            }

            MediaPlayer {
                id: player
                autoPlay: true
            }

            Playbin {
                id: gst
                source: player
                videoSink: root.videoSink
                showText: root.showText
                textUri: root.textUri
                textFont: root.textFont
                audioStream: root.audioStream
                videoStream: root.videoStream
                textStream: root.textStream
            }

            MouseArea {
                anchors.fill: parent
                onClicked:  {
                    if (player.playbackState == MediaPlayer.PlayingState)
                        player.pause()
                    else
                        player.play()
                }
            }

            Component.onCompleted: {
                if (root.uri)
                    player.source = root.uri
                else
                    gst.pipeline = "videotestsrc ! qtvideosink"
            }
        }
    }

    function init() {
        comp.createObject(root)
    }
}
