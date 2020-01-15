/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0


Rectangle {
	id: root

	signal scrollPageUp()
	signal scrollPageDown()

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

		PaneTitle {
			height: implicitHeight * 1.5
			verticalAlignment: Text.AlignTop
			text: title
		}

		Rectangle {
			width: parent.width
			height: 40
			visible: repeater.count < 1
			GText {
				id: emptyText

				anchors.verticalCenter: parent.verticalCenter
				width: parent.width

				//: LABEL IOS_TABLET
				text: qsTr("No data requested") + SettingsModel.translationTrigger
				textStyle: Style.text.normal_secondary
			}
			GSeparator {
				anchors.top: parent.bottom
				anchors.topMargin: -height
				width: parent.width
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
					height: 40
					color: Constants.white

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
