/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.ProgressView
import Governikus.TitleBar

ProgressView {
	id: root

	required property bool networkInterfaceActive

	signal cancel

	//: INFO DESKTOP The user aborted the authentication process, according to TR we need to inform the service provider
	headline: qsTr("Aborting process and informing the service provider")
	progressBarVisible: false
	text: {
		if (networkInterfaceActive) {
			//: INFO DESKTOP Information message about cancellation process with present network connectivity
			return qsTr("Please wait a moment.");
		}
		//: INFO DESKTOP Information message about cancellation process without working network connectivity
		return qsTr("Network problems detected, trying to reach server within 30 seconds.");
	}

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Cancel

		onClicked: root.cancel()
	}
}
