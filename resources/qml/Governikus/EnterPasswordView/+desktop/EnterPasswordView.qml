/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
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
		anchors.bottom: retryCounter.top
		anchors.bottomMargin: Constants.component_spacing
		anchors.horizontalCenter: retryCounter.horizontalCenter
		font.bold: true
		//: LABEL DESKTOP
		text: qsTr("Attempts")
		visible: retryCounter.visible
	}
	RetryCounter {
		id: retryCounter

		anchors.left: parent.left
		anchors.margins: height
		anchors.top: parent.top
		visible: NumberModel.retryCounter >= 0 && (passwordType === PasswordType.PIN || passwordType === PasswordType.SMART_PIN)
	}
	TintableAnimation {
		id: animatedIcon

		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.top
		anchors.verticalCenterOffset: parent.height / 4
		height: Style.dimens.header_icon_size
		source: switch (passwordType) {
		case PasswordType.TRANSPORT_PIN:
			return "qrc:///images/transportpin_%1.webp".arg(Style.currentTheme.name);
		case PasswordType.CAN:
			return "qrc:///images/can.webp";
		case PasswordType.SMART_PIN:
		case PasswordType.NEW_SMART_PIN:
		case PasswordType.NEW_SMART_PIN_CONFIRMATION:
		case PasswordType.NEW_PIN_CONFIRMATION:
		case PasswordType.NEW_PIN:
		case PasswordType.PIN:
			return "qrc:///images/pin_person.webp";
		case PasswordType.PUK:
			return "qrc:///images/puk_%1.webp".arg(Style.currentTheme.name);
		case PasswordType.REMOTE_PIN:
			return "qrc:///images/pairingCode.webp";
		default:
			return "";
		}
		tintColor: Style.color.control
		tintEnabled: passwordType !== PasswordType.PUK && passwordType !== PasswordType.TRANSPORT_PIN
		visible: source.toString() !== ""
	}
	GText {
		id: mainText

		activeFocusOnTab: true
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: animatedIcon.bottom
		anchors.topMargin: Constants.component_spacing
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

		FocusFrame {
		}
	}
	GText {
		id: subText

		activeFocusOnTab: true
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: mainText.bottom
		anchors.topMargin: Constants.text_spacing
		color: NumberModel.inputError !== "" ? Style.color.text_warning : Style.color.text
		horizontalAlignment: Text.AlignHCenter
		text: {
			if (NumberModel.inputError !== "") {
				return NumberModel.inputError;
			}
			if (passwordType === PasswordType.TRANSPORT_PIN) {
				//: INFO DESKTOP The AA2 expects the Transport PIN with five digits.
				return qsTr("Please enter the five-digit Transport PIN.");
			}
			if (passwordType === PasswordType.PIN) {
				return ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_CHANGE_PIN ?
				//: INFO DESKTOP The AA2 expects the current ID card PIN with six digits in a PIN change.
				qsTr("Please enter your current six-digit ID card PIN.") :
				//: INFO DESKTOP The AA2 expects a ID card PIN with six digits in an authentication.
				qsTr("Please enter your six-digit ID card PIN.");
			}
			if (passwordType === PasswordType.SMART_PIN) {
				if (NumberModel.retryCounter === 1) {
					//: INFO DESKTOP The wrong Smart-eID PIN was entered twice on the Smart-eID
					return qsTr("You have entered an incorrect, six-digit Smart-eID PIN twice. An incorrect third attempt will invalidate your Smart-eID and you will have to set it up again.");
				}
				return ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_CHANGE_PIN ?
				//: INFO DESKTOP The AA2 expects the current Smart-eID PIN with six digits in a PIN change.
				qsTr("Please enter your current six-digit Smart-eID PIN.") :
				//: INFO DESKTOP The AA2 expects a Smart-eID PIN with six digits in an authentication.
				qsTr("Please enter your six-digit Smart-eID PIN.");
			}
			if (passwordType === PasswordType.CAN) {
				return NumberModel.isCanAllowedMode ?
				//: INFO DESKTOP The user is required to enter the six-digit CAN in CAN-allowed authentication.
				qsTr("Please enter the six-digit Card Access Number (CAN). You can find it in the bottom right on the front of the ID card.") :
				//: INFO DESKTOP The wrong ID card PIN was entered twice, the third attempt requires the CAN for additional verification, hint where the CAN is found.
				qsTr("A wrong ID card PIN has been entered twice on your ID card. For a third attempt, please first enter the six-digit Card Access Number (CAN). You can find your CAN in the bottom right on the front of your ID card.");
			}
			if (passwordType === PasswordType.PUK) {
				//: INFO DESKTOP The PUK is required to unlock the ID card since the wrong ID card PIN entered three times.
				return qsTr("You have entered an incorrect, six-digit ID card PIN thrice, your ID card PIN is now blocked. To remove the block, the ten-digit PUK must be entered first.");
			}
			if (passwordType === PasswordType.NEW_PIN) {
				//: INFO DESKTOP A new six-digit ID card PIN needs to be supplied.
				return qsTr("Please enter a new six-digit ID card PIN now.");
			}
			if (passwordType === PasswordType.NEW_PIN_CONFIRMATION) {
				//: INFO DESKTOP The new ID card PIN needs to be entered again for verification.
				return qsTr("Please confirm your new six-digit ID card PIN.");
			}
			if (passwordType === PasswordType.NEW_SMART_PIN) {
				//: INFO DESKTOP A new six-digit Smart-eID PIN needs to be supplied.
				return qsTr("Please enter a new six-digit Smart-eID PIN now.");
			}
			if (passwordType === PasswordType.NEW_SMART_PIN_CONFIRMATION) {
				//: INFO DESKTOP The new Smart-eID PIN needs to be confirmed.
				return qsTr("Please confirm your new six-digit Smart-eID PIN.");
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

		FocusFrame {
		}
	}
	MoreInformationLink {
		id: moreInformation

		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: subText.bottom
		anchors.topMargin: Constants.component_spacing * 3
		visible: text !== "" && passwordType !== PasswordType.REMOTE_PIN

		onClicked: baseItem.requestPasswordInfo()
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
			horizontalCenter: parent.horizontalCenter
			top: moreInformation.bottom
		}
		NavigationButton {
			id: button

			accessibleText: baseItem.accessibleContinueText !== "" ? baseItem.accessibleContinueText :
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
			buttonType: NavigationButton.Type.Forward
			enabled: numberField.validInput
			size: Style.dimens.huge_icon_size

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
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		deleteEnabled: numberField.number.length > 0
		submitAccessibleText: button.accessibleText
		submitEnabled: numberField.validInput

		onDeletePressed: {
			numberField.removeLast();
			if (numberField.number.length === 0)
				numberField.forceActiveFocus();
		}
		onDigitPressed: digit => numberField.append(digit)
		onSubmitPressed: d.setPassword()
	}
}
