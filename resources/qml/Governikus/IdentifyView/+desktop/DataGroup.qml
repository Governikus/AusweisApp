import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Type.ApplicationModel 1.0


Column {
	id: column
	visible: count > 0
	spacing: Constants.pane_spacing

	property alias title: dataTitle.text
	property alias columns: grid.columns
	property alias chat: repeater.model
	readonly property alias count: repeater.count

	PaneTitle {
		id: dataTitle
	}

	Item {
		width: parent.width
		height: noDataText * 1.5
		visible: count < 1

		Text {
			id: noDataText
			color: Constants.black
			anchors.verticalCenter: parent.verticalCenter
			width: parent.width
			font.pixelSize: Constants.normal_font_size
			wrapMode: Text.WordWrap
			text: qsTr("No data requested") + settingsModel.translationTrigger
		}

		Rectangle {
			anchors.bottom: parent.bottom
			height: Math.max(ApplicationModel.scaleFactor * 1, 1)
			width: parent.width
			color: Constants.grey
		}
	}

	Grid {
		id: grid
		columnSpacing: Constants.pane_spacing
		width: parent.width
		verticalItemAlignment: Grid.AlignBottom

		Repeater {
			id: repeater
			visible: count > 0

			Item {
				width: (grid.width - ((grid.columns - 1) * grid.columnSpacing)) / grid.columns
				height: dataText.height * 1.5

				Text {
					id: dataText
					color: Constants.black
					anchors.verticalCenter: parent.verticalCenter
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.rightMargin: checkBox.visible ? checkBox.width + Constants.pane_spacing : 0
					font.pixelSize: Constants.normal_font_size
					text: name
					wrapMode: Text.WordWrap
				}

				Rectangle {
					anchors.bottom: parent.bottom
					height: Math.max(ApplicationModel.scaleFactor * 1, 1)
					width: parent.width
					color: Constants.grey
				}

				GCheckBox {
					id: checkBox
					anchors.right: parent.right
					anchors.verticalCenter: parent.verticalCenter
					height: parent.height / 2
					width: height
					visible: optional
					checked: selected
				}

				MouseArea {
					anchors.fill: parent
					enabled: optional
					onClicked: selected = !selected

					Rectangle {
						anchors.fill: parent
						color: Constants.accent_color
						opacity: parent.pressed ? 0.5 : 0
						Behavior on opacity { NumberAnimation { duration: 100 } }
					}
				}
			}
		}
	}
}
