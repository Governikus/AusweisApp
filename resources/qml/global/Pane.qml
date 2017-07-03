import QtQuick 2.5

import "."


Column {
	id: root
	property alias title: titleText.text
	default property alias paneData: paneContent.data
	readonly property real margin: Utils.dp(20)
	readonly property real spacing: Utils.dp(20)
	readonly property color color: "white"

	width: parent.width
	height: childrenRect.height

	Text {
		id: titleText
		height: Utils.dp(30)
		visible: text !== ""
		anchors.left: parent.left
		anchors.leftMargin: root.margin
		font.pixelSize: Constants.header_font_size
		font.capitalization: Font.AllUppercase
		color: Constants.grey
	}

	Rectangle { width: parent.width; height: 1; color: Constants.grey}
	Rectangle {
		color: root.color
		width: parent.width
		height: childrenRect.height

		Column {
			width: parent.width
			Item { width: parent.width; height: root.margin }
			Column {
				id: paneContent
				anchors.left: parent.left
				anchors.leftMargin: root.margin
				anchors.right: parent.right
				anchors.rightMargin: root.margin
				spacing: root.spacing
			}
			Item { width: parent.width; height: root.margin }
		}
	}
	Rectangle { width: parent.width; height: 1; color: Constants.grey}
}
