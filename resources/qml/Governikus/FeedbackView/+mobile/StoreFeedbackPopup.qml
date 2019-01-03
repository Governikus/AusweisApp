import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0

import Governikus.Type.ApplicationModel 1.0


Popup {
	id: popup

	modal: true
	focus: true
	closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

	width: Utils.dp(350)
	height: contentColumn.height + 2 * Constants.pane_padding
	padding: Constants.pane_padding

	Column {
		id: contentColumn
		width: parent.width
		spacing: Constants.pane_spacing

		Text {
			id: header
			color: Constants.secondary_text
			font.pixelSize: Constants.normal_font_size
			font.bold: true
			wrapMode: Text.WordWrap
			text: qsTr("Would you like to rate this app?") + settingsModel.translationTrigger
		}

		Text {
			id: info
			color: Constants.secondary_text
			width: parent.width
			wrapMode: Text.WordWrap
			font.pixelSize: Constants.normal_font_size
			text: qsTr("We would be very grateful if you could leave a rating on the Google Play Store!") + settingsModel.translationTrigger
		}

		Row {
			width: parent.width
			spacing: Constants.pane_spacing

			GButton {
				text: qsTr("No, Thanks") + settingsModel.translationTrigger
				width: (parent.width - Constants.pane_spacing) / 2

				onClicked: {
					close()
				}
			}

			GButton {
				text: qsTr("Rate") + settingsModel.translationTrigger
				width: (parent.width - Constants.pane_spacing) / 2

				onClicked: {
					Qt.openUrlExternally("market://details?id=" + ApplicationModel.packageName)
					close()
				}
			}
		}
	}
}
