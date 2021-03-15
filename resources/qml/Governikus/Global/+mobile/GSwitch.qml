/*
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Style 1.0

Switch {
	id: control

	property color color: Style.color.switch_checked

	Accessible.onPressAction: if (Qt.platform.os === "ios") toggle()

	implicitWidth: indicator.implicitWidth
	implicitHeight: indicator.implicitHeight
	padding: 0

	// Empty item since we don't want the text
	contentItem: Item {}

	indicator: Item {
		implicitWidth: 48
		implicitHeight: 24

		Rectangle {
			width: parent.width
			height: parent.height / 2
			y: parent.height / 2 - height / 2
			radius: height / 2

			color: control.enabled ? (control.checked ? Qt.lighter(control.color, 1.3) : Qt.darker(Style.color.accent_disabled, 1.4)) : Qt.darker(Style.color.accent_disabled, 1.1)
		}

		Rectangle {
			width: parent.height
			height: parent.height
			x: control.checked ? parent.width - width : 0
			radius: height / 2
			border.width: control.enabled && !control.checked ? Style.dimens.separator_size : 0
			border.color: Style.color.border

			color: control.enabled ? (control.checked ? control.color : Style.color.switch_unchecked) : Style.color.accent_disabled

			Behavior on x {
				NumberAnimation {easing.type: Easing.InOutQuad; duration: 200}
			}
		}
	}
}
