import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.View 1.0


Button {
	id: button
	width: height
	enabled: visible
	property alias source: image.source

	Accessible.role: Accessible.Button

	FocusFrame {}

	background: Image {
		id: image
		anchors.centerIn: parent
		sourceSize.height: button.height * (button.down ? 0.9 : 1)
	}
}
