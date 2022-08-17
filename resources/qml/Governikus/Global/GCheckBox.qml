/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0

CheckBox {
	id: control

	property alias textStyle: description.textStyle
	property alias maximumLineCount: description.maximumLineCount

	Accessible.onPressAction: {
		toggle()
		toggled()
	}

	padding: 0

	indicator: Rectangle {
		implicitHeight: ApplicationModel.scaleFactor * 33
		implicitWidth: implicitHeight

		color: enabled ? (control.checked ? Style.color.button : Style.color.transparent) : Style.color.button_disabled
		radius: Math.max(ApplicationModel.scaleFactor * 4, 1)
		border.width: Math.max(ApplicationModel.scaleFactor * 4, 1)
		border.color: enabled ? Style.color.accent : Style.color.button_disabled

		TintableIcon {
			visible: control.checked

			anchors.fill: parent
			anchors.margins: Math.max(ApplicationModel.scaleFactor * 4, 1)

			source: "qrc:///images/checkbox_indicator.svg"
			tintColor: Style.color.button_text
			fillMode: Image.PreserveAspectFit
		}
	}

	contentItem: GText {
		id: description

		activeFocusOnTab: false
		Accessible.ignored: true

		leftPadding: control.indicator.width + control.spacing

		text: control.text
		textStyle: Style.text.normal
		verticalAlignment: Text.AlignVCenter
		maximumLineCount: 1
		elide: Text.ElideRight
	}
}
