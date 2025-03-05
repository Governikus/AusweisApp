/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

import Governikus.Animations
import Governikus.Global
import Governikus.Style

DecisionView {
	id: root

	//: LABEL ALL_PLATFORMS The text between %1 and %2 will be emphasized.
	readonly property string commonDescriptionText: qsTr("You can call up the initial setup again at any time under %1Help%2.").arg("<b>").arg("</b>")

	signal continueOnboarding
	signal skipOnboarding

	//: LABEL ALL_PLATFORMS %1 will be replaced with the app name
	agreeButtonText: qsTr("No, set up %1").arg(Qt.application.name)
	//: LABEL ALL_PLATFORMS
	disagreeButtonText: qsTr("Yes, skip setup")
	//: LABEL ALL_PLATFORMS
	headlineText: qsTr("Set up the eID function")
	//: LABEL ALL_PLATFORMS
	subtitleText: qsTr("Are you sure you want to skip the setup?")

	iconSourceComponent: SiteWithLogoAnimation {
		sourceSize.height: Style.dimens.header_icon_size
	}

	onAgreeClicked: root.continueOnboarding()
	onDisagreeClicked: root.skipOnboarding()
}
