/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Animations
import Governikus.Global
import Governikus.Style

DecisionView {
	id: root

	//: ALL_PLATFORMS The text between %1 and %2 will be emphasized.
	readonly property string commonDescriptionText: qsTr("You can call up the initial setup again at any time under %1Help%2.").arg("<b>").arg("</b>")

	signal continueOnboarding
	signal skipOnboarding

	//: ALL_PLATFORMS
	headlineText: qsTr("Set up the eID function")
	//: ALL_PLATFORMS
	primaryButton.text: qsTr("Yes, skip setup")
	//: ALL_PLATFORMS %1 will be replaced with the app name
	secondaryButton.text: qsTr("No, set up %1").arg(Qt.application.name)
	//: ALL_PLATFORMS
	subtitleText: qsTr("Are you sure you want to skip the setup?")

	iconSourceComponent: SiteWithLogoAnimation {
		sourceSize.height: Style.dimens.header_icon_size
	}

	onPrimaryButtonClicked: root.skipOnboarding()
	onSecondaryButtonClicked: root.continueOnboarding()
}
