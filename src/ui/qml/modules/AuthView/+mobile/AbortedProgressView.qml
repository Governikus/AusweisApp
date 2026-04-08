/**
 * Copyright (c) 2021-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import Governikus.ProgressView
import Governikus.TitleBar

ProgressView {
	id: root

	required property bool networkInterfaceActive

	signal cancel

	//: DESKTOP The user aborted the authentication process, according to TR we need to inform the service provider
	headline: qsTr("Aborting process and informing the service provider")
	text: {
		if (networkInterfaceActive) {
			//: DESKTOP Information message about cancellation process with present network connectivity
			return qsTr("Please wait a moment.");
		}
		//: DESKTOP Information message about cancellation process without working network connectivity
		return qsTr("Network problems detected, trying to reach server within 30 seconds.");
	}

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Cancel

		onClicked: root.cancel()
	}
}
