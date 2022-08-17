/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0

TutorialFooter {
	id: footer
	width: baseItem.width
	anchors.horizontalCenter: parent.horizontalCenter
	color: Style.color.accent
	anchors.bottom: parent.bottom

	backRotation: 180
	//: LABEL ANDROID IOS
	backText: qsTr("Back")
	state: "showBothOptions"
}
