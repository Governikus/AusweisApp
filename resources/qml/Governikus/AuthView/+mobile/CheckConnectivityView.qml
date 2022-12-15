/*
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.ProgressView 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0

ProgressView {
	id: root
	signal cancel

	//: INFO ANDROID IOS No network connection, the user needs to active the network interface or abort the procedure.
	subText: qsTr("Please establish an internet connection.")
	subTextColor: Style.color.warning_text
	//: LABEL ANDROID IOS
	text: qsTr("No network connectivity")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Cancel

		onClicked: cancel()
	}
}
