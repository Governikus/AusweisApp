/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Animations
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type.ApplicationModel
import Governikus.Type.AuthModel
import Governikus.Type.NumberModel
import Governikus.Type.PasswordType
import Governikus.Type.RemoteServiceModel

SectionPage {
	id: baseItem

	property string accessibleContinueText
	property alias moreInformationText: moreInformation.text
	property int passwordType: NumberModel.passwordType

	signal passwordEntered(var pPasswordType)
	signal requestPasswordInfo

	Keys.onPressed: event => {
		event.accepted = numberField.handleKeyEvent(event.key, event.modifiers);
	}
	onVisibleChanged: if (!visible)
		numberField.number = ""

	QtObject {
		id: d

		function setPassword() {
			let currentPasswordType = passwordType; // The passwordType binding changes once we set any PIN/CAN/whatever
			switch (currentPasswordType) {
			case PasswordType.PIN:
			case PasswordType.TRANSPORT_PIN:
			case PasswordType.SMART_PIN:
				NumberModel.pin = numberField.number;
				baseItem.passwordEntered(currentPasswordType);
				break;
			case PasswordType.NEW_SMART_PIN:
			case PasswordType.NEW_PIN:
				NumberModel.newPin = numberField.number;
				mainText.forceActiveFocus(Qt.MouseFocusReason);
				baseItem.passwordEntered(currentPasswordType);
				break;
			case PasswordType.NEW_SMART_PIN_CONFIRMATION:
			case PasswordType.NEW_PIN_CONFIRMATION:
				NumberModel.newPinConfirmation = numberField.number;
				mainText.forceActiveFocus(Qt.MouseFocusReason);
				baseItem.passwordEntered(currentPasswordType);
				break;
			case PasswordType.CAN:
				NumberModel.can = numberField.number;
				baseItem.passwordEntered(currentPasswordType);
				break;
			case PasswordType.PUK:
				NumberModel.puk = numberField.number;
				baseItem.passwordEntered(currentPasswordType);
				break;
			case PasswordType.REMOTE_PIN:
				RemoteServiceModel.connectToRememberedServer(numberField.number);
				baseItem.passwordEntered(currentPasswordType);
				break;
			}
			numberField.number = "";
			if (!visible)
				mainText.forceActiveFocus(Qt.MouseFocusReason);
		}
	}
	GText {
		Accessible.ignored: true
		font.bold: true
		//: LABEL DESKTOP
		text: qsTr("Attempts")
		visible: retryCounter.visible

		anchors {
			bottom: retryCounter.top
			bottomMargin: Constants.component_spacing
			horizontalCenter: retryCounter.horizontalCenter
		}
	}
	RetryCounter {
		id: retryCounter

		visible: NumberModel.retryCounter >= 0 && (passwordType === PasswordType.PIN || passwordType === PasswordType.SMART_PIN)

		anchors {
			left: parent.left
			margins: height
			top: parent.top
		}
	}
	AnimationLoader {
		id: animatedIcon

		type: switch (baseItem.passwordType) {
		case PasswordType.TRANSPORT_PIN:
			return AnimationLoader.Type.ENTER_TRANSPORT_PIN;
		case PasswordType.CAN:
			return AnimationLoader.Type.ENTER_CAN;
		case PasswordType.SMART_PIN:
		case PasswordType.PIN:
			return AnimationLoader.Type.ENTER_PIN;
		case PasswordType.NEW_PIN_CONFIRMATION:
		case PasswordType.NEW_PIN:
		case PasswordType.NEW_SMART_PIN:
		case PasswordType.NEW_SMART_PIN_CONFIRMATION:
			return AnimationLoader.Type.ENTER_NEW_PIN;
		case PasswordType.PUK:
			return AnimationLoader.Type.ENTER_PUK;
		case PasswordType.REMOTE_PIN:
			return AnimationLoader.Type.ENTER_REMOTE_PIN;
		default:
			return AnimationLoader.Type.NONE;
		}

		anchors {
			horizontalCenter: parent.horizontalCenter
			top: parent.top
			topMargin: Constants.component_spacing
		}
	}
	GText {
		id: mainText

		activeFocusOnTab: true
		horizontalAlignment: Text.AlignHCenter

		//: LABEL DESKTOP
		text: passwordType === PasswordType.CAN ? qsTr("Enter CAN") :
		//: LABEL DESKTOP
		passwordType === PasswordType.PUK ? qsTr("Enter PUK") :
		//: LABEL DESKTOP
		passwordType === PasswordType.REMOTE_PIN ? qsTr("Enter pairing code") :
		//: LABEL DESKTOP
		passwordType === PasswordType.NEW_PIN ? qsTr("Enter new ID card PIN") :
		//: LABEL DESKTOP
		passwordType === PasswordType.NEW_PIN_CONFIRMATION ? qsTr("Confirm new ID card PIN") :
		//: LABEL DESKTOP
		passwordType === PasswordType.TRANSPORT_PIN ? qsTr("Enter Transport PIN") :
		//: LABEL DESKTOP
		passwordType === PasswordType.SMART_PIN ? qsTr("Enter Smart-eID PIN") :
		//: LABEL DESKTOP
		passwordType === PasswordType.NEW_SMART_PIN ? qsTr("Enter new Smart-eID PIN") :
		//: LABEL DESKTOP
		passwordType === PasswordType.NEW_SMART_PIN_CONFIRMATION ? qsTr("Confirm new Smart-eID PIN") :
		//: LABEL DESKTOP
		qsTr("Enter ID card PIN")
		textStyle: Style.text.headline
		visible: text !== ""
		width: Math.min(parent.width - (2 * Constants.pane_padding), Style.dimens.max_text_width)

		anchors {
			horizontalCenter: parent.horizontalCenter
			top: animatedIcon.bottom
			topMargin: Constants.component_spacing
		}
		FocusFrame {
		}
	}
	GText {
		id: subText

		activeFocusOnTab: true
		color: NumberModel.inputError !== "" ? Style.color.warning : Style.color.text
		horizontalAlignment: Text.AlignHCenter
		text: {
			if (NumberModel.inputError !== "") {
				return NumberModel.inputError;
			}
			if (passwordType === PasswordType.TRANSPORT_PIN) {
				//: INFO DESKTOP The AA2 expects the Transport PIN with five digits.
				return qsTr("Please enter the 5-digit Transport PIN.");
			}
			if (passwordType === PasswordType.PIN) {
				return ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_CHANGE_PIN ?
				//: INFO DESKTOP The AA2 expects the current ID card PIN with six digits in a PIN change.
				qsTr("Please enter your current 6-digit ID card PIN.") :
				//: INFO DESKTOP The AA2 expects a ID card PIN with six digits in an authentication.
				qsTr("Please enter your 6-digit ID card PIN.");
			}
			if (passwordType === PasswordType.SMART_PIN) {
				if (NumberModel.retryCounter === 1) {
					//: INFO DESKTOP The wrong Smart-eID PIN was entered twice on the Smart-eID
					return qsTr("You have entered an incorrect, 6-digit Smart-eID PIN 2 times. An incorrect 3rd attempt will invalidate your Smart-eID and you will have to set it up again.");
				}
				return ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_CHANGE_PIN ?
				//: INFO DESKTOP The AA2 expects the current Smart-eID PIN with six digits in a PIN change.
				qsTr("Please enter your current 6-digit Smart-eID PIN.") :
				//: INFO DESKTOP The AA2 expects a Smart-eID PIN with six digits in an authentication.
				qsTr("Please enter your 6-digit Smart-eID PIN.");
			}
			if (passwordType === PasswordType.CAN) {
				return NumberModel.isCanAllowedMode ?
				//: INFO DESKTOP The user is required to enter the 6-digit CAN in CAN-allowed authentication.
				qsTr("Please enter the 6-digit Card Access Number (CAN). You can find it in the bottom right on the front of the ID card.") :
				//: INFO DESKTOP The wrong ID card PIN was entered twice, the 3rd attempt requires the CAN for additional verification, hint where the CAN is found.
				qsTr("A wrong ID card PIN has been entered 2 times on your ID card. For a 3rd attempt, please first enter the 6-digit Card Access Number (CAN). You can find your CAN in the bottom right on the front of your ID card.");
			}
			if (passwordType === PasswordType.PUK) {
				//: INFO DESKTOP The PUK is required to unlock the ID card since the wrong ID card PIN entered three times.
				return qsTr("You have entered an incorrect, 6-digit ID card PIN 3 times, your ID card PIN is now blocked. To remove the block, the 10-digit PUK must be entered first.");
			}
			if (passwordType === PasswordType.NEW_PIN) {
				//: INFO DESKTOP A new 6-digit ID card PIN needs to be supplied.
				return qsTr("Please enter a new 6-digit ID card PIN now.");
			}
			if (passwordType === PasswordType.NEW_PIN_CONFIRMATION) {
				//: INFO DESKTOP The new ID card PIN needs to be entered again for verification.
				return qsTr("Please confirm your new 6-digit ID card PIN.");
			}
			if (passwordType === PasswordType.NEW_SMART_PIN) {
				//: INFO DESKTOP A new 6-digit Smart-eID PIN needs to be supplied.
				return qsTr("Please enter a new 6-digit Smart-eID PIN now.");
			}
			if (passwordType === PasswordType.NEW_SMART_PIN_CONFIRMATION) {
				//: INFO DESKTOP The new Smart-eID PIN needs to be confirmed.
				return qsTr("Please confirm your new 6-digit Smart-eID PIN.");
			}
			if (passwordType === PasswordType.REMOTE_PIN) {
				//: INFO DESKTOP The pairing code needs to be supplied.
				return qsTr("Enter the pairing code shown on your smartphone.");
			}

			//: INFO DESKTOP Error message during PIN/CAN/PUK input procedure, the requested password type is unknown; internal error.
			return qsTr("Unknown password type:") + " " + passwordType;
		}
		visible: text !== ""
		width: Math.min(parent.width - (2 * Constants.pane_padding), Style.dimens.max_text_width)

		anchors {
			horizontalCenter: parent.horizontalCenter
			top: mainText.bottom
			topMargin: Constants.text_spacing
		}
		FocusFrame {
		}
	}
	MoreInformationLink {
		id: moreInformation

		visible: text !== "" && passwordType !== PasswordType.REMOTE_PIN

		onClicked: baseItem.requestPasswordInfo()

		anchors {
			horizontalCenter: parent.horizontalCenter
			top: subText.bottom
			topMargin: Constants.component_spacing * 3
		}
	}
	Item {
		RoundedRectangle {
			id: numberFieldContainer

			anchors.centerIn: parent
			borderColor: Style.color.border
			height: numberField.height + Constants.component_spacing
			width: numberField.width + 2 * Constants.component_spacing

			NumberField {
				id: numberField

				anchors.centerIn: parent
				passwordLength: passwordType === PasswordType.TRANSPORT_PIN ? 5 : passwordType === PasswordType.PUK ? 10 : passwordType === PasswordType.REMOTE_PIN ? 4 : 6

				onAccepted: d.setPassword()
			}
		}
		anchors {
			bottom: parent.bottom
			bottomMargin: Constants.component_spacing * 5
			horizontalCenter: parent.horizontalCenter
			top: moreInformation.bottom
		}
		NavigationButton {
			id: button

			Accessible.name: baseItem.accessibleContinueText !== "" ? baseItem.accessibleContinueText :
			//: LABEL DESKTOP
			passwordType === PasswordType.CAN ? qsTr("Send CAN") :
			//: LABEL DESKTOP
			passwordType === PasswordType.PUK ? qsTr("Send PUK") :
			//: LABEL DESKTOP
			passwordType === PasswordType.REMOTE_PIN ? qsTr("Send pairing code") :
			//: LABEL DESKTOP
			passwordType === PasswordType.NEW_PIN ? qsTr("Send new ID card PIN") :
			//: LABEL DESKTOP
			passwordType === PasswordType.NEW_PIN_CONFIRMATION ? qsTr("Confirm new ID card PIN") :
			//: LABEL DESKTOP
			passwordType === PasswordType.TRANSPORT_PIN ? qsTr("Send Transport PIN") :
			//: LABEL DESKTOP
			passwordType === PasswordType.SMART_PIN ? qsTr("Send Smart-eID PIN") :
			//: LABEL DESKTOP
			passwordType === PasswordType.NEW_SMART_PIN ? qsTr("Send new Smart-eID PIN") :
			//: LABEL DESKTOP
			passwordType === PasswordType.NEW_SMART_PIN_CONFIRMATION ? qsTr("Confirm new Smart-eID PIN") :
			//: LABEL DESKTOP
			qsTr("Send ID card PIN")
			activeFocusOnTab: true
			enabled: numberField.validInput

			onClicked: {
				d.setPassword();
			}

			anchors {
				left: numberFieldContainer.right
				leftMargin: Constants.component_spacing * 2
				verticalCenter: numberFieldContainer.verticalCenter
			}
		}
	}
	NumberPad {
		deleteEnabled: numberField.number.length > 0
		submitAccessibleText: button.Accessible.name
		submitEnabled: numberField.validInput

		onDeletePressed: {
			numberField.removeLast();
			if (numberField.number.length === 0)
				numberField.forceActiveFocus();
		}
		onDigitPressed: digit => numberField.append(digit)
		onSubmitPressed: d.setPassword()

		anchors {
			bottom: parent.bottom
			left: parent.left
		}
	}
}
