import QtQuick 2.7
import QtQuick.Controls 1.4

Rectangle {
	id: splashScreen
	visible: true
	anchors.fill: parent

	signal fireHiding()

	readonly property var startTime: new Date().getTime()

	Image {
		source: "qrc:/images/npa.svg"
		height: Math.min(parent.width, parent.height) * 0.5
		width: height
		fillMode: Image.PreserveAspectFit
		anchors.centerIn: parent
		visible: parent.visible
	}

	function hide() {
		if (!splashScreen.visible) {
			return
		}

		var TIMEOUT = 1000;
		var remaining = startTime + TIMEOUT - new Date().getTime();
		var timer = Qt.createQmlObject("import QtQuick 2.0; Timer {}", splashScreen);
		timer.interval = remaining > 0 ? remaining : 0;
		timer.repeat = false;
		timer.triggered.connect(function(){
			splashScreen.visible = false
			fireHiding()
		})
		timer.start();
	}
}
