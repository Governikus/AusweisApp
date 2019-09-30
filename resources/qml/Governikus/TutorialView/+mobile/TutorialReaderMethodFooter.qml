/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0


TutorialFooter {
	id: footer
	width: baseItem.width
	anchors.horizontalCenter: parent.horizontalCenter
	color: Style.color.accent
	anchors.bottom: parent.bottom

	backRotation: 180
	//: LABEL ANDROID IOS
	backText: qsTr("Back") + SettingsModel.translationTrigger
	state: "showBothOptions"
}
