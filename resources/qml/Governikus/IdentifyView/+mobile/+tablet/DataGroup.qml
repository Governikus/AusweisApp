/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	id: root

	signal scrollPageUp()
	signal scrollPageDown()

	readonly property alias count: repeater.count
	property alias title: dataTitle.text
	property alias titleStyle: dataTitle.textStyle
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

		PaneTitle {
			id: dataTitle

			height: implicitHeight * 1.5
			verticalAlignment: Text.AlignTop
		}

		Grid {
			id: grid
			columns: root.columns
			columnSpacing: Constants.pane_spacing
			width: parent.width
			verticalItemAlignment: Grid.AlignBottom
			flow: Grid.TopToBottom

			Repeater {
				id: repeater
				model: chat
				visible: repeater.count > 0

				Item {
					width: (grid.width - ((grid.columns - 1) * grid.columnSpacing)) / grid.columns
					height: 40

					GText {
						id: text

						Accessible.onScrollDownAction: baseItem.scrollPageDown()
						Accessible.onScrollUpAction: baseItem.scrollPageUp()

						anchors.verticalCenter: parent.verticalCenter
						anchors.left: parent.left
						anchors.right: checkBox.left

						text: name
						textStyle: writeRight ? Style.text.normal_warning : Style.text.normal_secondary
					}

					GSeparator {
						anchors.top: parent.bottom
						anchors.topMargin: -height
						width: parent.width
						visible: !(index === repeater.count - 1 || ((index + 1) % Math.ceil(repeater.count / grid.columns)) === 0)
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
							color: Style.color.accent
							opacity: parent.pressed ? 0.5 : 0
							Behavior on opacity { NumberAnimation { duration: 100 } }
						}
					}
				}
			}
		}
	}
}
