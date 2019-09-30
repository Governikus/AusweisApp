/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0


CheckBox {
	id: control

	implicitHeight: ApplicationModel.scaleFactor * 26
	implicitWidth: implicitHeight

	indicator: Rectangle {
		anchors.fill: parent

		color: control.checked ? (enabled ? Style.color.accent : Style.color.accent_disabled) : Style.color.transparent
		radius: Math.max(ApplicationModel.scaleFactor * 4, 1)
		border.width: Math.max(ApplicationModel.scaleFactor * 4, 1)
		border.color: enabled ? Style.color.accent : Style.color.accent_disabled

		Image {
			visible: control.checked

			anchors.fill: parent
			anchors.margins: Math.max(ApplicationModel.scaleFactor * 4, 1)

			source: "qrc:///images/check.svg"
			fillMode: Image.PreserveAspectFit
		}
	}
}
