import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0

Item {
	id: baseItem
	property var historyModelItem: historyModel

	height: swipeComponent.height
	width: parent.width

	signal clicked()

	function reactToEditModeChanged(pNewValue){
		if (pNewValue){
			swipeComponent.swipe.open(SwipeDelegate.Right)
		} else {
			swipeComponent.swipe.close()
		}
	}

	SwipeDelegate {
		id: swipeComponent
		height: Constants.history_section_height
		anchors.left: parent.left
		anchors.right: parent.right

		onClicked: {
			baseItem.clicked()
		}

		contentItem: Rectangle {
			id: background
			color: "white"
			anchors.top: parent.top
			anchors.bottom: parent.bottom

			HistoryItemImage {
				id: categoryImage
				imageUrl: providerIcon
				visible: true
			}

			Rectangle {
				id: purposeObject
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: categoryImage.right
				anchors.leftMargin: Utils.dp(15)
				anchors.right: parent.right

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
						text: (Utils.isToday(dateTime) ? qsTr("today") :
							  Utils.isYesterday(dateTime) ? qsTr("yesterday") :
							  Utils.isThisWeek(dateTime) ? dateTime.toLocaleString(Qt.locale(), qsTr("dddd")) :
							  dateTime.toLocaleString(Qt.locale(), qsTr("dd.MM.yyyy"))
							  ) + settingsModel.translationTrigger
					}
					Text {
						id: subjectText

						anchors.left: parent.left
						anchors.right: parent.right
						verticalAlignment: Text.AlignVCenter
						font.pixelSize: Constants.label_font_size
						wrapMode: Text.WordWrap
						text: subject
					}
					Text {
						id: purposeText

						anchors.left: parent.left
						anchors.right: parent.right
						verticalAlignment: Text.AlignVCenter
						font.pixelSize: Constants.small_font_size
						color: Constants.history_delegate_address_color
						wrapMode: Text.WordWrap
						text: !!purpose ? historyModelItem.purpose : qsTr("Tap for more details") + settingsModel.translationTrigger
					}
				}
			}

			Image {
				height: Utils.dp(35)
				source: "qrc:///images/arrowRight.svg"
				anchors.right: parent.right
				anchors.verticalCenter: purposeObject.verticalCenter
				anchors.rightMargin: Utils.dp(2)
				fillMode: Image.PreserveAspectFit
			}
		}

		swipe.right: Rectangle {
			id: confirmDeletionRect
			height: parent.height
			width: parent.width / 4
			anchors.right: parent.right
			anchors.top: parent.top

			color: Constants.red
			border.width: Constants.groupbox_spacing
			border.color: Constants.white

			MouseArea {
				anchors.fill: parent
				onClicked: {
					historyModel.removeRows(index, 1)
				}
			}

			Image {
				anchors.fill: parent
				anchors.margins: Utils.dp(20)
				source: "qrc:///images/trash_icon_white.svg"
				fillMode: Image.PreserveAspectFit
			}
		}
	}
}

