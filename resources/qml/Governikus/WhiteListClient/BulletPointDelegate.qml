import QtQuick 2.10

import Governikus.Global 1.0

Item {
	width: parent.width
	height: content.height
	property alias text: content.text

	Text {
		id: bullet
		anchors.top: parent.top
		anchors.left: parent.left
		text: "\u2022"
		font.pixelSize: Constants.normal_font_size
		color: Constants.secondary_text
	}

	Text {
		id: content
		anchors.top: parent.top
		anchors.left: bullet.right
		anchors.leftMargin: Constants.groupbox_spacing
		anchors.right: parent.right
		anchors.rightMargin: Constants.groupbox_spacing

		font.pixelSize: Constants.normal_font_size
		color: Constants.secondary_text
		wrapMode: Text.WordWrap
	}
}
