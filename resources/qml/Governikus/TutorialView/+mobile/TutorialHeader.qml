/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtGraphicalEffects 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	id: baseItem
	width: parent.width

	property alias headerImageSource: headerImage.source
	property alias titleText: title.text
	property string miniIconSource
	property alias miniIconCoordinates: backgroundIcons.model
	property bool overlapping: true
	property real overlappingHeight: overlapping ? height * (4.0/3.0) : height
	property bool categoryAbove: true
	property real initY
	signal clicked()

	Accessible.name: title.text
	Accessible.onPressAction: if (Qt.platform.os === "ios") clicked()

	Image{
		id: headerImage
		width: parent.width
		height: baseItem.overlappingHeight
		fillMode: Image.Stretch

		MouseArea {
			anchors.fill: parent
			onClicked: baseItem.clicked()
		}

		Repeater {
			id: backgroundIcons
			Image {
				source: baseItem.miniIconSource
				width: height
				height: 0.125 * baseItem.overlappingHeight
				x: modelData.x * baseItem.width
				y: modelData.y * baseItem.overlappingHeight
			}
		}

		GText {
			id: title

			anchors.horizontalCenter: parent.horizontalCenter
			y: ((categoryAbove ? 0.575 : 0.5) * parent.height) - (0.5 * height)

			Accessible.ignored: true

			font.bold: true
			textStyle: Style.text.tutorial_title
			layer.enabled: GraphicsInfo.api !== GraphicsInfo.Software
			layer.effect: DropShadow {
				verticalOffset: 3
				horizontalOffset: 3
				color: Constants.white
				radius: 1
				samples: 3
			}
		}
	}
}
