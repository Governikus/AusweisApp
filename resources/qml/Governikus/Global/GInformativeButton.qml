/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Style
import Governikus.View

AbstractButton {
	id: root

	property alias description: description.text

	Accessible.name: text + ". " + description
	horizontalPadding: Constants.component_spacing
	verticalPadding: Constants.component_spacing

	background: GPaneBackground {
		id: pane

		FocusFrame {
			marginFactor: 0.8
			radius: parent.radius * 1.2
			scope: root
		}
	}
	contentItem: RowLayout {
		id: rowLayout

		spacing: Constants.component_spacing

		TintableIcon {
			source: root.icon.source
			sourceSize.height: Constants.is_desktop ? Style.dimens.icon_size : Style.dimens.small_icon_size
			tintColor: title.color
		}
		ColumnLayout {
			Layout.maximumWidth: Number.POSITIVE_INFINITY
			spacing: Constants.text_spacing / 2

			GText {
				id: title

				Accessible.ignored: true
				elide: Text.ElideRight
				text: root.text
				textStyle: Style.text.subline
			}
			GText {
				id: description

				Accessible.ignored: true
				elide: Text.ElideRight
			}
		}
		TintableIcon {
			source: "qrc:///images/material_arrow_right.svg"
			sourceSize.height: Constants.is_desktop ? Style.dimens.icon_size : Style.dimens.small_icon_size
			tintColor: description.color
		}
	}

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
					title.color: Style.color.text_subline_pressed
				}
			},
			State {
				name: "hovered"
				when: root.hovered

				PropertyChanges {
					description.color: Style.color.text_hovered
					pane.border.color: Style.color.pane_border_hovered
					pane.color: Style.color.pane_hovered
					title.color: Style.color.text_subline_hovered
				}
			}
		]
	}
}
