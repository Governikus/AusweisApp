import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4


Item {
	property string image
	property alias running: busyIndicator.running

	BusyIndicator {
		id: busyIndicator
		anchors.centerIn: parent
		height: img.height
		width: height
		running: false
		style: NpaBusyIndicatorStyle {factor: 1.2}
	}

	Image {
		id: img
		anchors.centerIn: parent
		width: parent.width
		fillMode: Image.PreserveAspectFit
		smooth: true
		source: image
	}
}
