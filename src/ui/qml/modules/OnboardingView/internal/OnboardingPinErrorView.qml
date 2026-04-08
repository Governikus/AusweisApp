/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Animations
import Governikus.Global
import Governikus.Type

DecisionView {
	id: root

	signal abort
	signal pukBlocked
	signal tryAgain

	descriptionTextsModel: [ChangePinModel.resultString]
	primaryButton.text: {
		return ChangePinModel.pukInoperative ?
		//: ALL_PLATFORMS
		qsTr("Abort setup") :
		//: ALL_PLATFORMS
		qsTr("Try again");
	}
	//: ALL_PLATFORMS
	secondaryButton.text: ChangePinModel.pukInoperative ? "" : qsTr("Abort setup")

	customContentSourceComponent: PinResetHints {
		statusCode: ChangePinModel.statusCode
	}
	iconSourceComponent: WorkflowAnimationLoader {
		Layout.alignment: Qt.AlignHCenter
		animated: false
		type: ChangePinModel.statusCodeAnimation
	}

	onPrimaryButtonClicked: ChangePinModel.pukInoperative ? root.pukBlocked() : root.tryAgain()
	onSecondaryButtonClicked: root.abort()
}
