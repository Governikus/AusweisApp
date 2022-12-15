/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	id: baseItem

	property bool circularFrame: true
	property alias source: image.source
	property alias tintColor: image.tintColor

	Rectangle {
		id: frame
		anchors.centerIn: parent
		anchors.fill: parent
		border.color: baseItem.tintColor
		border.width: height / 20
		color: Style.color.transparent
		radius: circularFrame ? width / 2 : 0
	}
	TintableIcon {
		id: image
		anchors.fill: frame
		anchors.margins: frame.border.width * 2
		sourceSize.height: frame.height
		sourceSize.width: frame.width
	}
}
