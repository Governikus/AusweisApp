/*
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	id: baseItem

	property alias text: echoField.text
	property int passwordLength: 6
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
		color: Style.color.primary_text
		verticalAlignment: TextInput.AlignVCenter
		echoMode: TextInput.Password
		font.pixelSize: 18
		font.letterSpacing: 10
		passwordMaskDelay: 500
		cursorVisible: false
		activeFocusOnPress: false
		focus: false
		validator: RegExpValidator {
			regExp: new RegExp("[0-9]{" + echoField.maximumLength + "}")
		}
		maximumLength: baseItem.passwordLength
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
			model: baseItem.passwordLength
			delegate: Rectangle {
				width: sample.contentWidth - sample.font.letterSpacing
				height: 1
				color: "black"
		   }
		}
	}
}
