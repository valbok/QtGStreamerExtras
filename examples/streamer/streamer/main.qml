import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.4
import QtMultimedia 5.11
import QtGstreamerExtras 5.12
import Streamer 1.0

ApplicationWindow {
    visible: true
    width: 1024
    height: 768

    Item {
        anchors.fill: parent

        Camera {
            id: cam
        }

        VideoOutput {
            id: video
            anchors.fill: parent
            source: cam
            fillMode: VideoOutput.PreserveAspectFit

            Timer {
                interval: 1000; running: true; repeat: true
                onTriggered: time.text = Date().toString()
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                color: "red"
                id: time
                font.pointSize: 20
            }
        }

        MediaPlayer {
            id: player
            autoPlay: true
        }

        Streamer {
            id: pl
            source: player
            item: video
            pipeline: "appsrc name=src ! videoconvert ! video/x-raw,format=I420 ! jpegenc ! rtpjpegpay ! udpsink host=127.0.0.1 port=5201"
        }
    }
}
