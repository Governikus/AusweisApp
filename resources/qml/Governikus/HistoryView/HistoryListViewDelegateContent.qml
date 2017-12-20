import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import Governikus.Global 1.0

Item {
	id: baseItem

	property bool showDetail: false
	property var listModel

	Rectangle {
		id: background
		color: "white"
		anchors.left: parent.left
		anchors.leftMargin: Utils.dp(5)
		anchors.right: parent.right
		anchors.rightMargin: Utils.dp(5)
		anchors.top: parent.top
		anchors.bottom: parent.bottom

		HistoryItemImage {
			id: categoryImage
			imageUrl: historyModelItem ? historyModelItem.providerIcon : ""
			visible: !showDetail
		}

		Rectangle {
			id: purposeObject
			anchors.verticalCenter: parent.verticalCenter
			anchors.left: showDetail ? parent.left : categoryImage.right
			anchors.leftMargin: showDetail ? 0 : Utils.dp(15)
			anchors.right: deleteButton.left

			Column {
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.right: parent.right

				spacing: Constants.history_delegate_spacing

				Text {
					id: dateTimeText

					verticalAlignment: Text.AlignVCenter
					font.pixelSize: Constants.label_font_size
					font.capitalization: Font.AllUppercase
					color: Constants.blue
					text: (!historyModelItem ? "" :
						  Utils.isToday(historyModelItem.dateTime) ? qsTr("today") :
						  Utils.isYesterday(historyModelItem.dateTime) ? qsTr("yesterday") :
						  Utils.isThisWeek(historyModelItem.dateTime) ? historyModelItem.dateTime.toLocaleString(Qt.locale(), qsTr("dddd")) :
						  historyModelItem.dateTime.toLocaleString(Qt.locale(), qsTr("dd.MM.yyyy"))
						  ) + settingsModel.translationTrigger
				}
				Text {
					id: subjectText

					anchors.left: parent.left
					anchors.right: parent.right
					verticalAlignment: Text.AlignVCenter
					font.pixelSize: Constants.label_font_size
					wrapMode: Text.WordWrap
					text: historyModelItem ? historyModelItem.subject : ""
				}
				Text {
					id: purposeText

					anchors.left: parent.left
					anchors.right: parent.right
					verticalAlignment: Text.AlignVCenter
					font.pixelSize: Constants.small_font_size
					color: Constants.history_delegate_address_color
					wrapMode: Text.WordWrap
					text: historyModelItem ? !!historyModelItem.purpose ? historyModelItem.purpose : qsTr("Tap for more details") + settingsModel.translationTrigger : ""
				}
			}
		}

		Image {
			height: Utils.dp(35)
			source: "qrc:///images/android/arrowRight.svg"
			anchors.right: parent.right
			anchors.verticalCenter: purposeObject.verticalCenter
			anchors.rightMargin: Utils.dp(2)
			fillMode: Image.PreserveAspectFit
		}

		MouseArea {
			anchors.fill: parent
			onClicked: firePush(detailsHistoryView, { historyModelItem: historyModelItem })
		}

		MouseArea {
			id: deleteButton
			anchors.right: parent.right
			anchors.top: parent.top
			anchors.topMargin: Utils.dp(5)
			height: parent.height * 0.3
			width: height
			visible: PlatformConstants.is_layout_ios ? false : true

			onClicked: {
				if (typeof(listModel) === "object") {
					listModel.removeRows(historyModelItem ? historyModelItem.index : -1, 1)
				}
			}

			Image {
				height: Utils.dp(21)
				anchors.right: parent.right
				anchors.top: parent.top
				source: "qrc:///images/trash_icon.svg"
				fillMode: Image.PreserveAspectFit
			}
		}
	}
}
