import QtQuick 2.10
import QtGraphicalEffects 1.0

import Governikus.Global 1.0

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

		Text {
			id: title
			anchors.horizontalCenter: parent.horizontalCenter
			y: ((categoryAbove ? 0.575 : 0.5) * parent.height) - (0.5 * height)
			font.bold: true
			font.pixelSize: Constants.tutorial_header_font_size
			layer.enabled: true
			layer.effect: DropShadow {
				verticalOffset: Utils.dp(3)
				horizontalOffset: Utils.dp(3)
				color: Constants.white
				radius: Utils.dp(1)
				samples: 3
			}
		}
	}
}
