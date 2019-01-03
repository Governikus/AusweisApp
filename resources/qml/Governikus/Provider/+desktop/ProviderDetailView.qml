import QtQuick 2.10

import Governikus.View 1.0
import Governikus.TitleBar 1.0
import Governikus.Provider 1.0

SectionPage {
	titleBarAction: TitleBarAction {
		text: qsTr("Provider details")
		showSettings: false
		helpTopic: "providerPage"
	}

	Rectangle {
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		height: parent.height / 2
		width: parent.width / 2

		color: "mediumaquamarine"
	}
}
