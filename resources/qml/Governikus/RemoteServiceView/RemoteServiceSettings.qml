import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.1

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0

SectionPage {
	id: rootPage
	leftTitleBarAction: TitleBarAction { state: "back"; onClicked: firePop() }
	headerTitleBarAction: TitleBarAction { text: qsTr("Configure remote service") + settingsModel.translationTrigger; font.bold: true }

	content: RemoteServiceViewRemote {
		width: rootPage.width
		visible: true
		parentSectionPage: rootPage
	}
}
