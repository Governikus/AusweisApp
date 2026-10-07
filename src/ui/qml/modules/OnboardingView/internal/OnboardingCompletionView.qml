/**
 * Copyright (c) 2024-2026 Governikus Service GmbH, Germany
 */

import QtQuick

import Governikus.Animations
import Governikus.Global
import Governikus.Style
import Governikus.Type

DecisionView {
	descriptionTextsModel: [
		//: ALL_PLATFORMS %1 will be replaced with the application name
		qsTr("If you are asked to authenticate yourself online on a provider's website, the provider will automatically redirect you to %1. The app guides you through the authentication process and then takes you back to the provider's website.").arg(Qt.application.name)]
	//: ALL_PLATFORMS
	headlineText: qsTr("Use the eID function")

	//: ALL_PLATFORMS
	primaryButton.text: qsTr("Complete setup")
	//: ALL_PLATFORMS %1 will be replaced with the application name
	subtitleText: qsTr("You can use %1 to authenticate yourself to providers online").arg(Qt.application.name)

	customContentSourceComponent: Hint {
		//: ALL_PLATFORMS
		buttonText: qsTr("Open website")
		buttonToolTip: linkToOpen
		linkToOpen: "https://www.ausweisapp.bund.de/%1/aa2/providerlist".arg(SettingsModel.language)
		//: ALL_PLATFORMS
		text: qsTr("You can find many examples of use in the provider list.")
	}
	iconSourceComponent: SiteWithLogoAnimation {
		sourceSize.height: Style.dimens.header_icon_size
	}
}
