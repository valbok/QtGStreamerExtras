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

ApplicationWindow {
    id: window
    title: "GStreamer pipeline example"
    visible: true
    width: 1920
    height: 1080
    color: "black"

    property var filter: "queue"
    RowLayout {
        anchors.fill: parent
        ListView {
            visible: true
            id: view
            Layout.fillHeight: true
            width: 200
            readonly property int columnCount: 3

            model: ListModel {
                ListElement { filters: "identity" }
                ListElement { filters: "bulge" }
                ListElement { filters: "dicetv" }
                ListElement { filters: "edgetv" }
                ListElement { filters: "videoflip method=horizontal-flip" }
            }

            delegate: Pipeline {
                id: video
                height: 200
                width: 200
                pipeline: "filesrc location=images/qt-logo.png ! decodebin ! videoconvert ! imagefreeze ! " + model.filters + " ! qtvideosink"

                MouseArea {
                    anchors.fill:parent
                    onClicked: filter = model.filters
                }
            }
        }

        Pipeline {
            Layout.fillWidth: true
            Layout.fillHeight: true
            pipeline: "v4l2src ! queue ! avdec_h264 ! videoconvert ! " + filter + " ! videoconvert ! qtvideosink"
        }
    }
}
