/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */

import QtQml

import Governikus.Global
import Governikus.ResultView
import Governikus.Style
import Governikus.Type

ResultView {
	id: root

	//: LABEL ALL_PLATFORMS
	readonly property string leaveText: qsTr("If you have any questions or encounter any errors during the process, please contact the corresponding provider.")
	readonly property string redirectText: {
		if (SettingsModel.autoRedirectAfterAuthentication) {
			//: INFO ALL_PLATFORMS Redirect information when automatic redirect is enabled
			return qsTr("You will be automatically redirected to the provider in a few seconds. If you are not automatically redirected, click on the \"%1\" button.").arg(buttonText);
		}
		//: INFO ALL_PLATFORMS Redirect information when automatic redirect is disabled
		return qsTr("Press the button to complete the authentication and return to the provider.");
	}

	buttonIcon: "qrc:///images/open_website.svg"
	//: LABEL ALL_PLATFORMS
	buttonText: qsTr("Return to provider")
	//: LABEL ALL_PLATFORMS
	header: qsTr("Authentication successful")
	icon: "qrc:///images/status_ok_%1.svg".arg(Style.currentTheme.name)
	subheader: {
		if (Constants.is_desktop) {
			//: INFO DESKTOP Hint to user that the ID card should be removed
			return qsTr("Remove the ID card from the card reader");
		}
		//: INFO ANDROID IOS Hint to user that the ID card should be removed
		return qsTr("Remove the ID card from the NFC interface");
	}
	text: {
		if (AuthModel.showRemoveCardFeedback) {
			return "%1<br/><br/>%2".arg(leaveText).arg(redirectText);
		}
		return leaveText;
	}

	Component.onCompleted: {
		if (!Constants.is_desktop && SettingsModel.autoRedirectAfterAuthentication) {
			timeout.start();
		}
	}
	onVisibleChanged: {
		if (Constants.is_desktop && visible && SettingsModel.autoRedirectAfterAuthentication) {
			timeout.start();
		}
	}

	Timer {
		id: timeout

		interval: 7000

		onTriggered: root.confirm()
	}
}
