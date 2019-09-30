/*
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtGraphicalEffects 1.0

import Governikus.Global 1.0
import Governikus.Style 1.0

MouseArea {
	id: baseItem

	property alias text: textItem.text
	property alias source: imageItem.source

	Accessible.role: Accessible.Button
	Accessible.name: text
	Accessible.onPressAction: if (Qt.platform.os === "ios" && enabled) clicked(null)

	GText {
		id: textItem

		anchors.centerIn: parent

		Accessible.ignored: true

		textStyle: Style.text.title
	}

	Image {
		id: imageItem
		anchors.centerIn: parent
		height: 36
		width: height
		fillMode: Image.PreserveAspectFit
	}

	Rectangle {
		id: darkLayer
		anchors.centerIn: parent
		height: width
		radius: width /2
		color: Constants.black
		opacity: 0.1
		SequentialAnimation on width {
			running: baseItem.pressed
			alwaysRunToEnd: true
			NumberAnimation { from: 0; to: Math.SQRT2 * baseItem.width }
			PauseAnimation { duration: 100 }
			PropertyAction { value: 0 }
		}
	}
}
