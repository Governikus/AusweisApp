import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Controls.Styles 1.4

import "../"
import "../global"


Item {
	property string title
	property var chat

	width: parent.width
	height: childrenRect.height
	visible: repeater.count > 0

	Text {
		id: header
		anchors.left: parent.left
		anchors.leftMargin: Utils.dp(20)

		height: Utils.dp(20)
		verticalAlignment: Text.AlignVCenter
		text: title
		color: Constants.grey
		font.pixelSize: Constants.header_font_size
		font.capitalization: Font.AllUppercase
		elide: Text.ElideRight
	}

	Column {
		id: column
		anchors.top: header.bottom
		width: parent.width

		Repeater {
			id: repeater
			model: chat

			Rectangle {
				width: parent.width
				height: Utils.dp(40)
				radius: 3
				color: "white"
				Text {
					id: dataGroup
					anchors.left: parent.left
					anchors.leftMargin: Utils.dp(20)
					anchors.verticalCenter: parent.verticalCenter
					anchors.rightMargin: anchors.leftMargin
					anchors.right: checkBox.left
					width: parent.width
					font.pixelSize: Constants.normal_font_size
					wrapMode: Text.WordWrap
					text: name
				}
				Rectangle {
					anchors.top: parent.bottom
					anchors.topMargin: -height
					height: 1
					anchors.left: dataGroup.left
					anchors.right: dataGroup.right
					color: Constants.grey
				}
				GCheckBox {
					id: checkBox
					anchors.right: parent.right
					anchors.rightMargin: Utils.dp(20)
					anchors.verticalCenter: parent.verticalCenter
					visible: optional
					checked: selected
				}

				MouseArea {
					anchors.fill: parent
					enabled: optional
					onClicked: selected = !selected
					Rectangle {
						anchors.fill: parent
						color: Constants.grey
						opacity: parent.pressed ? 0.5 : 0
						Behavior on opacity { NumberAnimation { duration: 100 } }
					}
				}
			}
		}
	}
	Rectangle {
		anchors.top: column.top
		height: 1
		width: column.width
		color: Constants.grey
	}
	Rectangle {
		anchors.top: column.bottom
		anchors.topMargin: -height
		height: 1
		width: column.width
		color: Constants.grey
	}
}
