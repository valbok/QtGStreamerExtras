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
import QtGstreamerExtras 5.12
import QtTest 1.0

Item {
    id: top

    VideoOutput {
        id: video
        source: player
    }

    MediaPlayer {
        id: player
        autoPlay: false
        loops: Audio.Infinite
    }

    GstreamerPipeline {
        id: control
        source: player
    }

    SignalSpy {
        id: spySource
        target: control
        signalName: "sourceChanged"
    }

    SignalSpy {
        id: spyMediaPlayer
        target: control
        signalName: "mediaPlayerChanged"
    }

    SignalSpy {
        id: spyPipeline
        target: control
        signalName: "pipelineChanged"
    }

    MediaPlayer {
        id: player2
        autoPlay: false
        loops: Audio.Infinite
    }

    GstreamerPipeline {
        id: control2
        source: player2
    }

    SignalSpy {
        id: spyPipeline2
        target: control2
        signalName: "pipelineChanged"
    }

    MediaPlayer {
        id: player3
        autoPlay: false
        loops: Audio.Infinite
    }

    GstreamerPipeline {
        id: control3
        source: player3
        pipeline: "videotestsrc ! autovideosink"
    }

    SignalSpy {
        id: spyPipeline3
        target: control3
        signalName: "pipelineChanged"
    }

    TestCase {
        name: "Pipeline"

        function initTestCase() {
            compare(control.source, player);
            compare(spySource.count, 1);
            compare(spyMediaPlayer.count, 1);
        }

        function test_pipeline() {
            spyPipeline.clear();
            var pp = "videotestsrc ! autovideosink";
            control.pipeline = pp;
            compare(spyPipeline.count, 1);
            compare(control.pipeline, pp);
        }

        function test_setProp() {
            spyPipeline.clear();
            control.pipeline = "videotestsrc name=src ! autovideosink";
            compare(spyPipeline.count, 1);
            control.set("src", {"num-buffers": 1});
            compare(player.error, MediaPlayer.NoError);
        }

        function test_pipelineWithoutOutput() {
            spyPipeline2.clear();
            var pp = "videotestsrc ! autovideosink";
            control2.pipeline = pp;
            compare(spyPipeline2.count, 1);
            compare(control2.pipeline, pp);
            compare(player2.error, MediaPlayer.NoError);
            compare(spyPipeline3.count, 1);
            compare(control3.pipeline, pp);
            compare(player3.error, MediaPlayer.NoError);
        }
    }
}
