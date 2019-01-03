import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.1

import Governikus.Global 1.0

MouseArea {
	height: Utils.dp(40)

	signal requestPairing(string pDeviceId)

	onClicked: {
		if (isSupported) requestPairing(deviceId)
	}

	Text {
		id: nameText
		color: Constants.secondary_text
		width: parent.width
		font.pixelSize: Utils.dp(16)
		anchors.verticalCenter: parent.verticalCenter
		opacity: 0.87
		text: {
			settingsModel.translationTrigger

			if (isSupported) {
				return remoteDeviceName;
			}
			return remoteDeviceName + " (" + qsTr("Unsupported") + ")"
		}
	}

	Rectangle {
		width: parent.width
		height: Utils.dp(1)
		color: "black"
		opacity: 0.1
		anchors.bottom: parent.bottom
	}
}
