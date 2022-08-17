/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Style 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.PasswordType 1.0
import Governikus.Type.PinResetInformationModel 1.0


SectionPage {
	id: root

	signal close()

	property int passwordType: NumberModel.passwordType
	property bool changePinInfo: false

	//: LABEL ANDROID IOS
	title: changePinInfo ? qsTr("Change PIN information")
			//: LABEL ANDROID IOS
			: passwordType === PasswordType.PASSWORD_CAN ? qsTr("CAN information")
			//: LABEL ANDROID IOS
			: passwordType === PasswordType.PUK ? qsTr("PUK information")
			//: LABEL ANDROID IOS
			: passwordType === PasswordType.REMOTE_PIN ? qsTr("Smartphone as card reader information")
			//: LABEL ANDROID IOS
			: passwordType === PasswordType.TRANSPORT_PIN ? qsTr("Transport PIN information")
			//: LABEL ANDROID IOS
			: passwordType === PasswordType.SMART_BLOCKING_CODE ? qsTr("Smart-eID blocking code")
			//: LABEL ANDROID IOS
			: qsTr("PIN information")

	sectionPageFlickable: contentItem
	GFlickableColumnLayout {
		id: contentItem

		anchors.fill: parent

		maximumContentWidth: Style.dimens.max_text_width
		spacing: Constants.component_spacing
		fillHeight: false

		PasswordInfoImage {
			Layout.alignment: Qt.AlignHCenter
			Layout.topMargin: Constants.pane_padding

			passwordType: root.passwordType
		}

		GPane {
			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width
			Layout.alignment: Qt.AlignHCenter

			GText {
				width: parent.width

				text: {
					if (changePinInfo) {
						//: INFO ANDROID IOS
						return qsTr("Select <b>\"Six-digit PIN\"</b> to change the self-chosen PIN of your ID card or the Smart-eID.<br><br>Select <b>\"Five-digit Transport PIN\"</b> if your PIN letter is at hand (see figure) and you have not yet chosen a six-digit ID card PIN.<br><br>Select <b>\"PIN unknown\"</b> if your PIN letter is not at hand or you cannot recall your PIN.")
					}
					if (passwordType === PasswordType.CAN && NumberModel.isCanAllowedMode) {
						//: INFO ANDROID IOS Description text of CAN-allowed authentication
						return qsTr("The Card Access Number (CAN) allows to access the imprinted data of the ID card. The CAN is a six-digit number that can be found on the front of the ID card. It is located at the bottom right next to the validity date (marked in red).")
					}
					if (passwordType === PasswordType.CAN && !NumberModel.isCanAllowedMode) {
						//: INFO ANDROID IOS Description text of CAN if required for third ID card PIN attempt
						return qsTr("The Card Access Number (CAN) is required if the ID card PIN has already been entered incorrectly twice. In order to prevent a third incorrect entry and thus the blocking of the PIN without your consent, the CAN is also requested at this point. The CAN is a six-digit number that can be found on the front of the ID card. It is located at the bottom right next to the validity date (marked in red).")
					}
					if (passwordType === PasswordType.PUK) {
						//: INFO ANDROID IOS Description text of PUK
						return qsTr("The PUK is required if the ID card PIN has been entered incorrectly three times. At this point the ID card PIN is blocked. The PUK (marked in red) is a ten-digit number you received with the letter sent to you by your competent authority. Please note that you can only use the PUK to unblock the PIN entry.")
					}
					if (passwordType === PasswordType.REMOTE_PIN) {
						//: INFO ANDROID IOS Description text of SaC pairing
						return qsTr("You may use your smartphone as a card reader with AusweisApp2. The smartphone needs to feature a supported NFC chipset and both devices, your smartphone and this machine, need to be connected to the same WiFi network.<br><br>To use your smartphone as a card reader you'll always need to activate the remote service in the AusweisApp2 on the smartphone. For the first time you'll also need to start the pairing mode on your smartphone, select the device from the list of available devices on this machine and then enter the pairing code shown on the phone.")
					}
					if (passwordType === PasswordType.TRANSPORT_PIN) {
						//: INFO ANDROID IOS Description text of Transport PIN
						return qsTr("In order to use the online identification function you need an ID card PIN. It has six digits and is chosen by you.<br><br>If you have not yet set a six-digit ID card PIN, e.g. when picking up your ID card, use the five-digit Transport PIN for initially setting the ID card PIN. The Transport PIN is contained in the PIN letter (see figure) you received after applying for your ID card. Note that the Transport PIN is void after setting a six-digit ID card PIN.<br><br>If you already have a six-digit PIN only that is valid. This is also the case if you reinstalled the %1, the ID card PIN is stored on your ID card.").arg(Qt.application.name)
					}
					if (passwordType === PasswordType.SMART_BLOCKING_CODE) {
						//: INFO ANDROID IOS Description text of Transport PIN
						return qsTr("Just like the physical ID card, the Smart-eID stored on your Smartphone can be blocked. This might be required if you ever lose your device.<br><br>To revoke an active Smart-eID, a blocking code is required. The blocking code will be displayed after successfully creating the Smart-eID. Furthermore, it is contained in the letter you will receive after creation.")
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

			text: {
				if (passwordType === PasswordType.PIN)
				{
					return PinResetInformationModel.pinForgottenHint
				}
				if (passwordType === PasswordType.TRANSPORT_PIN)
				{
					return PinResetInformationModel.noPinAndNoTransportPinHint
				}
				if (passwordType === PasswordType.PUK) {
					return PinResetInformationModel.noPinAndNoPukHint
				}

				return ""
			}

			buttonText: PinResetInformationModel.pinResetActionText

			onClicked: Qt.openUrlExternally(PinResetInformationModel.pinResetUrl)
		}
	}
}
