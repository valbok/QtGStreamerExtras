import QtQuick 2.5
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.4
import QtMultimedia 5.11
import QtGstreamerExtras 5.12

ApplicationWindow {
    visible: true
    width: 800
    height: 600
    color: "grey"

    Item {
        anchors.fill: parent

        GstreamerPipeline {
            id: pl
            source: receiver
            pipeline: "udpsrc port=" + PORT + " ! application/x-rtp,encoding-name=JPEG,payload=26 ! rtpjpegdepay ! jpegdec ! qtvideosink"
        }

        MediaPlayer {
            id: receiver
            autoPlay: true
        }

        VideoOutput {
            id: video
            anchors.fill: parent
            source: receiver
            fillMode: VideoOutput.PreserveAspectFit
        }

        MouseArea {
            anchors.fill: video
            onClicked:  {
                if (receiver.playbackState == MediaPlayer.PlayingState)
                    receiver.pause()
                else
                    receiver.play()
            }
        }
    }
}
