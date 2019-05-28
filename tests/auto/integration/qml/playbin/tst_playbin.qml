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

import QtQuick 2.0
import QtMultimedia 5.9
import QtGStreamerExtras 5.12
import QtTest 1.0

Item {
    id: top

    property string srcMp4: "../../playbin/testdata/colors.mp4"
    property string srcText: "../../playbin/testdata/text.srt"

    VideoOutput {
        id: video
        source: player
    }

    MediaPlayer {
        id: player
        autoPlay: true
        loops: Audio.Infinite
        source: srcMp4
    }

    Playbin {
        id: control
        source: player
        textUri: srcText
        videoSink: "xvimagesink pixel-aspect-ratio=\"1/2\""
    }

    SignalSpy {
        id: spyVideoSink
        target: control
        signalName: "videoSinkChanged"
    }

    SignalSpy {
        id: spyShowText
        target: control
        signalName: "showTextChanged"
    }

    SignalSpy {
        id: spyTextUri
        target: control
        signalName: "textUriChanged"
    }

    SignalSpy {
        id: spyTextFont
        target: control
        signalName: "textFontChanged"
    }

    SignalSpy {
        id: spyAudioStreamsCount
        target: control
        signalName: "audioStreamsCountChanged"
    }

    SignalSpy {
        id: spyVideoStreamsCount
        target: control
        signalName: "videoStreamsCountChanged"
    }

    SignalSpy {
        id: spyTextStreamsCount
        target: control
        signalName: "textStreamsCountChanged"
    }

    SignalSpy {
        id: spyTextStream
        target: control
        signalName: "onTextStreamChanged"
    }

    SignalSpy {
        id: spyTextStreamProperties
        target: control
        signalName: "onTextStreamPropertiesChanged"
    }

    function basename(str) {
        return (str.slice(str.lastIndexOf("/") + 1))
    }

    TestCase {
        name: "Playbin"

        function initTestCase() {
            compare(control.source, player)
        }

        function test_videoSink() {
            compare(spyVideoSink.count, 1);
            compare(control.videoSink, "xvimagesink pixel-aspect-ratio=\"1/2\"")
        }

        function test_showText() {
            spyShowText.clear()
            verify(control.showText)
            control.showText = true
            compare(spyShowText.count, 0);
            verify(control.showText)
            control.showText = false
            compare(spyShowText.count, 1);
            verify(!control.showText)
        }

        function test_textUri() {
            spyTextUri.clear();
            control.showText = false
            control.textUri = "dummy"
            verify(control.showText)
            compare(spyTextUri.count, 1)
            control.textUri = srcText
            compare(spyTextUri.count, 2)
            compare(basename(control.textUri.toString()), basename(srcText.toString()))
        }

        function test_textFont() {
            compare(spyTextFont.count, 0)
            compare(control.textFont, "")
            control.textFont = "Sans, 18"
            compare(spyTextFont.count, 1)
            compare(control.textFont, "Sans, 18")
        }

        function test_audioStreams() {
            tryCompare(control, "audioStreamsCount", 1, 3000)
            verify(spyAudioStreamsCount.count > 0)
        }

        function test_videoStreams() {
            tryCompare(control, "videoStreamsCount", 1, 3000)
            verify(spyVideoStreamsCount.count > 0)
        }

        function test_textStreams() {
            tryCompare(control, "textStreamsCount", 3, 3000)
            verify(spyTextStreamsCount.count > 0)
        }

        function test_setTextStream() {
            spyTextStream.clear()
            control.textStream = 0
            compare(control.textStream, 0)
            compare(spyTextStream.count, 1)
            control.textStream = 1
            compare(control.textStream, 1)
            compare(spyTextStream.count, 2)
        }

        function test_textStreamProperties() {
            tryVerify(function(){
                return control.textStreamsCount > 1 && control.textStreamProperties(1)["Language"] == "en";
            }, 3000)
            compare(control.textStreamProperties(0)["Language"], undefined)
            compare(control.textStreamProperties(2)["Language"], "no")
            verify(spyTextStreamProperties.count > 0)
        }
    }
}
