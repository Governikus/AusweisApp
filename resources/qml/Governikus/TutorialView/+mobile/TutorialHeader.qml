import QtQuick 2.10
import QtGraphicalEffects 1.0

import Governikus.Global 1.0

Item {
	id: baseItem
	width: parent.width

	property alias headerImageSource: headerImage.source
	property alias titleText: title.text
	property bool overlapping: true
	property real initY
	signal clicked()

	Image{
		id: headerImage
		width: parent.width
		height: overlapping ? parent.height * (4.0/3.0) : parent.height
		fillMode: Image.Stretch

		MouseArea{
			anchors.fill: parent
			onClicked: baseItem.clicked()
		}

		Text {
			id: title
			anchors.centerIn: parent
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
