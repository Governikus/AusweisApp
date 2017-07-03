import QtQuick 2.5

import "global"

Item
{
	property string text
	property alias font: textItem.font
	readonly property alias contentWidth: invisibleText.contentWidth
	signal clicked

	id: root
	width: parent ? parent.width : undefined
	height: parent ? parent.height : undefined
	anchors.verticalCenter: parent ? parent.verticalCenter : undefined

	Behavior on text {
		SequentialAnimation {
			PropertyAnimation { target: root; property: "opacity"; to: 0; duration: 150 }
			PropertyAction { target: root; property: "text" }
			PropertyAnimation { target: root; property: "opacity"; to: 1; duration: 150 }
		}
	}

	Text {
		id: invisibleText
		visible: false
		text: textItem.text
		font.pointSize: textItem.font.pointSize
	}

	Text {
		id: textItem
		anchors.verticalCenter: parent.verticalCenter
		anchors.horizontalCenter: plugin.platformStyle.indexOf("android") !== -1 ? undefined : parent.horizontalCenter
		color: "white"
		font.pixelSize: Constants.titlebar_font_size
		width: Math.min(parent.width, invisibleText.contentWidth)
		height: contentHeight
		elide: invisibleText.contentWidth > width ? Text.ElideRight : Text.ElideNone
		text: root.text !== "" ? root.text :
				root.state === "cancel" ? qsTr("Cancel") :
				root.state === "edit" ? qsTr("Edit") :
				root.state === "back" ? qsTr("< back") : ""
	}

	MouseArea {
		anchors.fill: parent
		onClicked: root.clicked()
	}
}
