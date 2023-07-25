/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.PasswordType 1.0
import Governikus.Type.RemoteServiceModel 1.0

SectionPage {
	id: baseItem

	property string accessibleContinueText
	property alias moreInformationText: moreInformation.text
	property int passwordType: NumberModel.passwordType
	property alias statusIcon: statusIcon.source

	signal passwordEntered(bool pWasNewPin)
	signal requestPasswordInfo

	Keys.onPressed: event => {
		event.accepted = numberField.handleKeyEvent(event.key, event.modifiers);
	}
	onPasswordTypeChanged: numberField.inputConfirmation = ""
	onVisibleChanged: if (!visible)
		numberField.number = ""

	QtObject {
		id: d
		function setPassword() {
			if (!numberField.validInput) {
				return;
			}
			let wasNewPin = false;
			if (passwordType === PasswordType.PIN || passwordType === PasswordType.TRANSPORT_PIN || passwordType === PasswordType.SMART_PIN) {
				NumberModel.pin = numberField.number;
			} else if (passwordType === PasswordType.CAN) {
				NumberModel.can = numberField.number;
			} else if (passwordType === PasswordType.PUK) {
				NumberModel.puk = numberField.number;
			} else if (passwordType === PasswordType.NEW_PIN || passwordType === PasswordType.NEW_SMART_PIN) {
				if (numberField.inputConfirmation === "") {
					numberField.inputConfirmation = numberField.number;
					mainText.forceActiveFocus(Qt.MouseFocusReason);
				} else {
					NumberModel.newPin = numberField.number;
					numberField.inputConfirmation = "";
					wasNewPin = true;
				}
			} else if (passwordType === PasswordType.REMOTE_PIN) {
				RemoteServiceModel.connectToRememberedServer(numberField.number);
			}
			numberField.number = "";
			if (!visible)
				mainText.forceActiveFocus(Qt.MouseFocusReason);
			if (numberField.inputConfirmation === "") {
				baseItem.passwordEntered(wasNewPin);
			}
		}
	}
	GText {
		Accessible.ignored: true
		anchors.bottom: retryCounter.top
		anchors.bottomMargin: Constants.component_spacing
		anchors.horizontalCenter: retryCounter.horizontalCenter

		//: LABEL DESKTOP
		text: qsTr("Attempts")
		visible: retryCounter.visible
	}
	StatusIcon {
		id: retryCounter
		//: LABEL DESKTOP
		Accessible.name: qsTr("Remaining ID card PIN attempts: %1").arg(NumberModel.retryCounter)
		Accessible.role: Accessible.StaticText
		activeFocusOnTab: true
		anchors.left: parent.left
		anchors.margins: height
		anchors.top: parent.top
		contentBackgroundColor: Style.color.accent
		height: Style.dimens.status_icon_small
		text: NumberModel.retryCounter
		textStyle: Style.text.title_inverse
		visible: NumberModel.retryCounter >= 0 && (passwordType === PasswordType.PIN || passwordType === PasswordType.SMART_PIN)

		FocusFrame {
		}
	}
	StatusIcon {
		id: statusIcon
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.top
		anchors.verticalCenterOffset: parent.height / 4
		borderEnabled: false
		busy: true
		height: Style.dimens.status_icon_large
		source: AuthModel.readerImage
	}
	GText {
		id: mainText
		activeFocusOnTab: true
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: statusIcon.bottom
		anchors.topMargin: Constants.component_spacing
		horizontalAlignment: Text.AlignHCenter

		//: LABEL DESKTOP
		text: passwordType === PasswordType.CAN ? qsTr("Enter CAN") :
		//: LABEL DESKTOP
		passwordType === PasswordType.PUK ? qsTr("Enter PUK") :
		//: LABEL DESKTOP
		passwordType === PasswordType.REMOTE_PIN ? qsTr("Enter pairing code") :
		//: LABEL DESKTOP
		passwordType === PasswordType.NEW_PIN && numberField.inputConfirmation === "" ? qsTr("Enter new ID card PIN") :
		//: LABEL DESKTOP
		passwordType === PasswordType.NEW_PIN ? qsTr("Confirm new ID card PIN") :
		//: LABEL DESKTOP
		passwordType === PasswordType.TRANSPORT_PIN ? qsTr("Enter Transport PIN") :
		//: LABEL DESKTOP
		passwordType === PasswordType.SMART_PIN ? qsTr("Enter Smart-eID PIN") :
		//: LABEL DESKTOP
		(passwordType === PasswordType.NEW_SMART_PIN && numberField.inputConfirmation === "") ? qsTr("Enter new Smart-eID PIN") :
		//: LABEL DESKTOP
		(passwordType === PasswordType.NEW_SMART_PIN) ? qsTr("Confirm new Smart-eID PIN") :
		//: LABEL DESKTOP
		qsTr("Enter ID card PIN")
		textStyle: Style.text.header
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
		horizontalAlignment: Text.AlignHCenter
		text: {
			if (!numberField.confirmedInput) {
				return passwordType === PasswordType.NEW_SMART_PIN ?
				//: INFO DESKTOP The changed Smart-eID PIN was entered wrongfully during the confirmation process.
				qsTr("The new Smart-eID PIN and the confirmation do not match. Please correct your input.") :
				//: INFO DESKTOP The changed ID card PIN was entered wrongfully during the confirmation process.
				qsTr("The new ID card PIN and the confirmation do not match. Please correct your input.");
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
				return numberField.inputConfirmation === "" ?
				//: INFO DESKTOP A new six-digit ID card PIN needs to be supplied.
				qsTr("Please enter a new six-digit ID card PIN now.") :
				//: INFO DESKTOP The new ID card PIN needs to be entered again for verification.
				qsTr("Please confirm your new six-digit ID card PIN.");
			}
			if (passwordType === PasswordType.NEW_SMART_PIN) {
				return numberField.inputConfirmation === "" ?
				//: INFO DESKTOP A new six-digit Smart-eID PIN needs to be supplied.
				qsTr("Please enter a new six-digit Smart-eID PIN now.") :
				//: INFO DESKTOP The new Smart-eID PIN needs to be confirmed.
				qsTr("Please confirm your new six-digit Smart-eID PIN.");
			}
			if (passwordType === PasswordType.REMOTE_PIN) {
				//: INFO DESKTOP The pairing code needs to be supplied.
				return qsTr("Enter the pairing code shown on your smartphone.");
			}

			//: INFO DESKTOP Error message during PIN/CAN/PUK input procedure, the requested password type is unknown; internal error.
			return qsTr("Unknown password type:") + " " + passwordType;
		}
		textFormat: Text.StyledText
		textStyle: numberField.confirmedInput ? Style.text.header_secondary : Style.text.header_warning
		visible: text !== ""
		width: Math.min(parent.width - (2 * Constants.pane_padding), Style.dimens.max_text_width)

		FocusFrame {
		}
	}
	MoreInformationLink {
		id: moreInformation
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: subText.bottom
		anchors.topMargin: Constants.component_spacing
		visible: numberField.confirmedInput

		onClicked: baseItem.requestPasswordInfo()
	}
	Item {
		anchors.bottom: button.top
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: moreInformation.bottom

		NumberField {
			id: numberField
			passwordLength: passwordType === PasswordType.TRANSPORT_PIN ? 5 : passwordType === PasswordType.PUK ? 10 : passwordType === PasswordType.REMOTE_PIN ? 4 : 6

			onAccepted: d.setPassword()

			anchors {
				centerIn: parent
				horizontalCenterOffset: eyeWidth / 2
			}
		}
	}
	NumberPad {
		anchors.bottom: parent.bottom
		anchors.right: parent.right
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
		passwordType === PasswordType.NEW_PIN && numberField.inputConfirmation === "" ? qsTr("Send new ID card PIN") :
		//: LABEL DESKTOP
		passwordType === PasswordType.NEW_PIN ? qsTr("Confirm new ID card PIN") :
		//: LABEL DESKTOP
		passwordType === PasswordType.TRANSPORT_PIN ? qsTr("Send Transport PIN") :
		//: LABEL DESKTOP
		passwordType === PasswordType.SMART_PIN ? qsTr("Send Smart-eID PIN") :
		//: LABEL DESKTOP
		(passwordType === PasswordType.NEW_SMART_PIN && numberField.inputConfirmation === "") ? qsTr("Send new Smart-eID PIN") :
		//: LABEL DESKTOP
		(passwordType === PasswordType.NEW_SMART_PIN) ? qsTr("Confirm new Smart-eID PIN") :
		//: LABEL DESKTOP
		qsTr("Send ID card PIN")
		activeFocusOnTab: true
		buttonType: NavigationButton.Type.Forward
		enabled: numberField.validInput

		onClicked: {
			d.setPassword();
		}

		anchors {
			bottom: parent.bottom
			horizontalCenter: parent.horizontalCenter
			margins: Constants.component_spacing
		}
	}
}
