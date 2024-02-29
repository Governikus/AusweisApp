/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View

AbstractButton {
	id: control

	enum Type {
		Forward,
		Check,
		Cancel
	}

	property int buttonType: NavigationButton.Type.Forward
	property double size: Style.dimens.huge_icon_size

	icon.source: buttonType === NavigationButton.Type.Check ? "qrc:///images/material_check.svg" : buttonType === NavigationButton.Type.Cancel ? "qrc:///images/material_clear.svg" : "qrc:///images/desktop/material_arrow_forward.svg"

	background: Item {
	}
	contentItem: Column {
		spacing: Constants.text_spacing

		Rectangle {
			id: circle

			anchors.horizontalCenter: parent.horizontalCenter
			border.color: Style.color.control_border
			border.width: Style.dimens.border_width
			color: Style.color.control
			implicitHeight: control.size
			implicitWidth: control.size
			radius: height / 2

			TintableIcon {
				id: icon

				anchors.centerIn: parent
				source: control.icon.source
				sourceSize.height: Style.dimens.large_icon_size
				tintColor: Style.color.control_content
			}
			FocusFrame {
				marginFactor: 0.8
				radius: parent.radius * 1.2
				scope: control
			}
		}
		GText {
			anchors.horizontalCenter: parent.horizontalCenter
			horizontalAlignment: Text.AlignHCenter
			text: control.text
			textStyle: Style.text.subline
			visible: text !== ""
		}
	}

	Item {
		id: d

		states: [
			State {
				name: "disabled"
				when: !control.enabled

				PropertyChanges {
					circle.border.color: Style.color.control_border_disabled
					circle.color: Style.color.control_disabled
					icon.tintColor: Style.color.control_content_disabled
				}
			},
			State {
				name: "pressed"
				when: control.pressed

				PropertyChanges {
					circle.border.color: Style.color.control_border_pressed
					circle.color: Style.color.control_pressed
					icon.tintColor: Style.color.control_content_pressed
				}
			},
			State {
				name: "hovered"
				when: control.hovered

				PropertyChanges {
					circle.border.color: Style.color.control_border_hovered
					circle.color: Style.color.control_hovered
					icon.tintColor: Style.color.control_content_hovered
				}
			}
		]
	}
}
