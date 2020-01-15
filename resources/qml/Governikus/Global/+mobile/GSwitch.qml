/*
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.10

import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0

Switch {
	id: control

	property color color: Style.color.accent

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

			color: control.checked ? Qt.lighter(control.color, 1.4) : Qt.darker(Style.color.accent_disabled, 1.1)
		}

		Rectangle {
			width: parent.height
			height: parent.height
			x: control.checked ? parent.width - width : 0
			radius: height / 2

			color: control.checked ? control.color : Style.color.accent_disabled

			Behavior on x {
				NumberAnimation {easing.type: Easing.InOutQuad; duration: 200}
			}
		}
	}
}
