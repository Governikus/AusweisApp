/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Global
import Governikus.Style

Item {
	id: root

	property var chat
	property int columns: 1
	readonly property alias count: repeater.count
	property alias title: dataTitle.text
	property alias titleStyle: dataTitle.textStyle
	property bool writeAccess: false

	signal scrollPageDown
	signal scrollPageUp

	height: column.height
	visible: repeater.count > 0
	width: parent.width

	Column {
		id: column

		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top

		PaneTitle {
			id: dataTitle

			color: writeAccess ? Style.color.text_warning : titleStyle.textColor
			height: implicitHeight * 1.5
			verticalAlignment: Text.AlignTop
			width: parent.width
		}
		Grid {
			id: grid

			columnSpacing: Constants.pane_spacing
			columns: root.columns
			flow: Grid.TopToBottom
			verticalItemAlignment: Grid.AlignBottom
			width: parent.width

			Repeater {
				id: repeater

				model: chat
				visible: repeater.count > 0

				Item {
					Accessible.checkable: optional
					Accessible.checked: checkBox.checked
					Accessible.name: name
					Accessible.role: Accessible.ListItem
					height: text.implicitHeight + 2 * Constants.text_spacing
					width: (grid.width - ((grid.columns - 1) * grid.columnSpacing)) / grid.columns

					Accessible.onPressAction: if (optional)
						selected = !selected
					Accessible.onScrollDownAction: baseItem.scrollPageDown()
					Accessible.onScrollUpAction: baseItem.scrollPageUp()

					GText {
						id: text

						Accessible.ignored: true
						anchors.left: parent.left
						anchors.right: checkBox.left
						anchors.verticalCenter: parent.verticalCenter
						text: name
						textStyle: writeRight ? Style.text.normal_warning : Style.text.normal
					}
					GSeparator {
						anchors.top: parent.bottom
						anchors.topMargin: -height
						visible: !(index === repeater.count - 1 || ((index + 1) % Math.ceil(repeater.count / grid.columns)) === 0)
						width: parent.width
					}
					GCheckBox {
						id: checkBox

						Accessible.ignored: true
						anchors.right: parent.right
						anchors.verticalCenter: parent.verticalCenter
						checked: selected
						visible: optional
					}
					MouseArea {
						anchors.fill: parent
						enabled: optional

						onClicked: selected = !selected

						Rectangle {
							anchors.centerIn: parent
							color: Style.color.control
							height: parent.height
							opacity: parent.pressed ? 0.5 : 0
							width: root.width

							Behavior on opacity {
								NumberAnimation {
									duration: 100
								}
							}
						}
					}
				}
			}
		}
	}
}
