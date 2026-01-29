/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQml

import Governikus.Animations
import Governikus.ResultView
import Governikus.Style
import Governikus.Type

ResultView {
	id: root

	//: ALL_PLATFORMS
	readonly property string leaveText: qsTr("If you have any questions or encounter any errors during the process, please contact the corresponding provider.")
	readonly property string redirectText: {
		if (Style.is_layout_desktop) {
			if (SettingsModel.autoRedirectAfterAuthentication) {
				//: DESKTOP Redirect information when automatic redirect is enabled
				return qsTr("You will be automatically redirected to the provider in a few seconds. If you are not automatically redirected, click the \"%1\" button.").arg(buttonText);
			}

			//: DESKTOP Redirect information when automatic redirect is disabled
			return qsTr("Click the button to complete the authentication and return to the provider.");
		}

		if (SettingsModel.autoRedirectAfterAuthentication) {
			//: MOBILE Redirect information when automatic redirect is enabled
			return qsTr("You will be automatically redirected to the provider in a few seconds. If you are not automatically redirected, tap the \"%1\" button.").arg(buttonText);
		}

		//: MOBILE Redirect information when automatic redirect is disabled
		return qsTr("Tap the button to complete the authentication and return to the provider.");
	}

	animationSymbol: Symbol.Type.CHECK
	animationType: AnimationLoader.Type.STATUS
	buttonIcon: "qrc:///images/open_website.svg"
	buttonText: AuthModel.resultViewButtonText
	//: ALL_PLATFORMS
	header: qsTr("Authentication successful")
	linkToOpen: AuthModel.refreshUrl
	subheader: {
		if (Style.is_layout_desktop) {
			//: DESKTOP Hint to user that the ID card should be removed
			return qsTr("Remove the ID card from the card reader");
		}
		//: MOBILE Hint to user that the ID card should be removed
		return qsTr("You may now remove your ID card from the device.");
	}
	text: {
		if (AuthModel.showRemoveCardFeedback) {
			return "%1<br/><br/>%2".arg(leaveText).arg(redirectText);
		}
		return leaveText;
	}

	Component.onCompleted: {
		if (SettingsModel.autoRedirectAfterAuthentication) {
			timeout.start();
		}
	}

	Timer {
		id: timeout

		interval: ApplicationModel.feedbackTimeout

		onTriggered: root.confirm()
	}
	Connections {
		function onFireAppAboutToQuit() {
			timeout.stop();
		}

		target: ApplicationModel
	}
}
