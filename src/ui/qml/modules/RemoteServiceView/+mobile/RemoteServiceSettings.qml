/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.TitleBar
import Governikus.View

FlickableSectionPage {
	id: rootPage

	signal pairingFailed
	signal pairingSuccessful

	//: LABEL ANDROID IOS
	title: qsTr("Manage pairings")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: pop()
	}

	RemoteServiceViewRemote {
		Layout.fillWidth: true
		title: rootPage.title

		onPairingFailed: rootPage.pairingFailed()
		onPairingSuccessful: rootPage.pairingSuccessful()
		onReceivedFocus: pItem => rootPage.positionViewAtItem(pItem)
	}
}
