/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.ProgressView
import Governikus.Style
import Governikus.TitleBar

ProgressView {
	id: root

	signal cancel

	icon: "qrc:///images/no_internet.svg"
	//: INFO ANDROID IOS No network connection, the user needs to active the network interface or abort the procedure.
	subText: qsTr("Please establish an internet connection.")
	subTextColor: Style.color.textNormal.basic
	//: LABEL ANDROID IOS
	text: qsTr("No network connectivity")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Cancel

		onClicked: cancel()
	}
}
