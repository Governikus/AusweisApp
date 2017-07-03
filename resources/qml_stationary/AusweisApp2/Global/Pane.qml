import QtQuick 2.5
import QtGraphicalEffects 1.0

import "."


Rectangle {
	id: root
	property alias title: titleText.text
	default property alias paneChildren: paneContent.children

	anchors.left: parent.left
	anchors.right: parent.right
	height: childrenRect.height
	color: "white"
	radius: 16

	Column {
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.leftMargin: Constants.pane_padding
		anchors.rightMargin: Constants.pane_padding

		Text {
			id: titleText
			height: implicitHeight * 2
			width: parent.width
			visible: text !== ""
			verticalAlignment: Text.AlignVCenter
			font.pixelSize: Constants.header_font_size
			color: Constants.blue
		}
		Item { width: parent.width; height: Constants.pane_padding }
		Column {
			id: paneContent
			width: parent.width
			spacing: Constants.pane_spacing
		}
		Item { width: parent.width; height: Constants.pane_padding }
	}

	layer.enabled: true
	layer.effect: DropShadow {
					radius: 8
					samples: 8
					source: root
					color: Constants.grey
					verticalOffset: 2
				}
}
