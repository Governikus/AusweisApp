/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.PinResetInformationModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.PasswordType 1.0
import Governikus.Type.WorkflowModel 1.0

SectionPage {
	id: root

	property bool changePinInfo: false
	property int passwordType: NumberModel.passwordType
	property alias rootEnabled: titleBarAction.rootEnabled

	signal close

	titleBarAction: TitleBarAction {
		id: titleBarAction
		rootEnabled: ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_NONE
		showHelp: false
		showSettings: false
		text: headline.text
	}

	Keys.onEnterPressed: close()
	Keys.onEscapePressed: close()
	Keys.onReturnPressed: close()

	GFlickableColumnLayout {
		anchors.fill: parent
		anchors.margins: Constants.pane_padding
		spacing: Constants.groupbox_spacing

		GText {
			id: headline
			Layout.alignment: Qt.AlignHCenter
			Layout.fillWidth: true
			Layout.maximumWidth: rowLayout.width
			activeFocusOnTab: true
			horizontalAlignment: Text.AlignLeft
			//: LABEL DESKTOP
			text: changePinInfo ? qsTr("Change PIN information") :
			//: LABEL DESKTOP
			passwordType === PasswordType.CAN ? qsTr("CAN information") :
			//: LABEL DESKTOP
			passwordType === PasswordType.PUK ? qsTr("PUK information") :
			//: LABEL DESKTOP
			passwordType === PasswordType.REMOTE_PIN ? qsTr("Smartphone as card reader information") :
			//: LABEL DESKTOP
			passwordType === PasswordType.TRANSPORT_PIN ? qsTr("Transport PIN information") :
			//: LABEL DESKTOP
			qsTr("PIN information")
			textStyle: Style.text.header
			wrapMode: Text.WordWrap

			FocusFrame {
			}
		}
		GridLayout {
			id: rowLayout
			Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
			Layout.fillWidth: true
			columns: 2
			rowSpacing: Constants.groupbox_spacing

			GText {
				id: mainText
				Layout.alignment: Qt.AlignTop
				Layout.fillWidth: true
				Layout.maximumWidth: Style.dimens.max_text_width
				activeFocusOnTab: true
				horizontalAlignment: Text.AlignJustify
				text: {
					if (changePinInfo) {
						if (WorkflowModel.isSmartSupported) {
							//: INFO DESKTOP Description text of change PIN selection options
							return qsTr("Select <b>\"Six-digit PIN\"</b> to change the self-chosen PIN of your ID card or the Smart-eID.<br><br>Select <b>\"Five-digit Transport PIN\"</b> if your PIN letter is at hand (see figure) and you have not yet chosen a six-digit ID card PIN.<br><br>Select <b>\"PIN unknown\"</b> if your PIN letter is not at hand or you cannot recall your PIN.");
						}
						//: INFO DESKTOP Description text of change PIN selection options
						return qsTr("Select <b>\"Six-digit PIN\"</b> to change the self-chosen PIN of your ID card.<br><br>Select <b>\"Five-digit Transport PIN\"</b> if your PIN letter is at hand (see figure) and you have not yet chosen a six-digit ID card PIN.<br><br>Select <b>\"PIN unknown\"</b> if your PIN letter is not at hand or you cannot recall your PIN.");
					}
					if (passwordType === PasswordType.CAN && NumberModel.isCanAllowedMode) {
						//: INFO DESKTOP Description text of CAN-allowed authentication
						return qsTr("The Card Access Number (CAN) allows to access the imprinted data of the ID card. The CAN is a six-digit number that can be found on the front of the ID card. It is located at the bottom right next to the validity date (marked in red).");
					}
					if (passwordType === PasswordType.CAN && !NumberModel.isCanAllowedMode) {
						//: INFO DESKTOP Description text of CAN if required for third ID card PIN attempt
						return qsTr("The Card Access Number (CAN) is required if the ID card PIN has already been entered incorrectly twice. In order to prevent a third incorrect entry and thus the blocking of the PIN without your consent, the CAN is also requested at this point. The CAN is a six-digit number that can be found on the front of the ID card. It is located at the bottom right next to the validity date (marked in red).");
					}
					if (passwordType === PasswordType.PUK) {
						//: INFO DESKTOP Description text of PUK
						return qsTr("The PUK is required if the ID card PIN has been entered incorrectly three times. At this point the ID card PIN is blocked. The PUK (marked in red) is a ten-digit number you received with the letter sent to you by your competent authority. Please note that you can only use the PUK to unblock the PIN entry.");
					}
					if (passwordType === PasswordType.REMOTE_PIN) {
						//: INFO DESKTOP Description text of SaC pairing
						return qsTr("You may use your smartphone as a card reader with AusweisApp2. The smartphone needs to feature a supported NFC chipset and both devices, your smartphone and this machine, need to be connected to the same WiFi network.<br><br>To use your smartphone as a card reader you'll always need to activate the remote service in the AusweisApp2 on the smartphone. For the first time you'll also need to start the pairing mode on your smartphone, select the device from the list of available devices on this machine and then enter the pairing code shown on the phone.");
					}
					if (passwordType === PasswordType.TRANSPORT_PIN) {
						//: INFO DESKTOP Description text of Transport PIN
						return qsTr("In order to use the online identification function you need an ID card PIN. It has six digits and is chosen by you.<br><br>If you have not yet set a six-digit ID card PIN, e.g. when picking up your ID card, use the five-digit Transport PIN for initially setting the ID card PIN. The Transport PIN is contained in the PIN letter (see figure) you received after applying for your ID card. Note that the Transport PIN is void after setting a six-digit ID card PIN.<br><br>If you already have a six-digit PIN only that is valid. This is also the case if you reinstalled the %1, the ID card PIN is stored on your ID card.").arg(Qt.application.name);
					}

					//: INFO DESKTOP Description text of PIN
					return qsTr("The PIN is a six-digit number you set yourself and is required for every use of the online identification function. You can change it anytime and indefinitely if you know your valid PIN. For your six-digit PIN choose a combination of numbers, that is not easy to guess, neither \"123456\" nor your birth date, or any other numbers printed on the ID card.");
				}
				textStyle: Style.text.header

				FocusFrame {
				}
			}
			PasswordInfoImage {
				passwordType: root.passwordType
				scaleFactorCan: 2 * ApplicationModel.scaleFactor
				scaleFactorGeneral: 2.7 * ApplicationModel.scaleFactor
				textStyle: Style.text.normal
				visible: passwordType !== PasswordType.PIN || passwordType === PasswordType.TRANSPORT_PIN || changePinInfo
			}
			GSpacer {
				Layout.columnSpan: 2
				Layout.fillHeight: true
				Layout.row: 1
			}
			Hint {
				Layout.alignment: Qt.AlignHCenter
				Layout.fillWidth: true
				Layout.maximumWidth: mainText.width
				Layout.row: 2
				buttonText: PinResetInformationModel.pinResetActionText
				text: {
					if (passwordType === PasswordType.TRANSPORT_PIN) {
						return PinResetInformationModel.noPinAndNoTransportPinHint;
					}
					if (passwordType === PasswordType.PIN) {
						return PinResetInformationModel.pinForgottenHint;
					}
					if (passwordType === PasswordType.PUK) {
						return PinResetInformationModel.noPinAndNoPukHint;
					}
					return "";
				}
				visible: !changePinInfo && text !== ""

				onClicked: Qt.openUrlExternally(PinResetInformationModel.pinResetUrl)
			}
		}
		NavigationButton {
			id: button
			Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
			buttonType: NavigationButton.Type.Back

			onClicked: root.close()
		}
	}
}
