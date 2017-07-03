import QtQuick 2.5
import QtQuick.Controls 1.4

import "global"

Item
{
	id: baseItem

	state: "initial"

	function isInProcessingState() {
		return state === "bt_enternewpin" || state === "bt_updateretrycounter" ||
				state === "bt_enterpuk" || state === "bt_changepin_entercan" ||
				state === "bt_changepin_enterpin" || state === "bt_identify_entercan" ||
				state === "bt_identify_enterpin"
	}

	function processingText() {
		if (state === "bt_enterpuk") {
			return qsTr("Enter PUK...")
		}
		else if (state === "bt_changepin_entercan"|| state === "bt_identify_entercan") {
			return qsTr("Enter CAN...")
		}
		else if (state === "bt_changepin_enterpin" || state === "bt_identify_enterpin") {
			return qsTr("Enter PIN...")
		}
		else if (state === "bt_enternewpin") {
			return qsTr("Enter new PIN...")
		}
		// This should never happen
		else {
			return ""
		}
	}

	Item {
		id: currentAction
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top
		anchors.bottom: pCircle.top
		anchors.margins: Utils.dp(40)
		height: parent.height * 0.25

		BusyImageIndicator {
			anchors.centerIn: parent
			width: height
			height: parent.height - Utils.dp(40)
			image: "qrc:///images/icon_Bluetooth.svg"
			running: visible
			visible: baseItem.state === "bt_connect"
		}

		CardReader {
			anchors.centerIn: parent
			height: parent.height
			visible: baseItem.state === "bt_card" || baseItem.isInProcessingState()
			cardAnimation: baseItem.state === "bt_card"
			pinFieldAnimation: baseItem.state === "bt_enternewpin"
		}
	}

	ProgressCircle {
		id: pCircle
		anchors.top: baseItem.verticalCenter
		anchors.left: parent.left
		anchors.right: parent.right
		state: {
			if (baseItem.state === "bt_connect") {
				return "one"
			}
			if (baseItem.state === "bt_card") {
				return "two"
			}
			if (baseItem.isInProcessingState()) {
				return "three"
			}
			return "initial"
		}
	}

	Text {
		id: titleText
		verticalAlignment: Text.AlignVCenter
		horizontalAlignment: Text.AlignHCenter
		font.pixelSize: Constants.header_font_size
		font.weight: Font.Bold
		color: Constants.blue
		anchors.top: pCircle.bottom
		anchors.topMargin: Utils.dp(20)
		anchors.left: parent.left
		anchors.right: parent.right

		states: [
			State { when: baseItem.state === "bt_connect";
				PropertyChanges { target: titleText; text: qsTr("Establish connection") }
			},
			State { when: baseItem.state === "bt_card";
				PropertyChanges { target: titleText; text: qsTr("Determine card") }
			},
			State { when: (baseItem.state === "bt_enternewpin" ||
						   baseItem.state === "bt_enterpuk" ||
						   baseItem.state === "bt_changepin_entercan" ||
						   baseItem.state === "bt_changepin_enterpin");
				PropertyChanges { target: titleText; text: qsTr("Change PIN") }
			},
			State { when: (baseItem.state === "bt_identify_entercan" ||
						   baseItem.state === "bt_identify_enterpin");
				PropertyChanges { target: titleText; text: qsTr("Authenticate") }
			}
		]

		transitions: [
			Transition {
				SequentialAnimation {
					NumberAnimation { target: rotation; property: "angle"; to: 90; duration: 500 }
					PropertyAction { target: titleText; property: "text" }
					NumberAnimation { target: rotation; property: "angle"; to: 0; duration: 500 }
				}
			}
		]

		transform: Rotation {
			id: rotation
			origin.x: 0
			origin.y: titleText.height/2
			axis.x: 1; axis.y: 0; axis.z: 0
			angle: 0
		}
	}


	Rectangle {
		id: subTitleTextBackground
		color: Constants.red
		anchors.centerIn: subTitleText
		width: subTitleText.width  + 2 * Utils.dp(5)
		height: subTitleText.height + 2 * Utils.dp(5)
		visible: false
	}
	Text {
		id: subTitleText
		verticalAlignment: Text.AlignVCenter
		horizontalAlignment: Text.AlignHCenter
		font.pixelSize: Constants.normal_font_size
		color: Constants.grey
		anchors.top: titleText.bottom
		anchors.topMargin: Utils.dp(10)
		anchors.horizontalCenter: baseItem.horizontalCenter
		width: baseItem.width * 0.8
		wrapMode: Text.WordWrap

		states: [
			State { when: baseItem.state === "bt_connect"
				PropertyChanges { target: subTitleText; text: qsTr("Establish connection...") }
				PropertyChanges { target: subTitleText; color: Constants.grey }
			},
			State { when: baseItem.state === "bt_card"
				PropertyChanges { target: subTitleText; text: qsTr("Insert card...") }
				PropertyChanges { target: subTitleText; color: Constants.grey }
			},
			State { when: baseItem.isInProcessingState() && numberModel.inputError
				PropertyChanges { target: subTitleText; text: numberModel.inputError }
				PropertyChanges { target: subTitleText; color: "white" }
				PropertyChanges { target: subTitleTextBackground; visible: true }
			},
			State { when: baseItem.isInProcessingState()
				PropertyChanges { target: subTitleText; text: baseItem.processingText() }
				PropertyChanges { target: subTitleText; color: Constants.grey }
			}
		]

		Behavior on text {
			SequentialAnimation {
				animations: animation
			}
		}

		transitions: [
			Transition {
				id: transition
				animations: animation
			}
		]

		SequentialAnimation {
			id: animation
			PropertyAnimation { target: subTitleText; property: "anchors.topMargin"; to: baseItem.height - subTitleText.y + subTitleText.height; duration: 500}
			PropertyAction { target: subTitleText; property: "text" }
			PropertyAction { target: subTitleText; property: "color" }
			PropertyAction { target: subTitleTextBackground; property: "visible" }
			PropertyAnimation { target: subTitleText; property: "anchors.topMargin"; to: Utils.dp(10); duration: 500 }
		}
	}
}
