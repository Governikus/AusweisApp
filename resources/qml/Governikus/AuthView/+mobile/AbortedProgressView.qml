/*
 * \copyright Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.ProgressView 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.ConnectivityManager 1.0

ProgressView {
	id: root
	signal cancel

	progressBarVisible: false
	subText: {
		if (ConnectivityManager.networkInterfaceActive) {
			//: INFO DESKTOP Information message about cancellation process with present network connectivity
			return qsTr("Please wait a moment.");
		}
		//: INFO DESKTOP Information message about cancellation process without working network connectivity
		return qsTr("Network problems detected, trying to reach server within 30 seconds.");
	}
	subTextColor: !ConnectivityManager.networkInterfaceActive ? Style.color.warning_text : Style.color.secondary_text
	//: INFO DESKTOP The user aborted the authentication process, according to TR we need to inform the service provider
	text: qsTr("Aborting process and informing the service provider")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Cancel

		onClicked: root.cancel()
	}
}
