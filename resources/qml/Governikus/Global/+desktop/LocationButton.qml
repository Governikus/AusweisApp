/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type.SettingsModel

AbstractButton {
	id: root

	property alias image: icon.source
	property string language
	readonly property bool selected: SettingsModel.language === language

	Accessible.checkable: true
	Accessible.checked: selected
	Layout.maximumWidth: Number.POSITIVE_INFINITY
	padding: Constants.groupbox_spacing

	background: GPaneBackground {
		id: pane

		drawShadow: false

		FocusFrame {
			marginFactor: 0.8
			radius: parent.radius * 1.2
			scope: root
		}
	}
	contentItem: ColumnLayout {
		spacing: Constants.component_spacing

		Image {
			id: icon

			Layout.alignment: Qt.AlignHCenter
			sourceSize.height: Style.dimens.icon_size
		}
		GText {
			id: description

			Layout.alignment: Qt.AlignHCenter
			text: root.text
		}
	}

	onClicked: SettingsModel.language = language

	Item {
		id: d

		states: [
			State {
				name: "pressed"
				when: root.pressed

				PropertyChanges {
					description.color: Style.color.text_pressed
					pane.border.color: Style.color.pane_border_pressed
					pane.color: Style.color.pane_pressed
				}
			},
			State {
				name: "hovered"
				when: root.hovered

				PropertyChanges {
					description.color: Style.color.text_hovered
					pane.border.color: Style.color.pane_border_hovered
					pane.color: Style.color.pane_hovered
				}
			},
			State {
				name: "selected"
				when: root.selected

				PropertyChanges {
					description.color: Style.color.text_active
					pane.border.color: Style.color.pane_border_active
					pane.color: Style.color.pane_active
				}
			}
		]
	}
}
