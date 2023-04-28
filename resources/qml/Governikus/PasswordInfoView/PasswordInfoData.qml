/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Type.PasswordType 1.0
import Governikus.Type.PinResetInformationModel 1.0
import Governikus.Type.WorkflowModel 1.0

Item {
	id: root

	readonly property url buttonLink: infoContent.buttonLink
	readonly property string buttonText: infoContent.buttonText
	readonly property var contentList: infoContent.contentList
	property int contentType
	readonly property string hint: infoContent.hint
	readonly property int imageType: infoContent.imageType
	readonly property PasswordInfoContent infoContent: {
		switch (contentType) {
		case PasswordInfoContent.Type.PIN:
			return pinInfo;
		case PasswordInfoContent.Type.CHOOSE_PIN:
			return choosePinInfo;
		case PasswordInfoContent.Type.TRANSPORT_PIN:
			return transportPinInfo;
		case PasswordInfoContent.Type.SMARTPHONE_AS_CARD_READER:
			return smartphoneAsCardReaderInfo;
		case PasswordInfoContent.Type.PUK:
			return pukInfo;
		case PasswordInfoContent.Type.CAN:
			return canInfo;
		case PasswordInfoContent.Type.CAN_ALLOWED:
			return canAllowedInfo;
		case PasswordInfoContent.Type.CHANGE_PIN:
			return changePinInfo;
		case PasswordInfoContent.Type.SMART_BLOCKING_CODE:
			return smartBlockingCodeInfo;
		case PasswordInfoContent.Type.NO_PIN:
			return noPin;
		default:
			return pinInfo;
		}
	}
	readonly property string linkText: infoContent.linkText
	readonly property string title: infoContent.title

	function fromPasswordType(pPasswordType, pIsCanAllowedMode = false) {
		switch (pPasswordType) {
		case PasswordType.PIN:
			return PasswordInfoContent.Type.PIN;
		case PasswordType.CAN:
			return pIsCanAllowedMode ? PasswordInfoContent.Type.CAN_ALLOWED : PasswordInfoContent.Type.CAN;
		case PasswordType.PUK:
			return PasswordInfoContent.Type.PUK;
		case PasswordType.NEW_PIN:
			return PasswordInfoContent.Type.CHOOSE_PIN;
		case PasswordType.TRANSPORT_PIN:
			return PasswordInfoContent.Type.TRANSPORT_PIN;
		default:
			return PasswordInfoContent.Type.PIN;
		}
	}

	PasswordInfoContent {
		id: pinInfo
		//: LABEL ALL_PLATFORMS Hint text for PIN but it is unknown.
		hint: qsTr("If you have forgotten your card PIN, you can request a new PIN free of charge using the PIN Reset Service.")

		//: LABEL ALL_PLATFORMS
		linkText: qsTr("What is the card PIN?")
		//: LABEL ALL_PLATFORMS
		title: qsTr("PIN information")

		contentList: [
			PasswordInfoContentBlock {
				//: LABEL ALL_PLATFORMS
				blockTitle: qsTr("What is the card PIN?")
				paragraphList: [
					//: INFO ALL_PLATFORMS Answer to the question 'what is the card pin?'
					qsTr("The card PIN is a six-digit PIN that you set yourself. You always need this PIN if you want to use the eID function.")]
			},
			PasswordInfoContentBlock {
				//: LABEL ALL_PLATFORMS
				blockTitle: qsTr("Where can I find the card PIN?")
				paragraphList: [
					//: INFO ALL_PLATFORMS Answer to the question 'Where can I find the card PIN?'
					qsTr("You set the card PIN either directly when you picked up your ID card at the citizens' office (Bürgeramt) or later in AusweisApp2 using the five-digit Transport PIN. Only when you have set a six-digit PIN of your own choice can you use the eID function.")]
			}
		]
	}
	PasswordInfoContent {
		id: choosePinInfo

		//: LABEL ALL_PLATFORMS
		linkText: qsTr("How do I choose a secure PIN?")

		//: LABEL ALL_PLATFORMS
		title: qsTr("PIN information")

		contentList: [
			PasswordInfoContentBlock {
				//: LABEL ALL_PLATFORMS
				blockTitle: qsTr("How do I choose a secure PIN?")
				paragraphList: [
					//: INFO ALL_PLATFORMS Answer to the question 'How do I choose a secure PIN?' paragraph 1/3
					qsTr("For your six-digit PIN, choose a combination of numbers that cannot be guessed - i.e. neither \"123456\", nor your date of birth, nor any other numbers printed on your ID card."),
					//: INFO ALL_PLATFORMS Answer to the question 'How do I choose a secure PIN?' paragraph 2/3
					qsTr("You can change your six-digit PIN at any time and an unlimited number of times as long as you know your valid PIN."),
					//: INFO ALL_PLATFORMS Answer to the question 'How do I choose a secure PIN?' paragraph 3/3
					qsTr("Keep your PIN secret and change it if another person becomes aware of it.")]
			}
		]
	}
	PasswordInfoContent {
		id: transportPinInfo
		//: LABEL ALL_PLATFORMS Hint text for requested Transport PIN but both, Transport PIN and PIN, are not known.
		hint: qsTr("If you do not know either your Transport PIN or your card PIN, you can request a new PIN free of charge using the PIN Reset Service.")
		imageType: PasswordInfoImage.LETTER

		//: LABEL ALL_PLATFORMS
		linkText: qsTr("What is the Transport PIN?")
		//: LABEL ALL_PLATFORMS
		title: qsTr("Transport PIN information")

		contentList: [
			PasswordInfoContentBlock {
				//: LABEL ALL_PLATFORMS
				blockTitle: qsTr("What is the Transport PIN?")
				paragraphList: [
					//: INFO ALL_PLATFORMS Answer to the question 'What is the Transport PIN?' paragraph 1/3
					qsTr("The five-digit Transport PIN was sent to you in the PIN letter by mail after you applied for your ID card."),
					//: INFO ALL_PLATFORMS Answer to the question 'What is the Transport PIN?' paragraph 2/3
					qsTr("If you did not set a self-selected six-digit card PIN when you picked up your ID card, you can do so using the Transport PIN."),
					//: INFO ALL_PLATFORMS Answer to the question 'What is the Transport PIN?' paragraph 3/3
					qsTr("Once you have set a card PIN, the Transport PIN loses its validity.")]
			}
		]
	}
	PasswordInfoContent {
		id: smartphoneAsCardReaderInfo
		imageType: PasswordInfoImage.SMARTPHONE_AS_CARD_READER

		//: LABEL ALL_PLATFORMS
		title: qsTr("Smartphone as card reader information")

		contentList: [
			PasswordInfoContentBlock {
				//: LABEL ALL_PLATFORMS
				blockTitle: qsTr("Smartphone as card reader information")
				paragraphList: [
					//: INFO ALL_PLATFORMS Description text of SaC pairing
					qsTr("You may use your smartphone as a card reader with AusweisApp2. The smartphone needs to feature a supported NFC chipset and both devices, your smartphone and this machine, need to be connected to the same WiFi network."),
					//: INFO ALL_PLATFORMS Description text of SaC pairing
					qsTr("To use your smartphone as a card reader you'll always need to activate the remote service in the AusweisApp2 on the smartphone. For the first time you'll also need to start the pairing mode on your smartphone, select the device from the list of available devices on this machine and then enter the pairing code shown on the phone.")]
			}
		]
	}
	PasswordInfoContent {
		id: pukInfo
		hint: PinResetInformationModel.noPinAndNoPukHint
		imageType: PasswordInfoImage.LETTER_PUK

		//: LABEL ALL_PLATFORMS
		linkText: qsTr("Where do I find the PUK?")
		//: LABEL ALL_PLATFORMS
		title: qsTr("PUK information")

		contentList: [
			PasswordInfoContentBlock {
				//: LABEL ALL_PLATFORMS
				blockTitle: qsTr("Where do I find the PUK?")
				paragraphList: [
					//: INFO ALL_PLATFORMS Answer to the question 'Where do I find the PUK?'
					qsTr("The PUK is a ten-digit number that you can find in the PIN letter that was sent to you by mail after you applied for your ID card.")]
			},
			PasswordInfoContentBlock {
				//: LABEL ALL_PLATFORMS
				blockTitle: qsTr("Why is the PUK required?")
				paragraphList: [
					//: INFO ALL_PLATFORMS Answer to the question 'Why is the PUK required?'
					qsTr("The PUK is required if the card PIN has been entered incorrectly three times in a row. As a result, the card PIN is blocked.")]
			},
			PasswordInfoContentBlock {
				//: LABEL ALL_PLATFORMS
				blockTitle: qsTr("What is the PUK's purpose?")
				paragraphList: [
					//: INFO ALL_PLATFORMS Answer to the question 'What is the PUK's purpose?'
					qsTr("By entering the PUK, you unblock the card PIN and have another three chances to enter the correct PIN.")]
			}
		]
	}
	PasswordInfoContent {
		id: canInfo
		imageType: PasswordInfoImage.CAN

		//: LABEL ALL_PLATFORMS
		linkText: qsTr("Why is the CAN required?")
		//: LABEL ALL_PLATFORMS
		title: qsTr("CAN information")

		contentList: [
			PasswordInfoContentBlock {
				//: LABEL ALL_PLATFORMS
				blockTitle: qsTr("When is the card access number (CAN) required?")
				paragraphList: [
					//: INFO ALL_PLATFORMS Answer to the question 'When is the card access number (CAN) required?'
					qsTr("The card access number (CAN) is required when the card PIN has been entered incorrectly twice.")]
			},
			PasswordInfoContentBlock {
				//: LABEL ALL_PLATFORMS
				blockTitle: qsTr("Why do I have to enter the CAN before a third attempt?")
				paragraphList: [
					//: INFO ALL_PLATFORMS Answer to the question 'Why do I have to enter the CAN before a third attempt?'
					qsTr("A third incorrect entry blocks your PIN and you can no longer use the eID function until it is unblocked. Requesting the CAN ensures that a third incorrect entry can only be made with direct access to your ID card.")]
			},
			PasswordInfoContentBlock {
				//: LABEL ALL_PLATFORMS
				blockTitle: qsTr("Where can I find the CAN?")
				paragraphList: [
					//: INFO ALL_PLATFORMS Answer to the question 'Where can I find the CAN?'
					qsTr("The CAN is a six-digit number that can be found on the bottom right of the front of the ID card.")]
			}
		]
	}
	PasswordInfoContent {
		id: canAllowedInfo
		imageType: PasswordInfoImage.CAN

		//: LABEL ALL_PLATFORMS
		linkText: qsTr("Why is the CAN required?")
		//: LABEL ALL_PLATFORMS
		title: qsTr("CAN information")

		contentList: [
			PasswordInfoContentBlock {
				//: LABEL ALL_PLATFORMS
				blockTitle: qsTr("CAN information")
				paragraphList: [
					//: INFO ALL_PLATFORMS Description text of CAN-allowed authentication
					qsTr("The Card Access Number (CAN) allows to access the imprinted data of the ID card. The CAN is a six-digit number that can be found on the front of the ID card. It is located at the bottom right next to the validity date (marked in red).")]
			}
		]
	}
	PasswordInfoContent {
		id: changePinInfo

		//: LABEL ALL_PLATFORMS
		linkText: qsTr("Learn more about the two types of PIN")
		//: LABEL ALL_PLATFORMS
		title: qsTr("Types of PIN")

		contentList: [
			PasswordInfoContentBlock {
				//: LABEL ALL_PLATFORMS
				blockTitle: qsTr("Learn more about the two types of PIN")
				paragraphList: [
					//: INFO ALL_PLATFORMS Description text explaining the PINs 1/6
					qsTr("Your ID card comes with a five-digit 'Transport PIN' which you need to replace with a six-digit PIN that you choose yourself.")]
			},
			PasswordInfoContentBlock {
				blockHeaderImageType: PasswordInfoImage.Type.LETTER
				//: LABEL ALL_PLATFORMS
				blockTitle: qsTr("Five-digit Transport PIN")
				paragraphList: [
					//: INFO ALL_PLATFORMS Description text explaining the PINs 2/6
					qsTr("The five-digit Transport PIN was sent to you by post after you applied for your ID card."),
					//: INFO ALL_PLATFORMS Description text explaining the PINs 3/6
					qsTr("The PIN can only be used once. When you set up the eID function, you will replace this five-digit PIN with a six-digit PIN that you choose yourself.")]
			},
			PasswordInfoContentBlock {
				blockHeaderImageType: PasswordInfoImage.Type.PIN
				//: LABEL ALL_PLATFORMS
				blockTitle: qsTr("Six-digit PIN")
				paragraphList: [
					//: INFO ALL_PLATFORMS Description text explaining the PINs 4/6
					qsTr("This is a number that you choose yourself when you set up the eID function for the first time. It replaces your five-digit Transport PIN."),
					//: INFO ALL_PLATFORMS Description text explaining the PINs 5/6
					qsTr("This PIN allows you to prove online that the ID card belongs to you. No one can use your ID card online without this PIN."),
					//: INFO ALL_PLATFORMS Description text explaining the PINs 6/6
					qsTr("You can change your six-digit PIN at any time in AusweisApp2.")]
			}
		]
	}
	PasswordInfoContent {
		id: smartBlockingCodeInfo

		//: LABEL ANDROID IOS
		title: qsTr("Smart-eID blocking code")

		contentList: [
			PasswordInfoContentBlock {
				//: LABEL ALL_PLATFORMS
				blockTitle: qsTr("Smart-eID blocking code")
				paragraphList: [
					//: INFO ANDROID IOS Description text of Smart-eID PIN
					qsTr("Just like the physical ID card, the Smart-eID stored on your Smartphone can be blocked. This might be required if you ever lose your device."),
					//: INFO ANDROID IOS Description text of Smart-eID PIN
					qsTr("To revoke an active Smart-eID, a blocking code is required. The blocking code will be displayed after successfully creating the Smart-eID. Furthermore, it is contained in the letter you will receive after creation.")]
			}
		]
	}
	PasswordInfoContent {
		id: noPin
		//: LABEL ALL_PLATFORMS
		hint: qsTr("You can use the PIN Reset Service to request a new card PIN free of charge.")
		imageType: Constants.is_desktop ? PasswordInfoImage.NONE : PasswordInfoImage.NO_PIN

		//: LABEL ALL_PLATFORMS
		title: qsTr("No PIN known")

		contentList: [
			PasswordInfoContentBlock {
				//: LABEL ALL_PLATFORMS
				blockTitle: qsTr("You do not know your PIN?")
				paragraphList: [
					//: INFO ALL_PLATFORMS
					qsTr("You have not yet set a six-digit card PIN and cannot find the PIN letter with the Transport PIN?"),
					//: INFO ALL_PLATFORMS
					qsTr("You set a card PIN when picking up your ID card or later by yourself, but you can no longer remember it?")]
			}
		]
	}
}
