# QtGStreamerExtras
QtGStreamerExtras is Qt module that extends GStreamer functionality of Qt Multimedia.
 
    import QtQuick 2.9
    import QtMultimedia 5.12
    import QtGStreamerExtras 5.12
    
    Item {
        anchors.fill: parent
    
        VideoOutput {
            id: video
            anchors.fill: parent
            source: player
        }

        MediaPlayer {
            id: player
            autoPlay: true
        }
    
        Pipeline {
            id: gst
            pipeline: "videotestsrc ! qtvideosink"
        }
    }

