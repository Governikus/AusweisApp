/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */

import Governikus.Global
import Governikus.ResultView
import Governikus.Style
import Governikus.Type

ResultView {
	//: INFO ALL_PLATFORMS User message that the redirect to the provider is immanent and the user will leave the AusweisApp
	readonly property string leaveText: qsTr("You will now leave the %1. For any further questions regarding the current process or occurring errors contact the corresponding provider.").arg(Qt.application.name)
	readonly property string removeCardText: {
		if (Constants.is_desktop) {
			//: INFO DESKTOP Hint to user that the ID card should be removed
			return qsTr("Remove the ID card from the card reader.");
		}
		//: INFO ANDROID IOS Hint to user that the ID card should be removed
		return qsTr("Remove the ID card from the NFC interface.");
	}

	buttonIcon: "qrc:///images/open_website.svg"
	//: LABEL ALL_PLATFORMS
	buttonText: qsTr("Return to provider")
	//: LABEL ALL_PLATFORMS
	header: qsTr("Authentication successful")
	icon: "qrc:///images/status_ok_%1.svg".arg(Style.currentTheme.name)
	//: LABEL ALL_PLATFORMS
	subheader: qsTr("You will be redirected to the provider")
	text: {
		if (AuthModel.showRemoveCardFeedback) {
			return "%1<br/><br/>%2".arg(leaveText).arg(removeCardText);
		}
		return leaveText;
	}
}
