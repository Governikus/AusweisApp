import QtQuick 2.5
import QtQuick.Controls 1.4

import "global"

Item
{
	property int duration: 300
	property alias titleBarOpacity: colorStack.opacity
	id: titleBar
	height: Constants.titlebar_height

	function reset() {
	}

	function push(leftAction, headerAction, rightAction, color) {
		colorStack.push(color)
		leftActionStack.push(leftAction)
		headerActionStack.push(headerAction)
		rightActionStack.push(rightAction)
	}


	function pop(item) {
		colorStack.pop(item)
		leftActionStack.pop(item)
		headerActionStack.pop(item)
		rightActionStack.pop(item)
	}

	function setColor(color) {
		colorStack.color = color
	}

	Rectangle {
		id: colorStack
		anchors.fill: parent
		visible: false
		property var stack

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

		function push(nextColor) {
			if (stack === undefined) {
				// Workaround for Qt 5.6-beta:
				// when *not* performing lazy initialization,
				// the stack gets reset by unknown caller...
				//
				// TODO: report it or fix it
				stack = []
			}
			stack.push(nextColor)
			color = nextColor
			visible = true
		}
		function pop(item) {
			// analogous to StackView.pop(), see QML documentation
			if (typeof item === "undefined") {
				stack.pop()
			}
			else if (item === null) {
				while (stack.length > 1) {
					stack.pop()
				}
			}
			else {
				console.error("NOT SUPPORTED")
			}
			color = stack[stack.length-1]
		}

		Behavior on color { ColorAnimation { duration: titleBar.duration } }
	}

	StackView {
		id: leftActionStack
		anchors.left: parent.left
		anchors.leftMargin: 10
		height: parent.height
		width: currentItem ? currentItem.contentWidth : parent.height
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
		property int _width: parent.width - 2 * Math.max(leftActionStack.width, rightActionStack.width) - 20 // 20 == margin

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
		anchors.rightMargin: 10
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
