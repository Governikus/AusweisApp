/*
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0

Item {
	id: baseItem

	property alias text: echoField.text
	property alias passwordLength: echoField.maximumLength
	readonly property bool validInput: echoField.acceptableInput && confirmedInput

	property string inputConfirmation
	readonly property bool confirmedInput: inputConfirmation.length != text.length || inputConfirmation === text

	function append(number) {
		echoField.insert(echoField.length, number)
	}
	function removeLast() {
		echoField.remove(echoField.length-1, echoField.length)
	}

	height: echoField.height + lines.anchors.topMargin + lines.height
	width: lines.width

	FocusFrame {
		marginFactor: 2
	}

	TextInput {
		id: sample
		visible: false
		echoMode: echoField.echoMode
		font: echoField.font
		text: "0"
	}

	TextInput {
		id: echoField
		anchors.top: parent.top
		anchors.left: lines.left
		anchors.leftMargin: sample.font.letterSpacing / 4

		color: Constants.white
		verticalAlignment: TextInput.AlignVCenter
		echoMode: TextInput.Password
		font.bold: true
		font.pixelSize: ApplicationModel.scaleFactor * 50
		font.letterSpacing: ApplicationModel.scaleFactor * 40
		passwordMaskDelay: 500
		cursorVisible: false
		activeFocusOnPress: false
		focus: false
		validator: RegExpValidator {
			regExp: new RegExp("[0-9]{" + echoField.maximumLength + "}")
		}
		maximumLength: 6
	}

	Row {
		id: lines
		anchors.top: echoField.bottom
		anchors.topMargin: Constants.groupbox_spacing
		anchors.horizontalCenter: parent.horizontalCenter

		spacing: echoField.font.letterSpacing / 2

		Repeater {
			model: baseItem.passwordLength
			delegate: Rectangle {
				width: sample.contentWidth - (sample.font.letterSpacing / 2)
				height: Math.max(ApplicationModel.scaleFactor * 4, 1)
				color: Constants.white
		   }
		}
	}
}
