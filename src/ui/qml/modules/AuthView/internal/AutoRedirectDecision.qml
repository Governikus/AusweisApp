/**
 * Copyright (c) 2025-2026 Governikus GmbH & Co. KG, Germany
 */

import Governikus.Global
import Governikus.Type

DecisionView {
	id: root

	descriptionTextsModel: [
		//: ALL_PLATFORMS
		qsTr("Currently, the app automatically redirects you back to the service provider after authentication. This may not allow your screen reader to provide all information. To ensure you receive all information, enable manual redirection to the service provider."),
		//: ALL_PLATFORMS
		qsTr("You can change your preference at any time in the settings.")]
	//: ALL_PLATFORMS
	headlineText: qsTr("Optimize your settings for screen reading")
	//: ALL_PLATFORMS
	primaryButton.text: qsTr("Enable manual redirection")
	//: ALL_PLATFORMS
	secondaryButton.text: qsTr("Skip and keep automatic redirection")

	customContentSourceComponent: GCheckBox {
		checked: !SettingsModel.remindUserOfAutoRedirect
		horizontalPadding: 0
		maximumLineCount: 2
		//: ALL_PLATFORMS
		text: qsTr("Do not display this message in future.")

		onCheckedChanged: SettingsModel.remindUserOfAutoRedirect = !checked
	}

	onPrimaryButtonClicked: {
		SettingsModel.autoRedirectAfterAuthentication = false;
		root.leaveView();
	}
	onSecondaryButtonClicked: root.leaveView()
}
