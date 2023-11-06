/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.TitleBar
import Governikus.View

FlickableSectionPage {
	id: rootPage

	//: LABEL ANDROID IOS
	title: qsTr("Manage pairings")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: pop()
	}

	RemoteServiceViewRemote {
		Layout.fillWidth: true
	}
}
