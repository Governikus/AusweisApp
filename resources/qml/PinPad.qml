import QtQuick 2.5

import "global"

Item {
	id: baseItem
	property string _checkablePin
	signal pinEntered()

	onVisibleChanged: {
		if(visible) {
			// Workflow: Change-PIN
			// Wenn der Benutzer eine PIN eingegeben hat und den Workflow dann abbricht,
			// ist nach erneutem Workflow-Start die vorher eingegebene PIN initial gesetzt
			//
			// Das ist unerklärlich, da wir in der ChangePinStateMachine den state auf INITIAL
			// setzen, was zum löschen der alten PIN führt. Die alte PIN wird danach von Unbekannt
			// wieder gesetzt. FIXME!
			//
			// Die folgende Zeile ist ein Workaround
			echoField.text = ""
		}
	}

	state: "INITIAL"
	states: [
		State { name: "INITIAL"
			PropertyChanges { target: baseItem; _checkablePin: "" }
			PropertyChanges { target: echoField; text: "" }
		},
		State { name: "PIN" },
		State { name: "PIN_OR_TRANSPORT_PIN" },
		State { name: "PIN_NEW" },
		State { name: "PIN_NEW_AGAIN" },
		State { name: "CAN" },
		State { name: "PUK" }
	]

	Item {
		width: allImage.width + message.anchors.leftMargin + message.width
		height: Math.max(allImage.height, message.height)
		anchors.top: parent.top
		anchors.topMargin: Utils.dp(20)
		anchors.horizontalCenter: parent.horizontalCenter

		Image {
			id: allImage
			anchors.left: parent.left
			anchors.verticalCenter: parent.verticalCenter
			width: Utils.dp(58)
			height: Utils.dp(58)
			source: "qrc:///images/NFCPhoneCard.png"
			fillMode: Image.PreserveAspectFit
		}

		Text {
			property bool isError: echoField.pinDoesNotMatch || numberModel.inputError.length > 0

			id: message
			anchors.leftMargin: parent.anchors.topMargin
			anchors.left: allImage.right
			anchors.verticalCenter: parent.verticalCenter
			wrapMode: Text.WordWrap
			width: {
				var maxWidth = baseItem.width - allImage.width - 3 * anchors.leftMargin
				Math.min(dummyTextMessage.width, maxWidth)
			}

			font.pixelSize: isError ? Utils.sp(20) : Constants.header_font_size
			font.bold: isError ? false : true
			color: isError ? Constants.red : Constants.blue
			text: {
				if (isError) {
					echoField.pinDoesNotMatch ? qsTr("The PIN does not match.") : numberModel.inputError
				} else {
					baseItem.state === "CAN" ? qsTr("Enter your CAN") :
					baseItem.state === "PUK" ? qsTr("Enter your PUK") :
					baseItem.state === "PIN_NEW" ? qsTr("Enter your new PIN") :
					baseItem.state === "PIN_NEW_AGAIN" ? qsTr("Enter your new PIN again") : qsTr("Enter your PIN")
				}
			}
		}

		// Hidden text to calculate length of the message without word wrap
		Text {
			id: dummyTextMessage
			font.pixelSize: message.font.pixelSize
			font.bold: message.font.bold
			visible: false
			text: message.text
		}
	}

	TextInput {
		id: echoField
		anchors.bottom: numPad.top
		anchors.bottomMargin: Utils.dp(30)
		anchors.left: lines.left
		horizontalAlignment: TextInput.AlignHCenter
		echoMode: TextInput.Password
		font.pixelSize: baseItem.state === "PUK" ? Utils.sp(15) : Utils.sp(20)
		font.letterSpacing: baseItem.state === "PUK" ? Utils.dp(15) : Utils.dp(20)
		passwordMaskDelay: 500
		cursorVisible: false
		activeFocusOnPress: false
		focus: false;
		validator: RegExpValidator {
			regExp: baseItem.state === "PUK" ? /[0-9]{10}/ :
					baseItem.state === "PIN_OR_TRANSPORT_PIN" ? /[0-9]{5,6}/ : /[0-9]{6}/ }

		readonly property bool pinDoesNotMatch: baseItem.state === "PIN_NEW_AGAIN" && echoField.length === _checkablePin.length && echoField.text.localeCompare(_checkablePin) !== 0
	}

	TextInput {
		id: dummyTextInput
		anchors.left: lines.left
		horizontalAlignment: TextInput.AlignHCenter
		echoMode: echoField.echoMode
		font.pixelSize: echoField.font.pixelSize
		font.letterSpacing: echoField.font.letterSpacing
		visible: false
		text: "0"
	}

	Row {
		id: lines
		anchors.bottom: numPad.top
		anchors.bottomMargin: Utils.dp(26)
		spacing: echoField.font.letterSpacing

		anchors.horizontalCenter: parent.horizontalCenter

		Repeater {
			model: baseItem.state === "PUK" ? 10 : 6
			delegate:
				Rectangle {
						width: dummyTextInput.contentWidth - dummyTextInput.font.letterSpacing
						height: 1
						color: "black"
				   }
		}
	}

	Grid {
		id: numPad
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Utils.dp(30)

		anchors.horizontalCenter: parent.horizontalCenter

		columns: 3
		columnSpacing: Utils.dp(50)
		rowSpacing: Utils.dp(30)

		NumPadButton { text: "1"; onClicked: numPad.enterPin(1) }
		NumPadButton { text: "2"; onClicked: numPad.enterPin(2) }
		NumPadButton { text: "3"; onClicked: numPad.enterPin(3) }
		NumPadButton { text: "4"; onClicked: numPad.enterPin(4) }
		NumPadButton { text: "5"; onClicked: numPad.enterPin(5) }
		NumPadButton { text: "6"; onClicked: numPad.enterPin(6) }
		NumPadButton { text: "7"; onClicked: numPad.enterPin(7) }
		NumPadButton { text: "8"; onClicked: numPad.enterPin(8) }
		NumPadButton { text: "9"; onClicked: numPad.enterPin(9) }
		NumPadButton { source: "qrc:///images/delete.png"; onClicked: numPad.resetPin() }
		NumPadButton { text: "0"; onClicked: numPad.enterPin(0) }
		NumPadButton { source: "qrc:///images/submit.png"; visible: echoField.acceptableInput && !echoField.pinDoesNotMatch;
			onClicked: {
				switch(baseItem.state)
				{
					case "PIN":
						/* fall through */
					case "PIN_OR_TRANSPORT_PIN":
						numberModel.pin = echoField.text
						_checkablePin = ""
						baseItem.pinEntered()
						break

					case "PIN_NEW":
						baseItem.state = "PIN_NEW_AGAIN"
						_checkablePin = echoField.text
						break

					case "PIN_NEW_AGAIN":
						numberModel.newPin = _checkablePin
						_checkablePin = ""
						baseItem.pinEntered()
						break

					case "CAN":
						numberModel.can = echoField.text
						_checkablePin = ""
						baseItem.pinEntered()
						break

					case "PUK":
						numberModel.puk = echoField.text
						_checkablePin = ""
						baseItem.pinEntered()
						break
				}
				numPad.resetPin()
			}
		}

		function enterPin(pNum) {
			echoField.insert(echoField.length, pNum)
		}

		function resetPin() {
			echoField.text = ""
		}
	}
}
