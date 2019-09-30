/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0


Item {
	property alias border: frame.border
	property alias source: image.source
	property bool circularFrame: true

	Rectangle {
		id: frame

		color: Style.color.transparent
		border.width: height/20
		border.color: Constants.white
		anchors.fill: parent
		anchors.centerIn: parent
		radius: circularFrame ? width/2 : 0
	}

	Image {
		id: image

		fillMode: Image.PreserveAspectFit
		anchors.margins: frame.border.width * 2
		anchors.centerIn: frame
		anchors.fill: frame
		sourceSize.height: frame.height
		sourceSize.width: frame.width
	}
}
