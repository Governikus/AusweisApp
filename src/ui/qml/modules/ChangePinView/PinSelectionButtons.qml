/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style

ColumnLayout {
	id: root

	signal fiveDigitPin
	signal noPinAvailable
	signal sixDigitPin

	Layout.maximumWidth: Math.max(sixDigitButton.implicitWidth, fiveDigitButton.implicitWidth, dontKnowButton.implicitWidth)
	spacing: Style.dimens.pane_spacing

	GInformativeButton {
		id: sixDigitButton

		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		//: LABEL ALL_PLATFORMS
		description: qsTr("Set by yourself or received by the PIN Reset Service")
		icon.source: "qrc:///images/icon_six_digit_pin.svg"
		isPane: true

		//: LABEL ALL_PLATFORMS
		text: qsTr("6-digit PIN")

		onClicked: root.sixDigitPin()
	}
	GInformativeButton {
		id: fiveDigitButton

		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		//: LABEL ALL_PLATFORMS
		description: qsTr("Received by letter while ordering your ID card")
		icon.source: "qrc:///images/icon_five_digit_pin.svg"
		isPane: true

		//: LABEL ALL_PLATFORMS
		text: qsTr("5-digit Transport PIN")

		onClicked: root.fiveDigitPin()
	}
	GInformativeButton {
		id: dontKnowButton

		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		//: LABEL ALL_PLATFORMS
		description: qsTr("Lost, forgotten, or never received it")
		icon.source: "qrc:///images/material_block.svg"
		isPane: true

		//: LABEL ALL_PLATFORMS
		text: qsTr("No PIN")

		onClicked: root.noPinAvailable()
	}
}
