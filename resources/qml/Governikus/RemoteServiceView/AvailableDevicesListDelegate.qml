import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1

import Governikus.Global 1.0

MouseArea {
	height: Utils.dp(40)

	signal requestPairing(string pDeviceId)

	onClicked: {
		requestPairing(deviceId)
	}

	Text {
		id: nameText
		width: parent.width
		font.pixelSize: Utils.sp(16)
		anchors.verticalCenter: parent.verticalCenter
		opacity: 0.87
		text: remoteDeviceName
	}

	Rectangle {
		width: parent.width
		height: Utils.dp(1)
		color: "black"
		opacity: 0.1
		anchors.bottom: parent.bottom
	}
}
