import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Type.RemoteServiceModel 1.0

Item {
	id: baseItem
	height: mainColumn.height

	Column {
		id: mainColumn
		width: baseItem.width
		padding: Constants.pane_padding
		spacing: Constants.component_spacing

		readonly property int usableWidth: width - 2 * padding

		Text {
			id: errorMsg
			width: parent.usableWidth
			text: ""
			color: Constants.red
			font.pixelSize: Constants.normal_font_size
			wrapMode: Text.WordWrap
			verticalAlignment: Text.AlignVCenter
			visible: text !== ""
		}

		Column {
			id: nameContainer
			width: parent.usableWidth

			Text {
				text: qsTr("Device name") + settingsModel.translationTrigger
				font.pixelSize: Constants.normal_font_size
				font.bold: true
				color: Constants.blue
			}

			Text {
				color: Constants.secondary_text
				text: qsTr("Set device name:") + settingsModel.translationTrigger
				width: parent.width
				font.pixelSize: Constants.normal_font_size
				wrapMode: Text.WordWrap
			}

			Item {
				id: spacing
				height: Utils.dp(5)
				width: height
			}

			GTextField {
				id: serverName
				width: parent.width
				onAccepted: {
					settingsModel.serverName = text
					text = settingsModel.serverName
					nameContainer.forceActiveFocus(Qt.MouseFocusReason)
				}
				onVisibleChanged: {
					nameContainer.forceActiveFocus(Qt.MouseFocusReason)
					if (visible) text = settingsModel.serverName
				}
			}
		}

		Item {
			id: pinPadModeContainer
			visible: Qt.platform.os !== "ios"
			width: parent.usableWidth
			height: Math.max(pinPadModeText.height, pinPadModeSwitch.height)

			Item {
				id: pinPadModeText
				height: nameText.height + dateText.height
				anchors.left: pinPadModeContainer.left
				anchors.right: pinPadModeSwitch.left
				anchors.rightMargin: Constants.component_spacing
				anchors.verticalCenter: pinPadModeContainer.verticalCenter
				Text {
					id: nameText
					anchors.bottomMargin: Utils.dp(2)
					font.pixelSize: Constants.normal_font_size
					color: Constants.blue
					font.bold: true
					text: qsTr("PIN pad mode") + settingsModel.translationTrigger
				}

				Text {
					id: dateText
					width: parent.width
					anchors.top: nameText.bottom
					font.pixelSize: Constants.normal_font_size
					color: Constants.secondary_text
					text: qsTr("Enter PIN on this device") + settingsModel.translationTrigger
					wrapMode: Text.WordWrap
				}
			}

			GSwitch {
				id: pinPadModeSwitch
				anchors.right: pinPadModeContainer.right
				anchors.verticalCenter: pinPadModeContainer.verticalCenter
				initialState: settingsModel.pinPadMode
				onSwitched: settingsModel.pinPadMode = pinPadModeSwitch.isOn
			}
		}
	}
}
