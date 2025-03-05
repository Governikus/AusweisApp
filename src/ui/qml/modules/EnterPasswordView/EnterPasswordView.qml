/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Animations
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type

FlickableSectionPage {
	id: root

	property string accessibleContinueText
	property alias enableTransportPinLink: transportPinLink.visible
	property alias moreInformationText: moreInformation.text
	required property int passwordType

	signal changePinLength
	signal passwordEntered(var pPasswordType)
	signal requestPasswordInfo

	fillWidth: true

	Keys.onPressed: event => {
		event.accepted = pinField.handleKeyEvent(event.key, event.modifiers);
	}
	onVisibleChanged: if (!visible)
		pinField.number = ""

	QtObject {
		id: d

		function setPassword() {
			// On desktop the passwordType binding changes once we set any PIN/CAN/whatever. If we
			// remove the tmp var it will lead to loops while entering PINs and users cannot reach
			// further workflow steps. On mobile this is irrelevant, since passwordType is static there.
			let currentPasswordType = root.passwordType;
			switch (currentPasswordType) {
			case NumberModel.PasswordType.PIN:
			case NumberModel.PasswordType.TRANSPORT_PIN:
			case NumberModel.PasswordType.SMART_PIN:
				NumberModel.pin = pinField.number;
				root.passwordEntered(currentPasswordType);
				break;
			case NumberModel.PasswordType.NEW_SMART_PIN:
			case NumberModel.PasswordType.NEW_PIN:
				NumberModel.newPin = pinField.number;
				mainText.forceActiveFocus(Qt.MouseFocusReason);
				root.passwordEntered(currentPasswordType);
				break;
			case NumberModel.PasswordType.NEW_SMART_PIN_CONFIRMATION:
			case NumberModel.PasswordType.NEW_PIN_CONFIRMATION:
				NumberModel.newPinConfirmation = pinField.number;
				mainText.forceActiveFocus(Qt.MouseFocusReason);
				root.passwordEntered(currentPasswordType);
				break;
			case NumberModel.PasswordType.CAN:
				NumberModel.can = pinField.number;
				root.passwordEntered(currentPasswordType);
				break;
			case NumberModel.PasswordType.PUK:
				NumberModel.puk = pinField.number;
				root.passwordEntered(currentPasswordType);
				break;
			case NumberModel.PasswordType.REMOTE_PIN:
				RemoteServiceModel.connectToRememberedServer(pinField.number);
				root.passwordEntered(currentPasswordType);
				break;
			}
			pinField.number = "";
		}
	}
	GridLayout {
		id: grid

		readonly property bool isLandscape: width > Math.max(infoLayout.Layout.minimumWidth, pinField.Layout.preferredWidth) + separator.implicitWidth + numberPad.implicitWidth

		Layout.maximumHeight: Number.POSITIVE_INFINITY
		Layout.maximumWidth: Number.POSITIVE_INFINITY
		Layout.minimumHeight: isLandscape ? Math.max(infoLayout.Layout.minimumHeight, numberPad.Layout.minimumHeight) : (infoLayout.Layout.minimumHeight + rowSpacing + numberPad.Layout.minimumHeight)
		Layout.minimumWidth: Math.max(infoLayout.Layout.minimumWidth, numberPad.Layout.minimumWidth)
		Layout.preferredHeight: implicitHeight
		columnSpacing: 0
		flow: isLandscape ? GridLayout.LeftToRight : GridLayout.TopToBottom
		rowSpacing: Style.dimens.pane_spacing

		GSpacer {
			Layout.fillWidth: true
			visible: grid.isLandscape
		}
		ColumnLayout {
			id: infoLayout

			Layout.alignment: Qt.AlignCenter
			Layout.maximumWidth: Style.dimens.max_text_width
			spacing: 0

			AnimationLoader {
				Layout.alignment: Qt.AlignHCenter
				Layout.bottomMargin: Style.dimens.pane_spacing
				type: {
					if (grid.isLandscape && !Style.is_layout_desktop) {
						return AnimationLoader.NONE;
					}
					switch (root.passwordType) {
					case NumberModel.PasswordType.TRANSPORT_PIN:
						return AnimationLoader.TRANSPORT_PIN;
					case NumberModel.PasswordType.CAN:
						return AnimationLoader.CAN;
					case NumberModel.PasswordType.SMART_PIN:
					case NumberModel.PasswordType.PIN:
						return AnimationLoader.PIN;
					case NumberModel.PasswordType.NEW_PIN_CONFIRMATION:
					case NumberModel.PasswordType.NEW_PIN:
					case NumberModel.PasswordType.NEW_SMART_PIN:
					case NumberModel.PasswordType.NEW_SMART_PIN_CONFIRMATION:
						return AnimationLoader.NEW_PIN;
					case NumberModel.PasswordType.PUK:
						return AnimationLoader.PUK;
					case NumberModel.PasswordType.REMOTE_PIN:
						return AnimationLoader.REMOTE_PIN;
					default:
						return AnimationLoader.NONE;
					}
				}
			}
			GText {
				id: mainText

				Layout.alignment: Qt.AlignHCenter
				horizontalAlignment: Text.AlignHCenter

				//: LABEL ALL_PLATFORMS This is the large main text below the icon.
				text: root.passwordType === NumberModel.PasswordType.CAN ? qsTr("Enter CAN") :
				//: LABEL ALL_PLATFORMS This is the large main text below the icon.
				root.passwordType === NumberModel.PasswordType.PUK ? qsTr("Enter PUK") :
				//: LABEL ALL_PLATFORMS This is the large main text below the icon.
				root.passwordType === NumberModel.PasswordType.REMOTE_PIN ? qsTr("Enter pairing code") :
				//: LABEL ALL_PLATFORMS This is the large main text below the icon.
				root.passwordType === NumberModel.PasswordType.NEW_PIN ? qsTr("Choose new ID card PIN") :
				//: LABEL ALL_PLATFORMS This is the large main text below the icon.
				root.passwordType === NumberModel.PasswordType.NEW_PIN_CONFIRMATION ? qsTr("Confirm ID card PIN") :
				//: LABEL ALL_PLATFORMS This is the large main text below the icon.
				root.passwordType === NumberModel.PasswordType.TRANSPORT_PIN ? qsTr("Enter Transport PIN") :
				//: LABEL ALL_PLATFORMS This is the large main text below the icon.
				root.passwordType === NumberModel.PasswordType.SMART_PIN ? qsTr("Enter Smart-eID PIN") :
				//: LABEL ALL_PLATFORMS This is the large main text below the icon.
				root.passwordType === NumberModel.PasswordType.NEW_SMART_PIN ? qsTr("Choose new Smart-eID PIN") :
				//: LABEL ALL_PLATFORMS This is the large main text below the icon.
				root.passwordType === NumberModel.PasswordType.NEW_SMART_PIN_CONFIRMATION ? qsTr("Confirm Smart-eID PIN") :
				//: LABEL ALL_PLATFORMS This is the large main text below the icon.
				qsTr("Enter ID card PIN")
				textStyle: Style.text.headline
			}
			GText {
				id: infoText

				Layout.alignment: Qt.AlignHCenter
				Layout.topMargin: Style.dimens.text_spacing
				elide: Text.ElideRight
				horizontalAlignment: Text.AlignHCenter
				maximumLineCount: 3
				text: {
					if (root.passwordType === NumberModel.PasswordType.CAN) {
						//: INFO ALL_PLATFORMS The user is required to enter the 6-digit CAN. This is the description for the main text.
						return qsTr("Please enter the CAN. You can find the CAN in the bottom right on the front of the ID card.");
					}
					if (root.passwordType === NumberModel.PasswordType.PUK) {
						//: INFO ALL_PLATFORMS The PUK is required to unlock the ID card since the wrong ID card PIN entered three times. This is the description for the main text.
						return qsTr("Please enter your 10-digit PUK.");
					}
					if (root.passwordType === NumberModel.PasswordType.NEW_PIN) {
						//: INFO ALL_PLATFORMS A new 6-digit ID card PIN needs to be supplied. This is the description for the main text.
						return qsTr("You will enter this 6-digit PIN every time you want to use your ID card online.");
					}
					if (root.passwordType === NumberModel.PasswordType.NEW_PIN_CONFIRMATION) {
						//: INFO ALL_PLATFORMS The new ID card PIN needs to be confirmed. This is the description for the main text.
						return qsTr("The PIN is going to be tied to your ID card. It always stays the same regardless of the device you want to use your ID card with.");
					}
					if (root.passwordType === NumberModel.PasswordType.NEW_SMART_PIN) {
						//: INFO ALL_PLATFORMS A new 6-digit Smart-eID PIN needs to be supplied. This is the description for the main text.
						return qsTr("You will enter this 6-digit PIN every time you want to use your Smart-eID.");
					}
					if (root.passwordType === NumberModel.PasswordType.NEW_SMART_PIN_CONFIRMATION) {
						//: INFO ALL_PLATFORMS The new Smart-eID PIN needs to be confirmed. This is the description for the main text.
						return qsTr("The PIN is going to be tied to your Smart-eID. It always stays the same regardless of the service you want to use your Smart-eID with.");
					}
					if (root.passwordType === NumberModel.PasswordType.TRANSPORT_PIN) {
						//: INFO ALL_PLATFORMS The Transport PIN is required by AA, it needs to be change to an actual PIN. This is the description for the main text.
						return qsTr("You will find the Transport PIN in the letter that was sent to you after you applied for your ID card.");
					}
					if (root.passwordType === NumberModel.PasswordType.REMOTE_PIN) {
						return Style.is_layout_desktop ?
						//: INFO DESKTOP The pairing code needs to be supplied. This is the description for the main text.
						qsTr("Enter the pairing code shown on your smartphone.") :
						//: INFO MOBILE The pairing code for the smartphone is required. This is the description for the main text.
						qsTr("Enter the pairing code shown on the device you want to pair.");
					}
					if (root.passwordType === NumberModel.PasswordType.SMART_PIN) {
						if (NumberModel.retryCounter === 1) {
							//: INFO ALL_PLATFORMS The wrong Smart-eID PIN was entered twice on the Smart-eID
							return qsTr("You have entered an incorrect, 6-digit Smart-eID PIN 2 times. After the next failed attempt you will no longer be able to use your Smart-eID and will need to set it up again.");
						}
						return ApplicationModel.currentWorkflow === ApplicationModel.Workflow.CHANGE_PIN ?
						//: INFO ALL_PLATFORMS The AA expects the current Smart-eID PIN with six digits in a PIN change. This is the description for the main text.
						qsTr("Please enter your current 6-digit Smart-eID PIN.") :
						//: INFO ALL_PLATFORMS The AA expects a Smart-eID PIN with six digits in an authentication. This is the description for the main text.
						qsTr("Please enter your 6-digit Smart-eID PIN.");
					}
					return ApplicationModel.currentWorkflow === ApplicationModel.Workflow.CHANGE_PIN ?
					//: INFO ALL_PLATFORMS The AA expects the current ID card PIN with six digits in a PIN change. This is the description for the main text.
					qsTr("You have chosen the 6-digit ID card PIN yourself or received it via the PIN Reset Service.") :
					//: INFO ALL_PLATFORMS The AA expects a ID card PIN with six digits in an authentication. This is the description for the main text.
					qsTr("Please enter your 6-digit ID card PIN.");
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
				Layout.topMargin: Style.dimens.text_spacing
				text: (root.passwordType === NumberModel.PasswordType.TRANSPORT_PIN ?
					//: LABEL ALL_PLATFORMS Button to switch to a 6-digit ID card PIN.
					qsTr("Do you have a 6-digit ID card PIN?") :
					//: LABEL ALL_PLATFORMS Button to start a change of the Transport PIN.
					qsTr("Do you have a 5-digit Transport PIN?"))
				visible: false

				onClicked: root.changePinLength()
			}
			MoreInformationLink {
				id: moreInformation

				Layout.alignment: Qt.AlignHCenter
				Layout.topMargin: Style.dimens.text_spacing
				visible: text !== "" && root.passwordType !== NumberModel.PasswordType.REMOTE_PIN

				onClicked: root.requestPasswordInfo()
			}
			NumberField {
				id: pinField

				Layout.alignment: Qt.AlignHCenter
				Layout.fillWidth: true
				Layout.topMargin: Style.dimens.pane_spacing
				padding: Style.dimens.pane_spacing / 2
				passwordLength: root.passwordType === NumberModel.PasswordType.REMOTE_PIN ? 4 : root.passwordType === NumberModel.PasswordType.TRANSPORT_PIN ? 5 : root.passwordType === NumberModel.PasswordType.PUK ? 10 : 6

				background: Rectangle {
					border.color: Style.color.border
					border.width: Style.dimens.separator_size
					color: Style.color.transparent
					radius: Style.dimens.control_radius
				}

				onAccepted: d.setPassword()
			}
		}
		ColumnLayout {
			id: separator

			visible: grid.isLandscape

			GSpacer {
				Layout.fillHeight: true
				Layout.preferredHeight: 1 / 8
			}
			GSeparator {
				Layout.fillHeight: true
				Layout.leftMargin: Style.dimens.pane_spacing
				Layout.preferredHeight: 6 / 8
				Layout.rightMargin: Style.dimens.pane_spacing
				orientation: Qt.Vertical
			}
			GSpacer {
				Layout.fillHeight: true
				Layout.preferredHeight: 1 / 8
			}
		}
		GSpacer {
			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.pane_spacing
			visible: grid.isLandscape
		}
		NumberPad {
			id: numberPad

			Layout.alignment: grid.isLandscape ? Qt.AlignCenter : Qt.AlignHCenter | Qt.AlignTop
			deleteEnabled: pinField.number.length > 0
			submitAccessibleText: root.accessibleContinueText !== "" ? root.accessibleContinueText :
			//: LABEL ALL_PLATFORMS This is the accessible text on the send/confirm button of the number pad.
			root.passwordType === NumberModel.PasswordType.CAN ? qsTr("Send CAN") :
			//: LABEL ALL_PLATFORMS This is the accessible text on the send/confirm button of the number pad.
			root.passwordType === NumberModel.PasswordType.PUK ? qsTr("Send PUK") :
			//: LABEL ALL_PLATFORMS This is the accessible text on the send/confirm button of the number pad.
			root.passwordType === NumberModel.PasswordType.REMOTE_PIN ? qsTr("Send pairing code") :
			//: LABEL ALL_PLATFORMS This is the accessible text on the send/confirm button of the number pad.
			root.passwordType === NumberModel.PasswordType.NEW_PIN ? qsTr("Send new ID card PIN") :
			//: LABEL ALL_PLATFORMS This is the accessible text on the send/confirm button of the number pad.
			root.passwordType === NumberModel.PasswordType.NEW_PIN_CONFIRMATION ? qsTr("Confirm new ID card PIN") :
			//: LABEL ALL_PLATFORMS This is the accessible text on the send/confirm button of the number pad.
			root.passwordType === NumberModel.PasswordType.TRANSPORT_PIN ? qsTr("Send Transport PIN") :
			//: LABEL ALL_PLATFORMS This is the accessible text on the send/confirm button of the number pad.
			root.passwordType === NumberModel.PasswordType.SMART_PIN ? qsTr("Send Smart-eID PIN") :
			//: LABEL ALL_PLATFORMS This is the accessible text on the send/confirm button of the number pad.
			root.passwordType === NumberModel.PasswordType.NEW_SMART_PIN ? qsTr("Send new Smart-eID PIN") :
			//: LABEL ALL_PLATFORMS This is the accessible text on the send/confirm button of the number pad.
			root.passwordType === NumberModel.PasswordType.NEW_SMART_PIN_CONFIRMATION ? qsTr("Confirm new Smart-eID PIN") :
			//: LABEL ALL_PLATFORMS This is the accessible text on the send/confirm button of the number pad.
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
