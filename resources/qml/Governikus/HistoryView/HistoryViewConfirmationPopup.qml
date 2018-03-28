import QtQuick 2.7
import QtQuick.Controls 2.2

import Governikus.Global 1.0

Popup {
	id: popupObject
	property string timePeriod: "UNKNOWN"
	property string popupDescriptionString: ""
	property var baseItem: Item { }
	x: (baseItem.width - width) / 2
	y: (baseItem.height - height) / 2

	function setValues(pPeriodToRemove, pNewDescriptionString) {
		timePeriod = pPeriodToRemove
		popupDescriptionString = pNewDescriptionString
	}

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
			text: qsTr("Delete history") + settingsModel.translationTrigger
			font.pixelSize: Constants.header_font_size
			font.bold: true
		}

		Text {
			id: info
			color: Constants.secondary_text
			width: parent.width - 2 * Constants.pane_padding
			wrapMode: Text.WordWrap
			font.pixelSize: Constants.normal_font_size
			text: popupObject.popupDescriptionString
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
					text: qsTr("Delete") + settingsModel.translationTrigger
					color: Constants.blue
					font.pixelSize: Constants.titlebar_font_size
				}

				onClicked: {
					var removedItems = settingsModel.removeHistory(deleteHistoryConfirmationDialog.timePeriod);
					qmlExtension.showFeedback(qsTr("Removed %1 entries from the history.").arg(removedItems))
					popupObject.close()
				}
			}
		}
	}
}

