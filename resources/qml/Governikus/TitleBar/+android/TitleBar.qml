import QtQuick 2.7
import QtQuick.Controls 1.4

import Governikus.Global 1.0

Item
{
	property int duration: 300
	property alias titleBarOpacity: background.opacity
	id: titleBar
	height: Constants.titlebar_height

	readonly property TitleBarAction defaultLeftAction: TitleBarAction {}
	readonly property TitleBarAction defaultTitle: TitleBarAction {}
	readonly property Item defaultRightAction: Item {}

	readonly property TitleBarAction activeleftAction: leftAction ? leftAction : defaultLeftAction
	readonly property TitleBarAction activeTitleItem: titleItem ? titleItem : defaultTitle
	readonly property Item activeRightAction: rightAction ? rightAction : defaultRightAction

	property var leftAction
	property var titleItem
	property var rightAction
	property var subTitleBarAction
	property var color


	Rectangle {
		id: background
		color: titleBar.color ? titleBar.color : Constants.blue
		anchors.top: parent.top
		anchors.left: PlatformConstants.is_tablet ? hamburgerFrame.right : parent.left
		anchors.bottom: parent.bottom
		anchors.right: parent.right

		onColorChanged: statusBarUtil.setStatusBarColor(String(color))
		Behavior on color { ColorAnimation { duration: titleBar.duration } }
	}

	Rectangle {
		id: hamburgerFrame
		height: parent.height
		width: Constants.menubar_width
		anchors.left: parent.left
		color: background.color
		opacity: PlatformConstants.is_tablet ? 1 : 0
	}

	Hamburger {
		id: burger
		anchors.horizontalCenter: hamburgerFrame.horizontalCenter
		height: parent.height
		width: height
		state: navBar.isOpen ? "back" : activeleftAction.state

		MouseArea {
			anchors.fill: parent
			onClicked: {
				switch (burger.state) {
					case "":
						navBar.open()
						break
					case "back":
						if (navBar.isOpen) {
							navBar.close()
						} else {
							activeleftAction.clicked(mouse)
						}
						break
					case "hidden":
						break
					default:
						activeleftAction.clicked(mouse)
				}
			}
		}
	}

	Item {
		id: titleText
		property string text: activeTitleItem.text
		property bool bold: activeTitleItem.font.bold

		anchors.left: burger.right
		anchors.leftMargin: Constants.titlebar_spacing
		anchors.right: parent.right
		anchors.rightMargin: Constants.titlebar_padding + (rightActionStack.width > 0 ? rightAction.width + Constants.titlebar_spacing : 0)
		height: parent.height
		clip: true

		TitleBarText {
			id: oldTitle
			text: parent.text
			Component.onCompleted: font.bold = parent.bold
			opacity: 0

			Behavior on text {
				SequentialAnimation {
					PropertyAnimation { target: oldTitle; property: "opacity"; from: 1; to: 0; duration: titleBar.duration }
					PropertyAction { target: oldTitle; property: "font.bold"; value: titleText.bold }
					PropertyAction { target: oldTitle; property: "text" }
				}
			}
		}

		TitleBarText {
			id: newTitle
			text: parent.text
			font.bold: parent.bold

			Behavior on text {
				ParallelAnimation {
					PropertyAnimation { target: newTitle; property: "opacity"; from: 0; to: 1; duration: titleBar.duration }
					PropertyAnimation { target: newTitle; property: "x"; from: width; to: 0; duration: titleBar.duration }
				}
			}
		}

	}

	Item {
		id: rightActionStack
		anchors.rightMargin: Constants.titlebar_padding
		anchors.top: parent.top
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		width: rightAction ? activeRightAction.contentWidth : 0
		data: activeRightAction
	}
}
