import QtQuick 2.6

import "../global"


Item {
	id: baseItem

	property color backgroundColor: PlatformConstants.blue
	property string imageSource: ""
	property string itemText: ""

	width: parent.width

	Rectangle {
		anchors.fill: itemRow
		color: baseItem.backgroundColor
	}

	Row {
		id: itemRow

		height: parent.height
		width: parent.width
		padding: Utils.dp(10)
		spacing: Utils.dp(10)

		Rectangle {
			height: parent.height
			width: height
			color: baseItem.backgroundColor
			anchors.verticalCenter: parent.verticalCenter

			Image {
				source: baseItem.imageSource

				height: width
				width: parent.width * 0.5
				anchors.centerIn: parent
				fillMode: Image.PreserveAspectFit

			}
		}

		Rectangle {
			height: parent.height
			width: parent.width - parent.height
			color: baseItem.backgroundColor
			anchors.verticalCenter: parent.verticalCenter

			Text {
				text: baseItem.itemText !== "" ? baseItem.itemText : qsTr("Unknown")

				rightPadding: Utils.dp(10)
				width: parent.width - rightPadding
				font.pixelSize: Constants.normal_font_size
				color: "white"
				elide: Text.ElideRight
				anchors.verticalCenter: parent.verticalCenter
			}
		}
	}
}
