import QtQuick 2.10

import "Utils.js" as Utils

Column {
	id: root
	property alias title: titleText.text
	property alias spacing: paneContent.spacing
	default property alias paneData: paneContent.data

	anchors.left: parent.left
	anchors.right: parent.right

	PaneTitle {
		id: titleText
		height: Utils.dp(30)
		anchors.margins: Constants.pane_padding
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
