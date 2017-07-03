import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import "../global"

Item {
	property bool showDetail: false

	property var listModel

	id: baseItem

	width: parent.width

	HistoryItemImage {
		id: categoryImage
		imageUrl: modelItem ? modelItem.provider_icon : ""
		visible: !showDetail
	}

	Rectangle {
		id: background
		color: "white"
		anchors.left: showDetail ? parent.left : categoryImage.right
		anchors.leftMargin: showDetail ? Utils.dp(5) : 0
		anchors.right: parent.right
		anchors.rightMargin: Utils.dp(5)
		anchors.top: parent.top
		anchors.bottom: parent.bottom

		Rectangle {
			anchors.verticalCenter: parent.verticalCenter
			anchors.left: parent.left
			anchors.leftMargin: Utils.dp(3)
			anchors.right: detailsLink.left

			Column {
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.right: parent.right

				spacing: Constants.history_delegate_spacing

				Text {
					id: dateTimeText

					verticalAlignment: Text.AlignVCenter
					font.pixelSize: Utils.sp(14)
					font.capitalization: Font.AllUppercase
					color: Constants.blue
					text: {
						if (!modelItem) {
							return "";
						}
						else if (Utils.isToday(modelItem.dateTime)) {
							return qsTr("today")
						}
						else if (Utils.isYesterday(modelItem.dateTime)) {
							return qsTr("yesterday")
						}
						else if (Utils.isThisWeek(modelItem.dateTime)) {
							return modelItem.dateTime.toLocaleString(Qt.locale(), qsTr("dddd"))
						}
						return modelItem.dateTime.toLocaleString(Qt.locale(), qsTr("MM/dd/yyyy"))
					}
				}
				Text {
					id: subjectText

					verticalAlignment: Text.AlignVCenter
					font.pixelSize: Utils.sp(14)
					elide: Text.ElideRight
					text: modelItem ? modelItem.subject : ""
				}
				Text {
					id: addressText

					verticalAlignment: Text.AlignVCenter
					font.pixelSize: Utils.sp(11)
					color: Constants.history_delegate_address_color
					elide: Text.ElideRight
					text: modelItem ? modelItem.provider_address_domain : ""
				}
			}
		}

		MouseArea {
			anchors.fill: parent
			onClicked: {
				if(showDetail) {
					push(detailsHistoryView, {modelItem: modelItem})
				} else {
					push(providerHistoryView, { modelItem: modelItem})
				}
			}
		}

		HistoryDetails {
			id: detailsLink
			providerAddress: modelItem ? modelItem.provider_address : ""
			listItemIndex: modelItem ? modelItem.index : -1
			listModel: baseItem.listModel
			visible: !showDetail
		}
	}
}
