/*
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0

SectionPage {
	id: rootPage
	//: LABEL ANDROID IOS
	title: qsTr("Configure remote service")

	content: RemoteServiceViewRemote {
		width: rootPage.width
	}
	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: pop()
	}
}
