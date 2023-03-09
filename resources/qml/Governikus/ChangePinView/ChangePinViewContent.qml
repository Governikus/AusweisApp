/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ChangePinModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.ApplicationModel 1.0

GFlickableColumnLayout {
	id: root

	property alias moreInformationText: moreInformationLink.text

	signal moreInformationRequested
	signal noPinAvailable

	maximumContentWidth: Style.dimens.max_text_width
	spacing: 0

	GText {
		id: pinDescWhatType
		Layout.alignment: Qt.AlignCenter
		Layout.maximumWidth: root.effectiveContentWidth
		Layout.topMargin: Constants.component_spacing
		horizontalAlignment: Text.AlignLeft

		//: LABEL ALL_PLATFORMS
		text: qsTr("What kind of PIN do you have?")
		textStyle: Style.text.title_highlight
		wrapMode: Text.WordWrap
	}
	MoreInformationLink {
		id: moreInformationLink
		Layout.alignment: Qt.AlignCenter
		Layout.maximumWidth: root.effectiveContentWidth
		Layout.topMargin: Constants.component_spacing

		onClicked: root.moreInformationRequested()
	}
	ColumnLayout {
		id: buttonLayout

		readonly property double buttonHeight: Math.max(sixDigitButton.implicitHeight, fiveDigitButton.implicitHeight, dontKnowButton.implicitHeight)

		Layout.alignment: Qt.AlignCenter
		Layout.fillWidth: false
		Layout.maximumWidth: root.effectiveContentWidth
		Layout.topMargin: Constants.component_spacing
		spacing: Constants.component_spacing

		GInformativeButton {
			id: sixDigitButton
			Layout.fillWidth: true
			Layout.preferredHeight: buttonLayout.buttonHeight
			//: LABEL ALL_PLATFORMS
			description: qsTr("Set by yourself")
			icon.source: "qrc:///images/icon_six_digit_pin_white.svg"

			//: LABEL ALL_PLATFORMS
			text: qsTr("6-digit PIN")

			onClicked: ChangePinModel.startWorkflow(false)
		}
		GInformativeButton {
			id: fiveDigitButton
			Layout.fillWidth: true
			Layout.preferredHeight: buttonLayout.buttonHeight
			//: LABEL ALL_PLATFORMS
			description: qsTr("Received by mail in PIN letter")
			icon.source: "qrc:///images/icon_five_digit_pin.svg"

			//: LABEL ALL_PLATFORMS
			text: qsTr("5-digit Transport PIN")

			onClicked: ChangePinModel.startWorkflow(true)
		}
		GInformativeButton {
			id: dontKnowButton
			Layout.fillWidth: true
			Layout.preferredHeight: buttonLayout.buttonHeight
			//: LABEL ALL_PLATFORMS
			description: qsTr("Lost, forgotten, or never received it")
			icon.source: "qrc:///images/material_block.svg"
			scaleIcon: 0.5

			//: LABEL ALL_PLATFORMS
			text: qsTr("No PIN")
			tintIcon: true

			onClicked: root.noPinAvailable()
		}
	}
	GSpacer {
		Layout.fillHeight: true
	}
}
