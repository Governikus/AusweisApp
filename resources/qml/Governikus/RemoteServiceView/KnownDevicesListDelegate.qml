import QtQuick 2.7
import QtQuick.Layouts 1.1

import Governikus.Global 1.0

Item {
	id: root
	height: Utils.dp(60)

	Item {
		id: textItem
		height: childrenRect.height
		width: parent.width * 0.8
		anchors.verticalCenter: root.verticalCenter

		Text {
			id: nameText
			color: Constants.secondary_text
			font.pixelSize: Utils.sp(16)
			opacity: 0.87
			text: {
				settingsModel.translationTrigger

				if (!isNetworkVisible) {
					return remoteDeviceName;
				}
				if (isSupported) {
					return remoteDeviceName + " (" + qsTr("Available") + ")"
				}
				return remoteDeviceName + " (" + qsTr("Available, but unsupported") + ")"
			}
		}

		Text {
			id: dateText
			color: Constants.secondary_text
			anchors.top: nameText.bottom
			anchors.topMargin: Utils.dp(2)
			font.pixelSize: Utils.sp(14)
			opacity: 0.38
			text: qsTr("Last connection:") + " " + lastConnected + settingsModel.translationTrigger
		}
	}

	MouseArea {
		id: iconClick
		width: Utils.dp(44)
		height: width

		anchors.right: root.right
		anchors.verticalCenter: root.verticalCenter

		Image {
			id: icon
			width: Utils.dp(22)
			height: width
			anchors.centerIn: parent
			source: "qrc:///images/iOS/search_cancel.svg"
		}

		onClicked: {
			remoteServiceModel.forgetDevice(deviceId)
		}
	}

	Rectangle {
		width: parent.width
		height: Utils.dp(1)
		color: "black"
		opacity: 0.1
		anchors.bottom: root.bottom
	}
}
