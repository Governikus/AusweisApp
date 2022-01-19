/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Style 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.PinResetInformationModel 1.0


SectionPage {
	id: root

	signal close()

	property int passwordType: NumberModel.passwordType
	property bool changePinInfo: false
	property bool transportPinInfo: NumberModel.requestTransportPin

	Accessible.name: qsTr("Password information")
	Accessible.description: qsTr("This is the password information section of the AusweisApp2.")

	//: LABEL ANDROID IOS
	title: changePinInfo ? qsTr("Change PIN information")
			//: LABEL ANDROID IOS
			: passwordType === NumberModel.PASSWORD_CAN ? qsTr("CAN information")
			//: LABEL ANDROID IOS
			: passwordType === NumberModel.PASSWORD_PUK ? qsTr("PUK information")
			//: LABEL ANDROID IOS
			: passwordType === NumberModel.PASSWORD_REMOTE_PIN ? qsTr("Smartphone as card reader information")
			//: LABEL ANDROID IOS
			: passwordType === NumberModel.PASSWORD_PIN && transportPinInfo ? qsTr("Transport PIN information")
			//: LABEL ANDROID IOS
			: qsTr("PIN information")

	content: ColumnLayout {
		property bool elementsFitOnScreen: implicitHeight <= root.height

		width: root.width
		height: elementsFitOnScreen ? root.height : undefined

		spacing: 0

		PasswordInfoImage {
			Layout.alignment: Qt.AlignHCenter
			Layout.topMargin: Constants.pane_padding

			passwordType: root.passwordType
		}

		GPane {
			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width
			Layout.alignment: Qt.AlignHCenter
			Layout.margins: Constants.pane_padding

			GText {
				width: parent.width

				text: {
					if (changePinInfo) {
						//: INFO ANDROID IOS
						return qsTr("Select <b>\"Six-digit PIN\"</b> to change the self-chosen PIN of your ID card.<br><br>Select <b>\"Five-digit Transport PIN\"</b> if your PIN letter is at hand (see figure) and you have not yet chosen a six-digit PIN.<br><br>Select <b>\"PIN unknown\"</b> if your PIN letter is not at hand or you cannot recall your PIN.")
					}
					if (passwordType === NumberModel.PASSWORD_CAN && NumberModel.isCanAllowedMode) {
						//: INFO ANDROID IOS Description text of CAN-allowed authentication
						return qsTr("The Card Access Number (CAN) allows to access the imprinted data of the ID card. The CAN is a six-digit number that can be found on the front of the ID card. It is located at the bottom right next to the validity date (marked in red).")
					}
					if (passwordType === NumberModel.PASSWORD_CAN && !NumberModel.isCanAllowedMode) {
						//: INFO ANDROID IOS Description text of CAN if required for third PIN attempt
						return qsTr("The Card Access Number (CAN) is required if the PIN has already been entered incorrectly twice. In order to prevent a third incorrect entry and thus the blocking of the PIN without your consent, the CAN is also requested at this point. The CAN is a six-digit number that can be found on the front of the ID card. It is located at the bottom right next to the validity date (marked in red).")
					}
					if (passwordType === NumberModel.PASSWORD_PUK) {
						//: INFO ANDROID IOS Description text of PUK
						return qsTr("The PUK is required if the PIN has been entered incorrectly three times. At this point the PIN is blocked. The PUK (marked in red) is a ten-digit number you received with the letter sent to you by your competent authority. Please note that you can only use the PUK to unblock the PIN entry.")
					}
					if (passwordType === NumberModel.PASSWORD_REMOTE_PIN) {
						//: INFO ANDROID IOS Description text of SaC pairing
						return qsTr("You may use your smartphone as a card reader with AusweisApp2. The smartphone needs to feature a supported NFC chipset and both devices, your smartphone and this machine, need to be connected to the same WiFi network.<br><br>To use your smartphone as a card reader you'll always need to activate the remote service in the AusweisApp2 on the smartphone. For the first time you'll also need to start the pairing mode on your smartphone, select the device from the list of available devices on this machine and then enter the pairing code shown on the phone.")
					}
					if (passwordType === NumberModel.PASSWORD_PIN && transportPinInfo) {
						//: INFO ANDROID IOS Description text of Transport PIN
						return qsTr("In order to use the online identification function you need a PIN. It has six digits and is chosen by you.<br><br>If you have not yet set a six-digit PIN, e.g. when picking up your ID card, use the five-digit Transport PIN for initially setting the PIN. The Transport PIN is contained in the PIN letter (see figure) you received after applying for your ID card. Note that the Transport PIN is void after setting a six-digit PIN.<br><br>If you already have a six-digit PIN only that is valid. This is also the case if you reinstalled the %1, the PIN is stored on your ID card.").arg(Qt.application.name)
					}

					//: INFO ANDROID IOS Description text of PIN
					return qsTr("The PIN is a six-digit number you set yourself and is required for every use of the online identification function. You can change it anytime and indefinitely if you know your valid PIN. For your six-digit PIN choose a combination of numbers, that is not easy to guess, neither \"123456\" nor your birth date, or any other numbers printed on the ID card.")
				}
			}
		}

		GSpacer { Layout.fillHeight: true }

		Hint {
			visible: !changePinInfo && text !== ""

			Layout.fillWidth: true
			Layout.alignment: Qt.AlignHCenter
			Layout.maximumWidth: Style.dimens.max_text_width
			Layout.leftMargin: Constants.pane_padding
			Layout.rightMargin: Constants.pane_padding
			Layout.bottomMargin: Constants.pane_padding

			text: {
				if (passwordType === NumberModel.PASSWORD_PIN)
				{
					return transportPinInfo
							? PinResetInformationModel.noPinAndNoTransportPinHint
							: PinResetInformationModel.pinForgottenHint
				}
				if (passwordType === NumberModel.PASSWORD_PUK) {
					return PinResetInformationModel.noPinAndNoPukHint
				}

				return ""
			}

			buttonText: PinResetInformationModel.pinResetActionText

			onClicked: Qt.openUrlExternally(PinResetInformationModel.pinResetUrl)
		}
	}
}
