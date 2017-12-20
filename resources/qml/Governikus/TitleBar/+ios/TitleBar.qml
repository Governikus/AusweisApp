import QtQuick 2.5
import QtQuick.Controls 2.2

import Governikus.Global 1.0

Item {
	property int duration: 300
	property alias titleBarOpacity: background.opacity
	id: titleBar
	height: Constants.titlebar_height + activeSubTitleBarAction.height

	readonly property var defaultLeftAction: Text {}
	readonly property var defaultTitle: Text {}
	readonly property var defaultRightAction: Text {}
	readonly property var defaultSubTitleBarAction: Item {}

	property var activeleftAction: leftAction ? leftAction : defaultLeftAction
	property var activeTitleItem: titleItem ? titleItem : defaultTitle
	property var activeRightAction: rightAction ? rightAction : defaultRightAction
	property var activeSubTitleBarAction: subTitleBarAction ? subTitleBarAction : defaultSubTitleBarAction

	property var leftAction
	property var titleItem
	property var rightAction
	property var subTitleBarAction
	property var color


	Rectangle {
		id: background
		anchors.fill: parent
		color: titleBar.color ? titleBar.color : Constants.blue

		Behavior on color { ColorAnimation { duration: titleBar.duration } }
	}

	Item {
		id: firstLine

		height: Constants.titlebar_height
		width: parent.width

		Item {
			data: activeleftAction
			width: activeleftAction.width
			anchors.verticalCenter: parent.verticalCenter
			anchors.left: parent.left
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
				width: parent.width
				horizontalAlignment: Text.AlignHCenter
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
				width: parent.width
				font.bold: parent.bold
				horizontalAlignment: Text.AlignHCenter

				Behavior on text {
					ParallelAnimation {
						PropertyAnimation { target: newTitle; property: "opacity"; from: 0; to: 1; duration: titleBar.duration }
						PropertyAnimation { target: newTitle; property: "x"; from: width; to: 0; duration: titleBar.duration }
					}
				}
			}
		}

		Item {
			data: activeRightAction
			width: activeRightAction.width
			anchors.verticalCenter: parent.verticalCenter
			anchors.right: parent.right
		}
	}

	Item {
		data: activeSubTitleBarAction
		width: parent.width
		anchors.top: firstLine.bottom
		anchors.horizontalCenter: parent.horizontalCenter
	}
}
