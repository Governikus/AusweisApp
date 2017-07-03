import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Controls.Styles 1.4

import "../"
import "../global"


Rectangle {
	id: root
	property string title;
	property var chat

	width: parent.width
	height: column.height
	color: "white"

	Column {
		id: column
		anchors.verticalCenter: parent.verticalCenter
		anchors.left: parent.left
		anchors.leftMargin: Utils.dp(20)
		anchors.right: parent.right
		anchors.rightMargin: anchors.leftMargin

		Text {
			height: implicitHeight * 2
			verticalAlignment: Text.AlignVCenter
			text: title
			color: Constants.blue
			font.pixelSize: Constants.header_font_size
			elide: Text.ElideRight
		}

		Rectangle {
			width: parent.width
			height: Utils.dp(40)
			visible: repeater.count < 1
			Text {
				id: emptyText
				anchors.verticalCenter: parent.verticalCenter
				width: parent.width
				font.pixelSize: Utils.sp(14)
				text: qsTr("No data available")
			}
			Rectangle {
				anchors.top: parent.bottom
				anchors.topMargin: -height
				height: 1
				width: parent.width
				color: Constants.grey
			}
		}

		Repeater {
			id: repeater
			model: chat
			visible: repeater.count > 0

			Rectangle {
				width: parent.width
				height: Utils.dp(40)
				color: "white"
				Text {
					id: text
					anchors.verticalCenter: parent.verticalCenter
					width: parent.width
					font.pixelSize: Utils.sp(14)
					text: name
				}
				Rectangle {
					anchors.top: parent.bottom
					anchors.topMargin: -height
					height: 1
					width: parent.width
					color: Constants.grey
				}
				CheckBox {
					anchors.right: parent.right
					anchors.verticalCenter: parent.verticalCenter
					visible: optional
					checked: selected
				}

				MouseArea {
					anchors.fill: parent
					enabled: optional
					onClicked: selected = !selected
					Rectangle {
						anchors.centerIn: parent
						width: root.width
						height: parent.height
						color: Constants.accent_color
						opacity: parent.pressed ? 0.5 : 0
						Behavior on opacity { NumberAnimation { duration: 100 } }
					}
				}
			}
		}
	}
}
