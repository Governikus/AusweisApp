/*
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0
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

		GText {
			id: errorMsg

			visible: text !== ""
			width: parent.usableWidth

			Accessible.role: Accessible.StaticText
			Accessible.name: text

			verticalAlignment: Text.AlignVCenter
			text: ""
			textStyle: Style.text.normal_warning
		}

		Column {
			id: nameContainer
			width: parent.usableWidth

			GText {
				Accessible.role: Accessible.StaticText
				Accessible.name: text

				//: LABEL ANDROID IOS
				text: qsTr("Device name") + SettingsModel.translationTrigger
				textStyle: Style.text.normal_accent
			}

			GText {
				width: parent.width

				Accessible.role: Accessible.StaticText
				Accessible.name: text

				//: LABEL ANDROID IOS
				text: qsTr("Set device name:") + SettingsModel.translationTrigger
				textStyle: Style.text.normal_secondary
			}

			Item {
				id: spacing
				height: 5
				width: height
			}

			GTextField {
				id: serverName
				width: parent.width
				onAccepted: {
					SettingsModel.serverName = text
					text = SettingsModel.serverName
					nameContainer.forceActiveFocus(Qt.MouseFocusReason)
				}
				onVisibleChanged: {
					nameContainer.forceActiveFocus(Qt.MouseFocusReason)
					if (visible) text = SettingsModel.serverName
				}
			}
		}

		Item {
			id: pinPadModeContainer

			width: parent.usableWidth
			height: Math.max(pinPadModeText.height, pinPadModeSwitch.height)

			Item {
				id: pinPadModeText

				height: nameText.height + dateText.height
				anchors.left: pinPadModeContainer.left
				anchors.right: pinPadModeSwitch.left
				anchors.rightMargin: Constants.component_spacing
				anchors.verticalCenter: pinPadModeContainer.verticalCenter
				GText {
					id: nameText

					anchors.bottomMargin: 2

					Accessible.role: Accessible.StaticText
					Accessible.name: text

					//: LABEL ANDROID IOS
					text: qsTr("PIN pad mode") + SettingsModel.translationTrigger
					textStyle: Style.text.normal_accent
				}

				GText {
					id: dateText

					width: parent.width
					anchors.top: nameText.bottom

					Accessible.role: Accessible.StaticText
					Accessible.name: text

					//: LABEL ANDROID IOS
					text: qsTr("Enter PIN on this device") + SettingsModel.translationTrigger
					textStyle: Style.text.normal_secondary
				}
			}

			GSwitch {
				id: pinPadModeSwitch
				anchors.right: pinPadModeContainer.right
				anchors.verticalCenter: pinPadModeContainer.verticalCenter
				initialState: SettingsModel.pinPadMode
				onSwitched: SettingsModel.pinPadMode = pinPadModeSwitch.isOn
			}
		}
	}
}
