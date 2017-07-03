import QtQuick 2.6

import "../global"


Rectangle {
	id: baseItem

	color: Constants.background_color

	property string description: ""

	readonly property int frameThickness: Utils.dp(10)

	Rectangle {
		radius: Utils.dp(5)
		height: baseItem.height - 2 * baseItem.frameThickness
		width: baseItem.width - 2 * baseItem.frameThickness
		anchors.top: parent.top
		anchors.topMargin: baseItem.frameThickness
		anchors.left: parent.left
		anchors.leftMargin: baseItem.frameThickness

		color: "white"

		Column {
			id: descriptionContent

			spacing: baseItem.frameThickness
			width: parent.width - 2 * parent.radius
			anchors.top: parent.top
			anchors.topMargin: parent.radius
			anchors.left: parent.left
			anchors.leftMargin: parent.radius

			Text {
				font.pixelSize: Constants.header_font_size
				color: PlatformConstants.blue_primary
				text: qsTr("Description")
			}

			Text {
				font.pixelSize: Constants.normal_font_size
				text: baseItem.description
				maximumLineCount: 3
				width: parent.width
				elide: Text.ElideRight
				wrapMode: Text.Wrap
			}
		}
	}
}
