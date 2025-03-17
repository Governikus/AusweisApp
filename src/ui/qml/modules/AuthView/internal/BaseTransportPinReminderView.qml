/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.ChangePinView
import Governikus.Global
import Governikus.MultiInfoView
import Governikus.Style
import Governikus.View

FlickableSectionPage {
	id: root

	signal cancel
	signal pinKnown
	signal showInfoView(var pContentType)
	signal showTransportPinInfo
	signal transportPinKnown

	spacing: Style.dimens.pane_spacing

	MultiInfoData {
		id: infoData

		contentType: MultiInfoData.CHANGE_PIN
	}
	GText {
		Layout.alignment: Qt.AlignHCenter

		//: LABEL ALL_PLATFORMS
		text: qsTr("Set up card PIN")
		textStyle: Style.text.headline
	}
	GText {
		Layout.alignment: Qt.AlignHCenter

		//: LABEL ALL_PLATFORMS %1 and %2 are replaced with bold highlighting
		text: qsTr("You need an %1ID Card PIN%2 in order to use the eID function. If you just have %1Transport PIN%2, you have to %1replace%2 it with an %1ID Card PIN once%2.").arg("<b>").arg("</b>")
	}
	MoreInformationLink {
		Layout.alignment: Qt.AlignHCenter
		text: infoData.linkText

		onClicked: root.showInfoView(MultiInfoData.CHANGE_PIN)
	}
	GText {
		//: LABEL ALL_PLATFORMS
		text: qsTr("What kind of PIN do you have?")
		textStyle: Style.text.subline
	}
	PinSelectionButtons {
		Layout.alignment: Qt.AlignHCenter

		onFiveDigitPin: root.showTransportPinInfo()
		onNoPinAvailable: root.showInfoView(MultiInfoData.NO_PIN)
		onSixDigitPin: root.pinKnown()
	}
}
