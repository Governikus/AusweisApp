/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type.SettingsModel

Button {
	id: control

	property real fontScale: 1
	property bool visualPrivacy: SettingsModel.visualPrivacy

	height: (1.5 * contentItem.font.pixelSize) / fontScale
	padding: 0
	width: height / d.aspectRatio

	background: Rectangle {
		id: circle

		border.color: Style.text.headline.textColor
		border.width: !visualPrivacy && control.focus ? Math.max(1, plugin.scaleFactor * 2) : 0
		color: control.enabled ? (control.down && !visualPrivacy ? Style.color.control : Style.color.pane_sublevel) : Style.color.control_disabled
		radius: Style.dimens.control_radius

		MouseArea {
			anchors.fill: parent
			cursorShape: enabled ? Qt.PointingHandCursor : undefined
		}
	}
	contentItem: GText {
		color: control.down && !visualPrivacy ? Style.color.control_pressed : textStyle.textColor
		font.pixelSize: fontScale * Style.dimens.text_headline
		horizontalAlignment: Text.AlignHCenter
		text: control.text
		textStyle: Style.text.headline
	}

	FocusFrame {
	}
	QtObject {
		id: d

		readonly property real aspectRatio: 0.7
	}
}
