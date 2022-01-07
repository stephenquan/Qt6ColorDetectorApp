import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.15
import QtMultimedia 6.0
import ColorDetectorApp 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Qt6 Color Detector App")

    Page {
        id: page

        anchors.fill: parent

        title: qsTr("Qt6 Color Detector App")

        header: Frame {
            background: Rectangle {
                color: "#e0e0e0"
            }

            RowLayout {
                width: parent.width

                Text {
                    Layout.fillWidth: true
                    text: page.title
                }
            }
        }

        VideoOutput {
            id: preview
            anchors.fill: parent
        }

        footer: Frame {
            background: Rectangle {
                color: "#e0e0e0"
            }

            RowLayout {
                width: parent.width

                Rectangle {
                    id: result
                    Layout.alignment: Qt.AlignHCenter

                    width: 24
                    height: 24
                }
            }
        }
    }

    MediaDevices {
        id: mediaDevices
    }

    CaptureSession {
        id: _captureSession
        camera: _camera
        videoOutput: preview
    }

    Camera {
        id: _camera
    }

    ColorDetector {
        id: colorDetector

        interval: 100

        onColorDetected: {
            result.color = color;
        }
    }

    Component.onCompleted: {
        for (let cd of mediaDevices.videoInputs) {
            if (cd.description.match(/(Logitech)/)) {
                _camera.cameraDevice = cd;
            }
        }

        _camera.start();

        colorDetector.start(_captureSession);
    }

}
