import QtQuick 2.10

import Governikus.Global 1.0

TutorialFooter {
	id: footer
	width: baseItem.width
	anchors.horizontalCenter: parent.horizontalCenter
	color: Constants.tutorial_blue
	anchors.bottom: parent.bottom

	backText: qsTr("Back") + settingsModel.translationTrigger
}
