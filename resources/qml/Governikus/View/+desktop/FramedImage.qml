/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0


Item {
	id: baseItem

	property alias tintColor: image.tintColor
	property alias source: image.source
	property bool circularFrame: true

	Rectangle {
		id: frame

		color: Style.color.transparent
		border.width: height/20
		border.color: baseItem.tintColor
		anchors.fill: parent
		anchors.centerIn: parent
		radius: circularFrame ? width/2 : 0
	}

	TintableIcon {
		id: image

		anchors.margins: frame.border.width * 2
		anchors.fill: frame
		sourceSize.height: frame.height
		sourceSize.width: frame.width
	}
}
