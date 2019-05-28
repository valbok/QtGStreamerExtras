# QtGStreamerExtras
QtGStreamerExtras is Qt module that extends GStreamer functionality of Qt Multimedia.

Introduced QGstPipeline base class to change a pipeline of the QMediaPlayer.
Provides an access to the media player's pipeline:
    
    QGstPipeline gst(player);
    gst.setPipeline("videotestsrc name=src ! autovideosink");

Also allows setting properties of some pipeline elements:

    QVariantMap m;
    m["name"] = "new_name";
    gst.set("src", m);

By default QMediaPlayer uses the playbin element.
Since it is backend related, playbin's features are limited by public API. And not possible to expose these features to users.

QGstPlaybin provides some playbin related functionalities:
- videoSink: Defines pipeline description of video sink
- showText: Show subtitles
- textUri: Defines the subtitles
- textFont: Font of the subtitles
- audioStreamsCount: Number of audio streams
- audioStream: Current audio stream
- videoStreamsCount: Number of video streams
- videoStream: Current video stream
- textStreamsCount: Number of text streams
- textStream: Current text stream

Changing of the video sink could be done like:

    QGstPlaybin playbin(player); 
    playbin.setVideoSink("xvimagesink");

# Build

Build the module:

    qmake; make
    
After If an app is widget based, need to add following to pro file:

    QT += gstreamerextras

Here is the simplest example how to use controls from QML MediaPlayer element with custom gstreamer pipeline:

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

        MouseArea {
           anchors.fill: parent
           onClicked:  {
               if (player.playbackState == MediaPlayer.PlayingState)
                   player.pause()
               else
                   player.play()
           }
        }
    }

# Examples

Some examples could be found in *examples* dir.

E.g. the streamer example allows to create a video stream of rendered QML elements. 
Consists of the *streamer* bin which takes QML VideoOutput element, grabs its content and sends over the network via udp.
The *receiver* bin just listens to defined port and renders received content to its VideoOutput element.

