/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.PasswordInfoView
import Governikus.Style
import Governikus.Type.NumberModel
import Governikus.Type.ApplicationModel

GFlickableColumnLayout {
	id: root

	readonly property double buttonWidth: Math.max(sixDigitButton.implicitWidth, fiveDigitButton.implicitWidth, dontKnowButton.implicitWidth)
	readonly property alias pinInfo: infoData

	signal changePin
	signal changePinInfoRequested
	signal changeTransportPin
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
		Layout.alignment: Qt.AlignHCenter
		text: infoData.linkText

		onClicked: root.changePinInfoRequested()

		PasswordInfoData {
			id: infoData

			contentType: PasswordInfoData.Type.CHANGE_PIN
		}
	}
	GInformativeButton {
		id: sixDigitButton

		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		Layout.maximumWidth: root.buttonWidth
		//: LABEL ALL_PLATFORMS
		description: qsTr("Set by yourself")
		icon.source: "qrc:///images/icon_six_digit_pin.svg"

		//: LABEL ALL_PLATFORMS
		text: qsTr("6-digit PIN")

		onClicked: root.changePin()
	}
	GInformativeButton {
		id: fiveDigitButton

		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		Layout.maximumWidth: root.buttonWidth
		//: LABEL ALL_PLATFORMS
		description: qsTr("Received by mail in PIN letter")
		icon.source: "qrc:///images/icon_five_digit_pin.svg"

		//: LABEL ALL_PLATFORMS
		text: qsTr("5-digit Transport PIN")

		onClicked: root.changeTransportPin()
	}
	GInformativeButton {
		id: dontKnowButton

		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		Layout.maximumWidth: root.buttonWidth
		//: LABEL ALL_PLATFORMS
		description: qsTr("Lost, forgotten, or never received it")
		icon.source: "qrc:///images/material_block.svg"

		//: LABEL ALL_PLATFORMS
		text: qsTr("No PIN")

		onClicked: root.noPinAvailable()
	}
}
