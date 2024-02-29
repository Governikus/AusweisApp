/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type.ApplicationModel
import Governikus.Type.SettingsModel

AbstractButton {
	id: root

	Accessible.name: ApplicationModel.stripHtmlTags(text)
	Layout.fillHeight: true
	Layout.fillWidth: true
	implicitHeight: 1
	implicitWidth: 1
	padding: 2 * Constants.pane_padding

	background: GPaneBackground {
		id: pane

		opacity: SettingsModel.showBetaTesting ? 0.9 : 1.0

		FocusFrame {
			marginFactor: 0.8
			radius: parent.radius * 1.2
			scope: root
		}
	}
	contentItem: Column {
		spacing: Constants.component_spacing

		TintableIcon {
			id: image

			source: root.icon.source
			sourceSize.height: Style.dimens.huge_icon_size
			tintColor: Style.color.text_title_focus
		}
		GText {
			id: label

			color: Style.color.text_title
			horizontalAlignment: Text.AlignLeft
			text: root.text
			textStyle: Style.text.tile
			width: parent.width
		}
	}

	Item {
		id: d

		states: [
			State {
				name: "pressed"
				when: root.pressed

				PropertyChanges {
					image.tintColor: Style.color.text_title_pressed
					label.color: Style.color.text_title_pressed
					pane.border.color: Style.color.pane_border_pressed
					pane.color: Style.color.pane_pressed
				}
			},
			State {
				name: "hovered"
				when: root.hovered

				PropertyChanges {
					image.tintColor: Style.color.text_title_hovered
					label.color: Style.color.text_title_hovered
					pane.border.color: Style.color.pane_border_hovered
					pane.color: Style.color.pane_hovered
				}
			},
			State {
				name: "focus"
				when: root.activeFocus && plugin.showFocusIndicator

				PropertyChanges {
					image.tintColor: Style.color.text_title_focus
					label.color: Style.color.text_title_focus
				}
			}
		]
	}
}
