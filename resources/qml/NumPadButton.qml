import QtQuick 2.5

import "global"

Rectangle {
	id: button

	property alias text: textItem.text;
	property alias source: imageItem.source;
	signal clicked

	width: Utils.dp(40)
	height: width

	color: Constants.background_color
	Behavior on color { ColorAnimation { duration: 120; easing.type: Easing.OutElastic} }

	Text {
		id: textItem

		anchors.verticalCenter: parent.verticalCenter
		anchors.horizontalCenter: parent.horizontalCenter

		font.pixelSize: Utils.sp(24)
		wrapMode: Text.WordWrap
		color: "black"

		Behavior on color { ColorAnimation { duration: 120; easing.type: Easing.OutElastic} }
		states: [
			State {
				name: "pressed"
				when: mouse.pressed
				PropertyChanges {
					target: textItem
					color: Qt.lighter("black")
				}
				PropertyChanges {
					target: button
					color: Qt.darker(Constants.blue_light)
				}
			}
		]
	}

	Rectangle {
		id: imageRect
		anchors.verticalCenter: parent.verticalCenter
		anchors.horizontalCenter: parent.horizontalCenter

		Image {
			id: imageItem

			anchors.verticalCenter: parent.verticalCenter
			anchors.horizontalCenter: parent.horizontalCenter

			width: button.width
			height: button.width

			fillMode: Image.PreserveAspectFit
		}
	}

	MouseArea {
		id: mouse
		anchors.fill: parent
		anchors.margins: -Utils.sp(24)
		onClicked: parent.clicked();
	}
}
