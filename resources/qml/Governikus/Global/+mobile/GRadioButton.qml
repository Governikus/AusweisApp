/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import "Utils.js" as Utils

import Governikus.Style 1.0

RadioButton {
	id: control

	property var textStyle: Style.text.normal

	Accessible.name: text

	spacing: Constants.groupbox_spacing

	indicator: Rectangle {
		implicitWidth: 26
		implicitHeight:  implicitWidth
		x: control.leftPadding
		y: parent.height / 2 - height / 2
		radius: height / 2
		border.color: Qt.darker(Constants.blue, control.down ? 1.5 : 1)

		Rectangle {
			width: 14
			height: width
			x: (parent.width - width) / 2
			y: x
			radius: width / 2
			color: Qt.darker(Constants.blue, control.down ? 1.5 : 1)
			visible: control.checked
		}
	}

	contentItem: GText {
		Accessible.ignored: true

		text: control.text
		textStyle: control.textStyle
		verticalAlignment: Text.AlignVCenter
		leftPadding: control.indicator.width + control.spacing
	}
}
