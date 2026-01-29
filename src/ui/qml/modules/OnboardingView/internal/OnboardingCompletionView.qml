/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Animations
import Governikus.Global
import Governikus.Style
import Governikus.Type

DecisionView {
	//: ALL_PLATFORMS
	readonly property string providerLink: "<a href=\"https://www.ausweisapp.bund.de/%1/aa2/providerlist\">%2</a>".arg(SettingsModel.language).arg(qsTr("provider list"))

	descriptionTextsModel: [
		//: ALL_PLATFORMS %1 will be replaced with the application name, %2 will be replaced with "provider list" as hyperlink.
		qsTr("If you are asked to authenticate yourself online on a provider's website, the provider will automatically redirect you to %1. The app guides you through the authentication process and then takes you back to the provider's website. You can find many examples of use in the %2.").arg(Qt.application.name).arg(providerLink)]
	//: ALL_PLATFORMS
	headlineText: qsTr("Use the eID function")

	//: ALL_PLATFORMS
	primaryButton.text: qsTr("Complete setup")
	//: ALL_PLATFORMS %1 will be replaced with the application name
	subtitleText: qsTr("You can use %1 to authenticate yourself to providers online").arg(Qt.application.name)

	iconSourceComponent: SiteWithLogoAnimation {
		sourceSize.height: Style.dimens.header_icon_size
	}
}
