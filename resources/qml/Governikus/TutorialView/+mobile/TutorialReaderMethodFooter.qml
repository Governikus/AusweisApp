/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0

TutorialFooter {
	id: footer
	anchors.bottom: parent.bottom
	anchors.horizontalCenter: parent.horizontalCenter
	backRotation: 180
	//: LABEL ANDROID IOS
	backText: qsTr("Back")
	color: Style.color.accent
	state: "showBothOptions"
	width: baseItem.width
}
