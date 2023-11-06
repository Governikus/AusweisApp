/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type.ApplicationModel
import Governikus.Type.RemoteServiceModel
import Governikus.Type.NumberModel
import Governikus.Type.PasswordType

FlickableSectionPage {
	id: baseItem

	property string accessibleContinueText
	property alias enableTransportPinLink: transportPinLink.visible
	property string inputError
	property alias moreInformationText: moreInformation.text
	required property int passwordType

	signal changePinLength
	signal passwordEntered(var pPasswordType)
	signal requestPasswordInfo

	fillWidth: true

	Keys.onPressed: event => {
		event.accepted = pinField.handleKeyEvent(event.key, event.modifiers);
	}
	onVisibleChanged: pinField.number = ""

	QtObject {
		id: d

		function setPassword() {
			switch (passwordType) {
			case PasswordType.PIN:
			case PasswordType.TRANSPORT_PIN:
			case PasswordType.SMART_PIN:
				NumberModel.pin = pinField.number;
				baseItem.passwordEntered(passwordType);
				break;
			case PasswordType.NEW_SMART_PIN:
			case PasswordType.NEW_PIN:
				NumberModel.newPin = pinField.number;
				baseItem.passwordEntered(passwordType);
				break;
			case PasswordType.NEW_SMART_PIN_CONFIRMATION:
			case PasswordType.NEW_PIN_CONFIRMATION:
				NumberModel.newPinConfirmation = pinField.number;
				baseItem.passwordEntered(passwordType);
				break;
			case PasswordType.CAN:
				NumberModel.can = pinField.number;
				baseItem.passwordEntered(passwordType);
				break;
			case PasswordType.PUK:
				NumberModel.puk = pinField.number;
				baseItem.passwordEntered(passwordType);
				break;
			case PasswordType.REMOTE_PIN:
				RemoteServiceModel.connectToRememberedServer(pinField.number);
				baseItem.passwordEntered(passwordType);
				break;
			}
			pinField.number = "";
		}
	}
	GridLayout {
		id: grid

		readonly property bool isLandscape: width > Math.max(infoLayout.Layout.minimumWidth, pinField.Layout.preferredWidth) + separator.effectiveImplicitWidth + numberPad.implicitWidth

		Layout.maximumHeight: Number.POSITIVE_INFINITY
		Layout.maximumWidth: Number.POSITIVE_INFINITY
		Layout.minimumWidth: Math.max(infoLayout.Layout.minimumWidth, numberPad.Layout.minimumWidth)
		columnSpacing: 0
		flow: isLandscape ? GridLayout.LeftToRight : GridLayout.TopToBottom
		rowSpacing: Constants.component_spacing

		ColumnLayout {
			id: infoLayout

			Layout.alignment: Qt.AlignCenter
			Layout.maximumWidth: Style.dimens.max_text_width
			spacing: 0

			GText {
				Layout.alignment: Qt.AlignHCenter
				horizontalAlignment: Text.AlignHCenter

				//: LABEL ANDROID IOS
				text: passwordType === PasswordType.CAN ? qsTr("Enter CAN") :
				//: LABEL ANDROID IOS
				passwordType === PasswordType.PUK ? qsTr("Enter PUK") :
				//: LABEL ANDROID IOS
				passwordType === PasswordType.REMOTE_PIN ? qsTr("Enter pairing code") :
				//: LABEL ANDROID IOS
				passwordType === PasswordType.NEW_PIN ? qsTr("Enter new ID card PIN") :
				//: LABEL ANDROID IOS
				passwordType === PasswordType.NEW_PIN_CONFIRMATION ? qsTr("Confirm new ID card PIN") :
				//: LABEL ANDROID IOS
				passwordType === PasswordType.TRANSPORT_PIN ? qsTr("Enter Transport PIN") :
				//: LABEL ANDROID IOS
				passwordType === PasswordType.SMART_PIN ? qsTr("Enter Smart-eID PIN") :
				//: LABEL ANDROID IOS
				passwordType === PasswordType.NEW_SMART_PIN ? qsTr("Enter new Smart-eID PIN") :
				//: LABEL ANDROID IOS
				passwordType === PasswordType.NEW_SMART_PIN_CONFIRMATION ? qsTr("Confirm new Smart-eID PIN") :
				//: LABEL ANDROID IOS
				qsTr("Enter ID card PIN")
				textStyle: Style.text.headline
			}
			GText {
				id: infoText

				Layout.alignment: Qt.AlignHCenter
				Layout.topMargin: Constants.text_spacing
				elide: Text.ElideRight
				horizontalAlignment: Text.AlignHCenter
				maximumLineCount: 3
				text: {
					if (!!inputError) {
						return inputError;
					}
					if (passwordType === PasswordType.SMART_PIN && NumberModel.retryCounter === 1) {
						//: INFO ANDROID IOS The wrong Smart-eID PIN was entered twice on the Smart-eID
						return qsTr("You have entered an incorrect, six-digit Smart-eID PIN twice. After the next failed attempt you will no longer be able to use your Smart-eID and will need to set it up again.");
					}
					if (passwordType === PasswordType.CAN) {
						if (NumberModel.isCanAllowedMode) {
							//: INFO ANDROID IOS The user is required to enter the six-digit CAN in CAN-allowed authentication.
							return qsTr("Please enter the six-digit Card Access Number (CAN). You can find it in the bottom right on the front of the ID card.");
						}
						//: INFO ANDROID IOS The wrong ID card PIN was entered twice, the third attempt requires the CAN for additional verification, hint where the CAN is found.
						return qsTr("A wrong ID card PIN has been entered twice on your ID card. For a third attempt, please first enter the six-digit Card Access Number (CAN). You can find your CAN in the bottom right on the front of your ID card.");
					}
					if (passwordType === PasswordType.PUK) {
						//: INFO ANDROID IOS The PUK is required to unlock the ID card since the wrong ID card PIN entered three times.
						return qsTr("You have entered an incorrect, six-digit ID card PIN thrice, your ID card PIN is now blocked. To remove the block, the ten-digit PUK must be entered first.");
					}
					if (passwordType === PasswordType.NEW_PIN) {
						//: INFO ANDROID IOS A new six-digit ID card PIN needs to be supplied.
						return qsTr("Please enter a new six-digit ID card PIN now.");
					}
					if (passwordType === PasswordType.NEW_PIN_CONFIRMATION) {
						//: INFO ANDROID IOS The new ID card PIN needs to be confirmed.
						return qsTr("Please confirm your new six-digit ID card PIN.");
					}
					if (passwordType === PasswordType.NEW_SMART_PIN) {
						//: INFO ANDROID IOS A new six-digit Smart-eID PIN needs to be supplied.
						return qsTr("Please enter a new six-digit Smart-eID PIN now.");
					}
					if (passwordType === PasswordType.NEW_SMART_PIN_CONFIRMATION) {
						//: INFO ANDROID IOS The new Smart-eID PIN needs to be confirmed.
						return qsTr("Please confirm your new six-digit Smart-eID PIN.");
					}
					if (passwordType === PasswordType.TRANSPORT_PIN) {
						//: INFO ANDROID IOS The Transport PIN is required by AA2, it needs to be change to an actual PIN.
						return qsTr("Please enter the five-digit Transport PIN.");
					}
					if (passwordType === PasswordType.REMOTE_PIN) {
						//: INFO ANDROID IOS The pairing code for the smartphone is required.
						return qsTr("Enter the pairing code shown on the device you want to pair.");
					}
					if (passwordType === PasswordType.SMART_PIN) {
						return ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_CHANGE_PIN ?
						//: INFO ANDROID IOS The AA2 expects the current Smart-eID PIN with six digits in a PIN change.
						qsTr("Please enter your current six-digit Smart-eID PIN.") :
						//: INFO ANDROID IOS The AA2 expects a Smart-eID PIN with six digits in an authentication.
						qsTr("Please enter your six-digit Smart-eID PIN.");
					}
					return ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_CHANGE_PIN ?
					//: INFO ANDROID IOS The AA2 expects the current ID card PIN with six digits in a PIN change.
					qsTr("Please enter your current six-digit ID card PIN.") :
					//: INFO ANDROID IOS The AA2 expects a ID card PIN with six digits in an authentication.
					qsTr("Please enter your six-digit ID card PIN.");
				}
				textStyle: {
					if (!!inputError || (passwordType === PasswordType.CAN && !NumberModel.isCanAllowedMode) || passwordType === PasswordType.PUK) {
						return Style.text.normal_warning;
					} else {
						return Style.text.normal;
					}
				}

				MouseArea {
					anchors.fill: parent
					anchors.margins: -12
					enabled: infoText.truncated

					onClicked: completeTextPopup.open()
				}
			}
			GSpacer {
				Layout.fillHeight: true
				visible: !grid.isLandscape
			}
			MoreInformationLink {
				id: transportPinLink

				Layout.alignment: Qt.AlignHCenter
				Layout.topMargin: Constants.text_spacing
				text: (passwordType === PasswordType.TRANSPORT_PIN ?
					//: LABEL ANDROID IOS Button to switch to a six-digit ID card PIN.
					qsTr("Do you have a six-digit ID card PIN?") :
					//: LABEL ANDROID IOS Button to start a change of the Transport PIN.
					qsTr("Do you have a five-digit Transport PIN?"))
				visible: false

				onClicked: baseItem.changePinLength()
			}
			MoreInformationLink {
				id: moreInformation

				Layout.alignment: Qt.AlignHCenter
				Layout.topMargin: Constants.text_spacing
				visible: text !== "" && passwordType !== PasswordType.REMOTE_PIN

				onClicked: baseItem.requestPasswordInfo()
			}
			NumberField {
				id: pinField

				Layout.alignment: Qt.AlignHCenter
				Layout.fillWidth: true
				Layout.topMargin: Constants.component_spacing
				padding: Constants.component_spacing / 2
				passwordLength: passwordType === PasswordType.REMOTE_PIN ? 4 : passwordType === PasswordType.TRANSPORT_PIN ? 5 : passwordType === PasswordType.PUK ? 10 : 6

				background: Rectangle {
					border.color: Style.color.border
					border.width: Style.dimens.separator_size
					color: Style.color.transparent
					radius: Style.dimens.control_radius
				}

				onAccepted: d.setPassword()
			}
		}
		GSeparator {
			id: separator

			readonly property real effectiveImplicitWidth: Layout.leftMargin + implicitWidth + Layout.rightMargin

			Layout.alignment: Qt.AlignCenter
			Layout.leftMargin: Constants.component_spacing
			Layout.preferredHeight: grid.height * 0.75
			Layout.rightMargin: Constants.component_spacing
			orientation: Qt.Vertical
			visible: grid.isLandscape
		}
		GSpacer {
			Layout.fillWidth: true
			visible: grid.isLandscape
		}
		NumberPad {
			id: numberPad

			Layout.alignment: grid.isLandscape ? Qt.AlignCenter : Qt.AlignHCenter | Qt.AlignTop
			deleteEnabled: pinField.number.length > 0
			submitAccessibleText: baseItem.accessibleContinueText !== "" ? baseItem.accessibleContinueText :
			//: LABEL ANDROID IOS
			passwordType === PasswordType.CAN ? qsTr("Send CAN") :
			//: LABEL ANDROID IOS
			passwordType === PasswordType.PUK ? qsTr("Send PUK") :
			//: LABEL ANDROID IOS
			passwordType === PasswordType.REMOTE_PIN ? qsTr("Send pairing code") :
			//: LABEL ANDROID IOS
			passwordType === PasswordType.NEW_PIN ? qsTr("Send new ID card PIN") :
			//: LABEL ANDROID IOS
			passwordType === PasswordType.NEW_PIN_CONFIRMATION ? qsTr("Send confirmation of new ID card PIN") :
			//: LABEL ANDROID IOS
			passwordType === PasswordType.TRANSPORT_PIN ? qsTr("Send Transport PIN") :
			//: LABEL ANDROID IOS
			passwordType === PasswordType.SMART_PIN ? qsTr("Send Smart-eID PIN") :
			//: LABEL ANDROID IOS
			passwordType === PasswordType.NEW_SMART_PIN ? qsTr("Send new Smart-eID PIN") :
			//: LABEL ANDROID IOS
			passwordType === PasswordType.NEW_SMART_PIN_CONFIRMATION ? qsTr("Send confirmation of new Smart-eID PIN") :
			//: LABEL ANDROID IOS
			qsTr("Send ID card PIN")
			submitEnabled: pinField.validInput

			onDeletePressed: {
				pinField.removeLast();
				if (pinField.number.length === 0)
					pinField.forceActiveFocus();
			}
			onDigitPressed: digit => pinField.append(digit)
			onSubmitPressed: d.setPassword()
		}
		GSpacer {
			Layout.fillWidth: true
			visible: grid.isLandscape
		}
	}
	ConfirmationPopup {
		id: completeTextPopup

		style: ConfirmationPopup.PopupStyle.OkButton
		text: infoText.text
	}
}
