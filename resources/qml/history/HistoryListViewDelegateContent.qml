import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import "../global"

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
			anchors.verticalCenter: parent.verticalCenter
			anchors.left: showDetail ? parent.left : categoryImage.right
			anchors.leftMargin: showDetail ? Utils.dp(5) : 0
			anchors.right: detailsLink.left

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
					text: {
						if (!historyModelItem) {
							return ""
						}
						else if (Utils.isToday(historyModelItem.dateTime)) {
							return qsTr("today")
						}
						else if (Utils.isYesterday(historyModelItem.dateTime)) {
							return qsTr("yesterday")
						}
						else if (Utils.isThisWeek(historyModelItem.dateTime)) {
							return historyModelItem.dateTime.toLocaleString(Qt.locale(), qsTr("dddd"))
						}
						return historyModelItem.dateTime.toLocaleString(Qt.locale(), qsTr("MM/dd/yyyy"))
					}
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
					text: !!historyModelItem.purpose ? historyModelItem.purpose : qsTr("Touch for more details")
				}
			}
		}

		MouseArea {
			anchors.fill: parent
			onClicked: if (showDetail) {
							push(detailsHistoryView, { historyModelItem: historyModelItem })
						}
						else {
							historyModel.nameFilter.setProviderAddress(historyModelItem.providerAddress)
							push(providerHistoryView, { historyModelItem: historyModelItem })
						}
		}

		Item {
			id: detailsInfoLink

			anchors.right: parent.right
			anchors.verticalCenter:	parent.verticalCenter

			height: Utils.dp(16)
			width: height

			visible: showDetail

			Rectangle {
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter

				height: Utils.dp(16)

				border.color: PlatformConstants.grey_light
				border.width: 1
				radius: height

				color: PlatformConstants.grey_light
				Text {
					anchors.centerIn: parent

					text: "i"
					font.bold: true
					color: Constants.primary_text
				}
			}
		}

		HistoryDetails {
			id: detailsLink
			providerAddress: historyModelItem ? historyModelItem.providerAddress : ""
			listItemIndex: historyModelItem ? historyModelItem.index : -1
			listModel: baseItem.listModel
			visible: !showDetail
		}
	}
}
