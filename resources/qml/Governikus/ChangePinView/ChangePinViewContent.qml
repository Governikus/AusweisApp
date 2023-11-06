/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.Type.NumberModel
import Governikus.Type.ApplicationModel

GFlickableColumnLayout {
	id: root

	readonly property double buttonHeight: Math.max(sixDigitButton.implicitHeight, fiveDigitButton.implicitHeight, dontKnowButton.implicitHeight)
	readonly property double buttonWidth: Math.max(sixDigitButton.implicitWidth, fiveDigitButton.implicitWidth, dontKnowButton.implicitWidth)
	property alias moreInformationText: moreInformationLink.text

	signal changePin
	signal changeTransportPin
	signal moreInformationRequested
	signal noPinAvailable

	maximumContentWidth: Style.dimens.max_text_width
	spacing: Constants.component_spacing

	GText {
		id: pinDescWhatType

		Layout.alignment: Qt.AlignHCenter
		Layout.topMargin: Constants.component_spacing

		//: LABEL ALL_PLATFORMS
		text: qsTr("What kind of PIN do you have?")
		textStyle: Style.text.headline
		wrapMode: Text.WordWrap
	}
	MoreInformationLink {
		id: moreInformationLink

		Layout.alignment: Qt.AlignHCenter

		onClicked: root.moreInformationRequested()
	}
	GInformativeButton {
		id: sixDigitButton

		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		Layout.maximumWidth: root.buttonWidth
		Layout.preferredHeight: root.buttonHeight
		//: LABEL ALL_PLATFORMS
		description: qsTr("Set by yourself")
		icon.source: "qrc:///images/icon_six_digit_pin.svg"

		//: LABEL ALL_PLATFORMS
		text: qsTr("Six-digit PIN")
		tintIcon: true

		onClicked: root.changePin()
	}
	GInformativeButton {
		id: fiveDigitButton

		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		Layout.maximumWidth: root.buttonWidth
		Layout.preferredHeight: root.buttonHeight
		//: LABEL ALL_PLATFORMS
		description: qsTr("Received by mail in PIN letter")
		icon.source: "qrc:///images/icon_five_digit_pin.svg"

		//: LABEL ALL_PLATFORMS
		text: qsTr("Five-digit Transport PIN")
		tintIcon: true

		onClicked: root.changeTransportPin()
	}
	GInformativeButton {
		id: dontKnowButton

		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		Layout.maximumWidth: root.buttonWidth
		Layout.preferredHeight: root.buttonHeight
		//: LABEL ALL_PLATFORMS
		description: qsTr("Lost, forgotten, or never received it")
		icon.source: "qrc:///images/material_block.svg"

		//: LABEL ALL_PLATFORMS
		text: qsTr("No PIN")
		tintIcon: true

		onClicked: root.noPinAvailable()
	}
}
