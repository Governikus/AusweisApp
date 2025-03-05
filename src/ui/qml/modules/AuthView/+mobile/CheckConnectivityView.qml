/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

import Governikus.ProgressView
import Governikus.TitleBar

ProgressView {
	id: root

	signal cancel

	//: LABEL ANDROID IOS
	headline: qsTr("No network connectivity")
	icon: "qrc:///images/no_internet.svg"
	//: INFO ANDROID IOS No network connection, the user needs to active the network interface or abort the procedure.
	text: qsTr("Please establish an internet connection.")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Cancel

		onClicked: root.cancel()
	}
}
