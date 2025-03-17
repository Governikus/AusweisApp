/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Animations
import Governikus.Global
import Governikus.Style
import Governikus.Type

DecisionView {
	id: root

	signal startSelfAuth

	agreeButtonColor: SettingsModel.useSelfauthenticationTestUri ? Style.color.error : Style.color.control.background.basic
	agreeButtonIcon: "qrc:///images/identify.svg"

	//: LABEL ALL_PLATFORMS
	agreeButtonText: qsTr("See my personal data")
	descriptionTextsModel: [
		//: INFO ALL_PLATFORMS
		qsTr("This way you can make sure that everything is working and that the data stored on your eID card is up to date before you identify yourself to a provider."),
		//: INFO ALL_PLATFORMS
		qsTr("To do this, you go through an authentication process at Governikus GmbH & Co. KG, the manufacturer of the %1.").arg(Qt.application.name)]
	disagreeButtonHighlighted: false
	//: INFO ALL_PLATFORMS
	headlineText: qsTr("Use eID function")

	//: INFO ALL_PLATFORMS
	subtitleText: qsTr("Have a look which data is stored on your eID card")
	tintAgreeButtonIcon: true
	//: LABEL ALL_PLATFORMS
	title: qsTr("Identify")

	customContentSourceComponent: PrivacyStatement {
	}
	iconSourceComponent: SiteWithLogoAnimation {
		sourceSize.height: Style.dimens.header_icon_size

		PkiSwitch {
			anchors.fill: parent
		}
	}

	onAgreeClicked: startSelfAuth()
}
