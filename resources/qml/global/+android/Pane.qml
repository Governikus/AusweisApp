import QtQuick 2.5
import QtGraphicalEffects 1.0

import "."


Rectangle {
	id: root
	property alias title: titleText.text
	default property alias paneChildren: paneContent.children
	readonly property real margin: Utils.dp(20)
	readonly property real spacing: Utils.dp(20)

	anchors.left: parent.left
	anchors.right: parent.right
	anchors.leftMargin: Utils.dp(10)
	anchors.rightMargin: Utils.dp(10)
	height: childrenRect.height
	color: "white"
	radius: 10

	Column {
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.leftMargin: root.margin
		anchors.rightMargin: root.margin

		Text {
			id: titleText
			height: implicitHeight * 2
			width: parent.width
			visible: text !== ""
			verticalAlignment: Text.AlignVCenter
			font.pixelSize: Constants.header_font_size
			color: Constants.blue
		}
		Item { width: parent.width; height: root.margin }
		Column {
			id: paneContent
			width: parent.width
			spacing: root.spacing
		}
		Item { width: parent.width; height: root.margin }
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
