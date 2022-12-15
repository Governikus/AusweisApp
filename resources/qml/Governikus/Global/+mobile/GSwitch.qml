/*
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Style 1.0

Switch {
	id: control

	property color color: Style.color.switch_checked

	implicitHeight: indicator.implicitHeight
	implicitWidth: indicator.implicitWidth
	padding: 0

	// Empty item since we don't want the text
	contentItem: Item {
	}
	indicator: Item {
		implicitHeight: 24
		implicitWidth: 48

		Rectangle {
			color: control.enabled ? (control.checked ? Qt.lighter(control.color, 1.3) : Qt.darker(Style.color.button_disabled, 1.4)) : Qt.darker(Style.color.button_disabled, 1.1)
			height: parent.height / 2
			radius: height / 2
			width: parent.width
			y: parent.height / 2 - height / 2
		}
		Rectangle {
			border.color: Style.color.border
			border.width: control.enabled && !control.checked ? Style.dimens.separator_size : 0
			color: control.enabled ? (control.checked ? control.color : Style.color.switch_unchecked) : Style.color.button_disabled
			height: parent.height
			radius: height / 2
			width: parent.height
			x: control.checked ? parent.width - width : 0

			Behavior on x  {
				NumberAnimation {
					duration: 200
					easing.type: Easing.InOutQuad
				}
			}
		}
	}

	Accessible.onPressAction: toggle()
}
