import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Controls.Styles 1.4

import Governikus.Global 1.0


Rectangle {
	id: root
	property string title;
	property int columns: 1
	property var chat

	width: parent.width
	height: column.height
	visible: repeater.count > 0

	Column {
		id: column
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right

		Text {
			height: implicitHeight * 1.5
			verticalAlignment: Text.AlignTop
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
				font.pixelSize: Constants.normal_font_size
				text: qsTr("No data requested") + settingsModel.translationTrigger
			}
			Rectangle {
				anchors.top: parent.bottom
				anchors.topMargin: -height
				height: 1
				width: parent.width
				color: Constants.grey
			}
		}

		Grid {
			id: grid
			columns: root.columns
			columnSpacing: Constants.pane_spacing
			width: parent.width
			verticalItemAlignment: Grid.AlignBottom

			Repeater {
				id: repeater
				model: chat
				visible: repeater.count > 0

				Rectangle {
					width: (grid.width - ((grid.columns - 1) * grid.columnSpacing)) / grid.columns
					height: Utils.dp(40)
					color: "white"

					Text {
						id: text
						anchors.verticalCenter: parent.verticalCenter
						anchors.left: parent.left
						anchors.right: checkBox.left
						font.pixelSize: Constants.normal_font_size
						text: name
						wrapMode: Text.WordWrap
					}

					Rectangle {
						anchors.top: parent.bottom
						anchors.topMargin: -height
						height: 1
						width: parent.width
						color: Constants.grey
					}

					GCheckBox {
						id: checkBox
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
}
