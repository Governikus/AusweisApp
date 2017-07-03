import QtQuick 2.5
import QtQuick.Controls 1.4

import "global"

Item
{
	property int duration: 300
	property alias titleBarOpacity: background.opacity
	id: titleBar
	height: Constants.titlebar_height

	readonly property var standardMenuAction: TitleBarMenuAction { state: "" }
	readonly property var standardTitle: TitleBarAction { text: qsTr("Identify"); font.bold: true }

	function reset() {
		leftActionStack.clear()
		headerActionStack.clear()
		rightActionStack.clear()

		leftActionStack.push(standardMenuAction)
		headerActionStack.push(standardTitle)
		background.color = PlatformConstants.blue
	}

	function push(leftAction, headerAction, rightAction, color) {
		// TODO Remove Stacks because they are not longer needed
		leftActionStack.pop()
		headerActionStack.pop()
		rightActionStack.pop()

		background.color = color
		leftActionStack.push(leftAction)
		headerActionStack.push(headerAction)
		rightActionStack.push(rightAction)
	}


	function pop(item) {
		leftActionStack.pop(item)
		headerActionStack.pop(item)
		rightActionStack.pop(item)
	}


	Rectangle {
		id: background
		color: PlatformConstants.blue
		anchors.top: parent.top
		anchors.left: PlatformConstants.is_tablet ? hamburgerFrame.right : parent.left
		anchors.bottom: parent.bottom
		anchors.right: parent.right

		onColorChanged: {
			var colorString
			var colorCompare = String(color)
			switch(colorCompare) {
			case Category.CATEGORY_COLOR_CITIZEN:
				colorString = "CITIZEN"
				break;
			case Category.CATEGORY_COLOR_INSURANCE:
				colorString = "INSURANCE"
				break;
			case Category.CATEGORY_COLOR_FINANCE:
				colorString = "FINANCE"
				break;
			case Category.CATEGORY_COLOR_OTHER:
				colorString = "OTHER"
				break;
			default:
				colorString = "DEFAULT"
			}
			statusBarUtil.setStatusBarColor(colorString)
		}

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
		state: navBar.isOpen ? "back" : (leftActionStack.currentItem ? leftActionStack.currentItem.state : "")

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
							leftActionStack.currentItem.clicked()
						}
						break
					default:
						leftActionStack.currentItem.clicked()
				}
			}
		}
	}

	StackView {
		id: leftActionStack
		anchors.left: hamburgerFrame.right
		anchors.leftMargin: Utils.dp(10)
		height: parent.height
		// We dont need this view on android at this time: width = 0
		width: 0 //currentItem ? currentItem.contentWidth : parent.height
		delegate: StackViewDelegate {
			pushTransition: StackViewTransition {
				PropertyAnimation { duration: titleBar.duration; target: exitItem; property: "opacity"; to: 0 }
				PropertyAnimation { duration: titleBar.duration; target: exitItem; property: "x"; from: 0; to: -headerActionStack.x }
				PropertyAnimation { duration: titleBar.duration; target: enterItem; property: "opacity"; from: 0; to: 1 }
				PropertyAnimation { duration: titleBar.duration; target: enterItem; property: "x"; from: headerActionStack.x; to: 0 }
			}
			popTransition: StackViewTransition {
				PropertyAnimation { duration: titleBar.duration; target: exitItem; property: "opacity"; to: 0 }
				PropertyAnimation { duration: titleBar.duration; target: exitItem; property: "x"; to: headerActionStack.x }
				PropertyAnimation { duration: titleBar.duration; target: enterItem; property: "opacity"; from: 0; to: 1 }
				PropertyAnimation { duration: titleBar.duration; target: enterItem; property: "x"; from: -headerActionStack.x; to: 0 }
			}
		}
	}

	StackView {
		property int _width: parent.width - 2 * Math.max(hamburgerFrame.width, rightActionStack.width) - Utils.dp(20) // 20 == margin

		id: headerActionStack
		anchors.centerIn: parent
		height: parent.height
		width: Math.max(_width, 0)
		delegate: StackViewDelegate {
			pushTransition: StackViewTransition {
				PropertyAnimation { duration: titleBar.duration; target: exitItem; property: "opacity"; to: 0 }
				PropertyAnimation { duration: titleBar.duration; target: exitItem; property: "x"; from: 0; to: headerActionStack.getRelX(leftActionStack) }
				PropertyAnimation { duration: titleBar.duration; target: enterItem; property: "opacity"; from: 0; to: 1 }
				PropertyAnimation { duration: titleBar.duration; target: enterItem; property: "x"; from: headerActionStack.getRelX(rightActionStack); to: 0 }
			}
			popTransition: StackViewTransition {
				PropertyAnimation { duration: titleBar.duration; target: exitItem; property: "opacity"; to: 0 }
				PropertyAnimation { duration: titleBar.duration; target: exitItem; property: "x"; to: headerActionStack.getRelX(rightActionStack) }
				PropertyAnimation { duration: titleBar.duration; target: enterItem; property: "opacity"; from: 0; to: 1 }
				PropertyAnimation { duration: titleBar.duration; target: enterItem; property: "x"; from: headerActionStack.getRelX(leftActionStack); to: 0 }
			}
		}
		function getRelX(otherItem) {
			return headerActionStack.mapFromItem(otherItem,otherItem.x,0).x
		}
	}

	StackView {
		id: rightActionStack
		anchors.right: parent.right
		anchors.rightMargin: Utils.dp(10)
		anchors.verticalCenter: parent.verticalCenter
		height: parent.height
		width: currentItem ? currentItem.contentWidth : 0
		delegate: StackViewDelegate {
			pushTransition: StackViewTransition {
				PropertyAnimation { duration: titleBar.duration; target: exitItem; property: "opacity"; to: 0 }
				PropertyAnimation { duration: titleBar.duration; target: enterItem; property: "opacity"; from: 0; to: 1 }
			}
		}
	}
}
