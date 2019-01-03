import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0

Popup {
	id: popupObject
	signal confirmed

	property var baseItem: Item { }
	x: (baseItem.width - width) / 2
	y: (baseItem.height - height) / 2

	property alias title: header.text
	property alias text: info.text
	property alias confirmText: textItemDelete.text

	modal: true
	focus: true
	closePolicy: Popup.CloseOnPressOutside | Popup.CloseOnEscape
	width: Utils.dp(250)
	height: popupColumn.height

	Column {
		id: popupColumn
		width: parent.width
		spacing: Constants.pane_spacing
		padding: Constants.pane_padding

		Text {
			id: header
			color: Constants.secondary_text
			font.pixelSize: Constants.header_font_size
			font.bold: true
		}

		Text {
			id: info
			color: Constants.secondary_text
			width: parent.width - 2 * Constants.pane_padding
			wrapMode: Text.WordWrap
			font.pixelSize: Constants.normal_font_size
		}

		Row {
			id: buttonBox
			anchors.right: parent.right
			anchors.rightMargin: Constants.pane_padding
			spacing: Constants.pane_spacing

			MouseArea {
				height: textItemCancel.height + 2 * Utils.dp(10)
				width: textItemCancel.width + 2 * Utils.dp(10)

				Text {
					id: textItemCancel
					anchors.centerIn: parent
					text: qsTr("Cancel") + settingsModel.translationTrigger
					color: Constants.blue
					font.pixelSize: Constants.titlebar_font_size
				}

				onClicked: {
					popupObject.close()
				}
			}

			MouseArea {
				height: textItemDelete.height + 2 * Utils.dp(10)
				width: textItemDelete.width + 2 * Utils.dp(10)

				Text {
					id: textItemDelete
					anchors.centerIn: parent
					color: Constants.blue
					font.pixelSize: Constants.titlebar_font_size
				}

				onClicked: {
					popupObject.confirmed()
					popupObject.close()
				}
			}
		}
	}
}

