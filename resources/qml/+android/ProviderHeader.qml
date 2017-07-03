import QtQuick 2.5

import "global"

Rectangle {
	id: baseItem

	/* this is interpreted by the SectionPage component */
	readonly property real titleBarOpacity: shadow.opacity === 1 ? 1 : 0

	property string address
	property string providerIcon
	property string providerImage
	property color transparentColor : Constants.blue

	color: "white"

	Image {
		id: headerBackgroundImage

		anchors.top:  parent.top
		anchors.left:  parent.left
		anchors.right:  parent.right

		width: parent.width
		height: parent.height * 0.7
		source: providerImage
		fillMode: Image.PreserveAspectCrop

		function transition() {
			return Math.min(1, Math.max(0, contentY/(parent.height - 2 * Constants.titlebar_height - parent.height * 0.3)))
		}

		Rectangle {
			id: shadow
			anchors.fill: parent
			color: transparentColor
			opacity: parent.transition() < 1 ? 0.5 : 1
			Behavior on opacity {
				NumberAnimation {}
			}
		}
	}

	Rectangle {
		width: headerBackgroundImage.width
		height: headerBackgroundImage.height
		color: transparentColor
		opacity: transparentColor === Constants.blue ? 0 : 0.7
	}

	Image {
		anchors.left: baseItem.left
		anchors.leftMargin: Utils.dp(30)
		anchors.verticalCenter: headerBackgroundImage.bottom
		height: Utils.dp(70)
		width: height
		source: providerIcon
		fillMode: Image.Stretch
	}

	Button {
		id: buttonText
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Utils.dp(12)
		anchors.right: parent.right
		anchors.rightMargin: Utils.dp(25)
		buttonColor: transparentColor
		text: qsTr("Online-Application")
		onClicked: {
			Qt.openUrlExternally(address)
		}
	}
}
