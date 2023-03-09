/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0

Column {
	id: root

	property alias chat: repeater.model
	property alias columns: grid.columns
	readonly property alias count: repeater.count
	property alias title: dataTitle.text
	property alias titleStyle: dataTitle.textStyle

	spacing: Constants.pane_spacing
	visible: count > 0

	GText {
		id: dataTitle
		Accessible.name: dataTitle.text
		activeFocusOnTab: true
		textStyle: Style.text.header_accent

		FocusFrame {
		}
	}
	Grid {
		id: grid
		columnSpacing: Constants.pane_spacing
		flow: Grid.TopToBottom
		verticalItemAlignment: Grid.AlignBottom
		width: parent.width

		Repeater {
			id: repeater
			visible: count > 0

			Item {
				id: rightItem

				property alias checked: checkBox.checked

				Accessible.name: dataText.text + (optional ? ": " + (selected ? qsTr("selected") : qsTr("not selected")) : "")
				Accessible.role: optional ? Accessible.CheckBox : Accessible.StaticText
				activeFocusOnTab: true
				height: dataText.height * 1.5
				width: (grid.width - ((grid.columns - 1) * grid.columnSpacing)) / grid.columns

				Keys.onSpacePressed: if (optional)
					selected = !selected

				GText {
					id: dataText
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.rightMargin: checkBox.visible ? checkBox.width + Constants.pane_spacing : 0
					anchors.verticalCenter: parent.verticalCenter
					text: name
					textStyle: writeRight ? Style.text.normal_warning : Style.text.normal

					FocusFrame {
						marginFactor: 0.7
						scope: rightItem
					}
				}
				GSeparator {
					anchors.bottom: parent.bottom
					visible: !(index === repeater.count - 1 || ((index + 1) % Math.ceil(repeater.count / grid.columns)) === 0)
					width: parent.width
				}
				GCheckBox {
					id: checkBox
					activeFocusOnTab: false
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
						anchors.fill: parent
						color: Style.color.accent
						opacity: parent.pressed ? 0.5 : 0

						Behavior on opacity  {
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
