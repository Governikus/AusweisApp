/**
 * Copyright (c) 2024-2026 Governikus Service GmbH, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Animations
import Governikus.Global
import Governikus.Style
import Governikus.Type

DecisionView {
	signal startSelfAuth

	descriptionTextsModel: [
		//: ALL_PLATFORMS
		qsTr("This way you can make sure that everything is working and that the data stored on your eID card is up to date before you identify yourself to a provider."),
		//: ALL_PLATFORMS
		qsTr("To do this, you go through an authentication process at Governikus Service GmbH, the manufacturer of the %1.").arg(Qt.application.name)]
	//: ALL_PLATFORMS
	headlineText: qsTr("Use eID function")
	primaryButton.buttonColor: SettingsModel.useSelfauthenticationTestUri ? Style.color.error : Style.color.control.background.basic_unchecked
	primaryButton.icon.source: "qrc:///images/identify.svg"
	//: ALL_PLATFORMS
	primaryButton.text: qsTr("See my personal data")
	primaryButton.tintIcon: true
	//: ALL_PLATFORMS
	subtitleText: qsTr("Have a look which data is stored on your eID card")
	//: ALL_PLATFORMS
	title: qsTr("Identify")

	customContentSourceComponent: Hint {
		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		//: ALL_PLATFORMS Buttontext for the link to the data privacy statement
		buttonText: qsTr("Data privacy statement")
		linkToOpen: "https://www.ausweisapp.bund.de/%1/aa2/privacy-data".arg(SettingsModel.language)
		//: ALL_PLATFORMS
		text: qsTr("Your personal data is neither saved nor processed in any way. Please see our data privacy statement for details on how your personal data is processed.")
	}
	iconSourceComponent: SiteWithLogoAnimation {
		sourceSize.height: Style.dimens.header_icon_size

		PkiSwitch {
			anchors.fill: parent
		}
	}

	onPrimaryButtonClicked: startSelfAuth()
}
