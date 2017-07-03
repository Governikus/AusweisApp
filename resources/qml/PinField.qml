import QtQuick 2.7

import "global"

Item {
	id: baseItem

	property alias text: echoField.text
	property string inputConfirmation
	readonly property bool confirmedInput: inputConfirmation.length != text.length || inputConfirmation === text
	readonly property bool validInput: echoField.acceptableInput && confirmedInput

	function append(number) {
		echoField.insert(echoField.length, number)
	}
	function removeLast() {
		echoField.remove(echoField.length-1, echoField.length)
	}

	height: sample.implicitHeight * 1.5
	width: lines.width
	clip: true

	TextInput {
		id: echoField
		verticalAlignment: TextInput.AlignVCenter
		echoMode: TextInput.Password
		font.pixelSize: Utils.sp(18)
		font.letterSpacing: Utils.dp(10)
		passwordMaskDelay: 500
		cursorVisible: false
		activeFocusOnPress: false
		focus: false
		validator: RegExpValidator {
			regExp: baseItem.state === "PUK" ? /[0-9]{10}/ :
					baseItem.state === "PIN_OR_TRANSPORT_PIN" ? /[0-9]{5,6}/ : /[0-9]{6}/ }
		maximumLength: baseItem.state === "PUK" ? 10 : 6
		clip: true
	}

	TextInput {
		id: sample
		visible: false
		echoMode: echoField.echoMode
		font: echoField.font
		text: "0"
	}

	Row {
		id: lines
		anchors.bottom: baseItem.bottom
		spacing: echoField.font.letterSpacing
		anchors.left: echoField.left

		Repeater {
			model: baseItem.state === "PUK" ? 10 : 6
			delegate:
				Rectangle {
					width: sample.contentWidth - sample.font.letterSpacing
					height: 1
					color: "black"
			   }
		}
	}
}
