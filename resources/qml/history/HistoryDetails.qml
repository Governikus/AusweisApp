import QtQuick 2.5

import "../global"

Item {
	property string providerAddress: ""

	property int listItemIndex: -1

	property var listModel

	anchors.right: parent.right
	anchors.margins: Utils.dp(5)
	height: parent.height
	width: parent.height * 0.4

	Rectangle {
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter
		height: width

		border.color: Constants.history_delegate_address_color
		border.width: Utils.dp(1)
		radius: width

		Text {
			anchors.centerIn: parent
			text: "i"
			color: parent.border.color
		}
	}
}
