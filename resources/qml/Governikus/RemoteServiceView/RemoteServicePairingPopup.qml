import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Type.RemoteServiceModel 1.0

Popup {
	property bool requestInput: false
	property alias pin: name.text
	property var deviceId

	id: popup
	modal: true
	focus: true
	closePolicy: Popup.CloseOnPressOutside | Popup.CloseOnEscape
	width: Utils.dp(250)
	height: contentColumn.height + 2 * Constants.pane_padding
	padding: Constants.pane_padding

	Connections {
		target: RemoteServiceModel
		onFireEnvironmentChanged: close()
	}

	Column {
		id: contentColumn
		width: parent.width
		spacing: Constants.pane_spacing

		Text {
			id: header
			color: Constants.secondary_text
			text: qsTr("Pairing code") + settingsModel.translationTrigger
			font.pixelSize: Constants.header_font_size
			font.bold: true
		}

		Text {
			id: info
			color: Constants.secondary_text
			width: parent.width
			wrapMode: Text.WordWrap
			font.pixelSize: Constants.normal_font_size
			text: ( requestInput
					? qsTr("Enter the pairing code shown on your other device to use it as a card reader")
					: qsTr("Enter this code on your other device to use this device as a card reader")
					) + settingsModel.translationTrigger
		}

		TextField {
			id: name
			focus: true
			width: parent.width
			horizontalAlignment: Text.AlignHCenter
			font.letterSpacing: Utils.dp(5)
			font.pixelSize: Utils.dp(50)
			font.bold: true
			readOnly: !requestInput
			inputMethodHints: Qt.ImhDigitsOnly
			validator: RegExpValidator { regExp: /\d\d\d\d/ }
			onAccepted: {
				RemoteServiceModel.connectToServer(deviceId, name.getText(0,4))
				close()
			}
		}

		GButton {
			text: qsTr("Start pairing") + settingsModel.translationTrigger
			width: parent.width
			visible: requestInput

			onClicked: {
				RemoteServiceModel.connectToServer(deviceId, name.getText(0,4))
				close()
			}
		}
	}
}
