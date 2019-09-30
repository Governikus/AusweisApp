/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0

import "Utils.js" as Utils


CheckBox {
	id: control

	indicator: Rectangle {
		implicitHeight: 20
		implicitWidth: 20
		anchors.centerIn: parent

		color: control.checked ? Style.color.accent : Style.color.accent_disabled
		border.color: control.checked ? Style.color.accent : Constants.black
		border.width: 2
		radius: 2

		Image {
			source: "qrc:///images/check.svg"
			anchors.fill: parent
			anchors.margins: 3
			fillMode: Image.PreserveAspectFit
			visible: control.checked
		}
	}
}
