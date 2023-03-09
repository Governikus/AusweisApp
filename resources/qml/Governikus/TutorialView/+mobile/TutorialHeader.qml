/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	id: baseItem

	property bool categoryAbove: true
	property alias headerImageSource: headerImage.source
	property real initY
	property alias miniIconCoordinates: backgroundIcons.model
	property string miniIconSource
	property bool overlapping: true
	property real overlappingHeight: overlapping ? height * (4.0 / 3.0) : height
	property alias titleText: title.text

	signal clicked

	Accessible.name: title.text
	width: parent.width

	Accessible.onPressAction: clicked()

	Image {
		id: headerImage
		fillMode: Image.Stretch
		height: baseItem.overlappingHeight
		width: parent.width

		MouseArea {
			anchors.fill: parent

			onClicked: baseItem.clicked()
		}
		Repeater {
			id: backgroundIcons
			Image {
				height: 0.125 * baseItem.overlappingHeight
				source: baseItem.miniIconSource
				width: height
				x: modelData.x * baseItem.width
				y: modelData.y * baseItem.overlappingHeight
			}
		}
		GText {
			id: title
			Accessible.ignored: true
			anchors.horizontalCenter: parent.horizontalCenter
			style: Text.Outline
			styleColor: Constants.white
			textStyle: Style.text.tutorial_title_highlight
			y: ((categoryAbove ? 0.575 : 0.5) * parent.height) - (0.5 * height)
		}
	}
}
