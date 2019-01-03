import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0


Rectangle {
	property alias text: number.text

	height: number.height * 2
	width: height
	enabled: false
	radius: width * 0.5
	color: enabled ? Constants.blue : Constants.white

	Text {
		id: number
		anchors.centerIn: parent
		font.bold: true
		font.pixelSize: Constants.header_font_size
		color: parent.enabled ? Constants.white : Constants.blue
	}
}
