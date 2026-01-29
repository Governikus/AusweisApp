/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.TitleBar

BaseSkipOnboardingConfirmation {
	id: root

	descriptionTextsModel: [
		//: MOBILE
		qsTr("Only skip this step, if you already used your ID card online and know your 6-digit ID card PIN."), root.commonDescriptionText,
		//: MOBILE %1 will be replaced with the name of the application
		qsTr("If you want to use the eID function on your PC, install %1 there and follow the initial setup.").arg(Qt.application.name)]

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: root.leaveView()
	}
}
