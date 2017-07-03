import QtQuick 2.7
import QtQuick.Controls 1.4

import "global"

Item
{
	property int duration: 300
	property alias titleBarOpacity: background.opacity
	id: titleBar
	height: Constants.titlebar_height

	readonly property TitleBarMenuAction standardMenuAction: TitleBarMenuAction { state: "" }
	readonly property TitleBarAction standardTitle: TitleBarAction { text: qsTr("Identify"); font.bold: true }

	property TitleBarMenuAction menuAction: standardMenuAction
	property TitleBarAction titleItem: standardTitle

	function reset() {
		rightActionStack.clear()

		setColor(PlatformConstants.blue)
		menuAction = standardMenuAction
		titleItem = standardTitle
	}

	function push(leftAction, headerAction, rightAction, color) {
		// TODO Remove Stack because we no longer need it
		rightActionStack.pop()

		setColor(color)
		menuAction = leftAction
		titleItem = headerAction
		rightActionStack.push(rightAction)
	}

	function setColor(color) {
		background.color = color
	}

	Rectangle {
		id: background
		color: PlatformConstants.blue
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
		state: navBar.isOpen ? "back" : menuAction.state

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
							menuAction.clicked()
						}
						break
					default:
						menuAction.clicked()
				}
			}
		}
	}

	Item {
		id: titleText
		property string text: titleItem.text
		property bool bold: titleItem.font.bold
		property int _width: parent.width - 2 * (Math.max(hamburgerFrame.width, rightActionStack.width) + Constants.titlebar_padding)

		anchors.centerIn: parent
		height: parent.height
		width: Math.max(_width, 0)
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

	StackView {
		id: rightActionStack
		anchors.margins: Constants.titlebar_padding
		anchors.top: parent.top
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		width: currentItem ? currentItem.contentWidth : 0
		delegate: StackViewDelegate {
			pushTransition: StackViewTransition {
				PropertyAnimation { duration: titleBar.duration; target: exitItem; property: "opacity"; to: 0 }
				PropertyAnimation { duration: titleBar.duration; target: enterItem; property: "opacity"; from: 0; to: 1 }
			}
		}
	}
}
