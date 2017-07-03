import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

import "../"
import "../global"

Rectangle {
	id: rootItem
	property int currentIndex

	anchors.horizontalCenter: parent.horizontalCenter
	width: descriptionTab.width + contactTab.width + 2 * border.width
	height: descriptionTab.height  + 2 * border.width

	border.color: Constants.blue
	border.width: Utils.dp(1)
	radius: Utils.dp(3)
	clip: true

	Row {
		id: row
		readonly property int maxContentWidth: Math.max(descriptionText.contentWidth, contactText.contentWidth)

		anchors.centerIn: parent
		Rectangle {
			id: descriptionTab
			width: row.maxContentWidth + Utils.dp(6)
			height: descriptionText.contentHeight + Utils.dp(6)
			color: rootItem.currentIndex === 0 ? Constants.blue : "white"
			Text {
				id: descriptionText
				anchors.centerIn: parent
				color: rootItem.currentIndex === 0 ? "white" : Constants.blue
				text: qsTr("Contact")
			}
			MouseArea {
				anchors.fill: parent
				onClicked: rootItem.currentIndex = 0
			}
		}
		Rectangle {
			id: contactTab
			width: row.maxContentWidth + Utils.dp(6)
			height: contactText.contentHeight + Utils.dp(6)
			color: rootItem.currentIndex === 1 ? Constants.blue : "white"
			Text {
				id: contactText
				anchors.centerIn: parent
				color: rootItem.currentIndex === 1 ? "white" : Constants.blue
				text: qsTr("History")
			}
			MouseArea {
				anchors.fill: parent
				onClicked: rootItem.currentIndex = 1
			}
		}
	}
}
