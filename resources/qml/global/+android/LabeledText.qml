import QtQuick 2.5

import "."

Item {
	property alias label: labelText.text
	property alias text: bodyText.text
	property int margin
	property int fontUppercase

	signal linkActivated(string link)

	height: childrenRect.height + margin

	Text {
		id: bodyText
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.leftMargin: margin
		anchors.right: parent.right
		anchors.rightMargin: margin
		font.pixelSize: Constants.normal_font_size
		font.capitalization: fontUppercase
		wrapMode: Text.WordWrap
		onLinkActivated: parent.linkActivated(link)
	}

	Text {
		id: labelText
		anchors.top: bodyText.bottom
		anchors.left: parent.left
		anchors.leftMargin: margin
		anchors.right: parent.right
		anchors.rightMargin: margin
		font.pixelSize: Utils.sp(14)
		color: Constants.blue
		wrapMode: Text.WordWrap
	}
}
