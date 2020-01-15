/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0


Column {
	id: root

	readonly property alias count: repeater.count
	property alias title: dataTitle.text
	property alias columns: grid.columns
	property alias chat: repeater.model

	visible: count > 0
	spacing: Constants.pane_spacing

	GText {
		id: dataTitle

		activeFocusOnTab: true
		Accessible.name: dataTitle.text

		textStyle: Style.text.header_accent

		FocusFrame {
			borderColor: Style.color.focus_indicator
		}
	}

	Item {
		visible: count < 1
		width: parent.width
		height: noDataText * 1.5

		GText {
			id: noDataText

			width: parent.width
			anchors.verticalCenter: parent.verticalCenter

			activeFocusOnTab: true
			Accessible.name: noDataText.text

			//: LABEL DESKTOP_QML
			text: qsTr("No data requested") + SettingsModel.translationTrigger
			textStyle: Style.text.normal
		}

		GSeparator {
			width: parent.width
			anchors.bottom: parent.bottom
		}
	}

	Grid {
		id: grid

		width: parent.width

		columnSpacing: Constants.pane_spacing
		verticalItemAlignment: Grid.AlignBottom

		Repeater {
			id: repeater

			visible: count > 0

			Item {
				id: rightItem

				property alias checked: checkBox.checked

				width: (grid.width - ((grid.columns - 1) * grid.columnSpacing)) / grid.columns
				height: dataText.height * 1.5

				activeFocusOnTab: true
				Accessible.role: optional ? Accessible.CheckBox : Accessible.StaticText
				Accessible.name: dataText.text + (optional ? ": " + (
										 selected ?
										 qsTr("selected") :
										 qsTr("not selected")
								 ) : "") + SettingsModel.translationTrigger

				Keys.onSpacePressed: if (optional) selected = !selected

				GText {
					id: dataText

					anchors.verticalCenter: parent.verticalCenter
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.rightMargin: checkBox.visible ? checkBox.width + Constants.pane_spacing : 0

					text: name
					textStyle: writeRight ? Style.text.normal_warning : Style.text.normal

					FocusFrame {
						scope: rightItem
						marginFactor: 0.7
						borderColor: Style.color.focus_indicator
					}
				}

				GSeparator {
					width: parent.width
					anchors.bottom: parent.bottom
				}

				GCheckBox {
					id: checkBox

					visible: optional
					anchors.right: parent.right
					anchors.verticalCenter: parent.verticalCenter

					activeFocusOnTab: false

					checked: selected
				}

				MouseArea {
					anchors.fill: parent

					enabled: optional
					onClicked: selected = !selected

					Rectangle {
						anchors.fill: parent

						color: Style.color.accent
						opacity: parent.pressed ? 0.5 : 0

						Behavior on opacity { NumberAnimation { duration: 100 } }
					}
				}
			}
		}
	}
}
