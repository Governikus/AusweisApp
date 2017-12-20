import QtQuick 2.5

import "."


Column {
	id: root
	property alias title: titleText.text
	property alias spacing: paneContent.spacing
	default property alias paneData: paneContent.data

	anchors.left: parent.left
	anchors.right: parent.right

	Text {
		id: titleText
		height: Utils.dp(30)
		visible: text !== ""
		anchors.left: parent.left
		anchors.leftMargin: Constants.pane_padding
		font.pixelSize: Constants.header_font_size
		font.capitalization: Font.AllUppercase
		color: Constants.grey
	}

	Rectangle { width: parent.width; height: 1; color: Constants.grey}
	Rectangle {
		color: "white"
		width: parent.width
		height: childrenRect.height

		Column {
			width: parent.width
			Item { width: parent.width; height: Constants.pane_padding }
			Column {
				id: paneContent
				anchors.left: parent.left
				anchors.leftMargin: Constants.pane_padding
				anchors.right: parent.right
				anchors.rightMargin: Constants.pane_padding
				spacing: Constants.component_spacing
			}
			Item { width: parent.width; height: Constants.pane_padding }
		}
	}
	Rectangle { width: parent.width; height: 1; color: Constants.grey}
}
