/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0

CheckBox {
	id: control

	Accessible.role: Accessible.CheckBox
	Accessible.checkable: true
	Accessible.checked: checked
	Accessible.name: text
	Accessible.onPressAction: toggle()

	indicator: Rectangle {
		implicitHeight: 26
		implicitWidth: 26
		anchors.centerIn: parent

		color: Constants.white
		border.color: Constants.black
		border.width: 1

		Image {
			source: "qrc:///images/iOS/CheckedCheckbox.png"
			anchors.fill: parent
			anchors.margins: 2
			fillMode: Image.PreserveAspectFit
			visible: control.checked
		}
	}
}
