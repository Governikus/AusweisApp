/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQml
import Governikus.Animations
import Governikus.Type.PasswordType
import Governikus.Type.PinResetInformationModel
import Governikus.Type.WorkflowModel

QtObject {
	enum Type {
		PIN,
		SMART_PIN,
		CHOOSE_PIN,
		CHOOSE_SMART_PIN,
		TRANSPORT_PIN,
		PUK,
		CAN,
		CAN_ALLOWED,
		CHANGE_PIN,
		SMART_BLOCKING_CODE,
		NO_PIN,
		EMPTY
	}

	readonly property url buttonLink: infoContent.buttonLink
	readonly property string buttonText: infoContent.buttonText
	readonly property list<QtObject> content: [
		PasswordInfoContent {
			id: empty

			linkText: ""
		},
		PasswordInfoContent {
			id: pinInfo

			hint: PinResetInformationModel.pinResetHint
			hintButtonText: PinResetInformationModel.pinResetActionText
			//: LABEL ALL_PLATFORMS
			hintTitle: qsTr("I can't recall my PIN")

			//: LABEL ALL_PLATFORMS
			linkText: qsTr("What is the card PIN?")
			//: LABEL ALL_PLATFORMS
			title: qsTr("PIN information")

			contentList: [
				PasswordInfoContentBlock {
					blockHeaderAnimation: AnimationLoader.Type.ENTER_PIN
					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("What is the card PIN?")
					paragraphList: [
						//: INFO ALL_PLATFORMS Answer to the question 'what is the card pin?'
						qsTr("The card PIN is a 6-digit PIN that you set yourself. You always need this PIN if you want to use the eID function with your ID card.")]
				},
				PasswordInfoContentBlock {
					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("Where can I find the card PIN?")
					paragraphList: WorkflowModel.isSmartSupported ? [
						//: INFO ALL_PLATFORMS Answer to the question 'Where can I find the card PIN?' (%1 is replaced with the application name)
						qsTr("You set the card PIN either directly when you picked up your ID card at the citizens' office (Bürgeramt) or later in %1 using the 5-digit Transport PIN. Only when you have set a 6-digit PIN of your own choice can you use the eID function and set up a Smart-eID.").arg(Qt.application.name)] : [

						//: INFO ALL_PLATFORMS Answer to the question 'Where can I find the card PIN?' (%1 is replaced with the application name)
						qsTr("You set the card PIN either directly when you picked up your ID card at the citizens' office (Bürgeramt) or later in %1 using the 5-digit Transport PIN. Only when you have set a 6-digit PIN of your own choice can you use the eID function.").arg(Qt.application.name)]
				}
			]
		},
		PasswordInfoContent {
			id: smartPinInfo

			//: LABEL ALL_PLATFORMS Hint text for PIN but it is unknown.
			hint: qsTr("If you have forgotten your Smart-eID PIN, you can renew your Smart-eID and thereby set a new PIN.")
			//: LABEL ALL_PLATFORMS
			linkText: qsTr("What is the Smart-eID PIN?")
			//: LABEL ALL_PLATFORMS
			title: qsTr("PIN information")

			contentList: [
				PasswordInfoContentBlock {
					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("What is the Smart-eID PIN?")
					paragraphList: [
						//: INFO ALL_PLATFORMS Answer to the question 'what is the Smart-eID pin?'
						qsTr("The Smart-eID PIN is a 6-digit PIN that you set yourself. You always need this PIN if you want to use your Smart-eID.")]
				},
				PasswordInfoContentBlock {
					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("Where can I find the Smart-eID PIN?")
					paragraphList: [
						//: INFO ALL_PLATFORMS Answer to the question 'Where can I find the Smart-eID PIN?'
						qsTr("You have set the Smart-eID PIN while setting up the Smart-eID.")]
				}
			]
		},
		PasswordInfoContent {
			id: choosePinInfo

			//: LABEL ALL_PLATFORMS
			linkText: qsTr("How do I choose a secure PIN?")

			//: LABEL ALL_PLATFORMS
			title: qsTr("PIN information")

			contentList: [
				PasswordInfoContentBlock {
					blockHeaderAnimation: AnimationLoader.Type.ENTER_NEW_PIN
					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("How do I choose a secure PIN?")
					paragraphList: [
						//: INFO ALL_PLATFORMS Answer to the question 'How do I choose a secure PIN?' paragraph 1/3
						qsTr("For your 6-digit PIN, choose a combination of numbers that cannot be guessed - i.e. neither \"123456\", nor your date of birth, nor any other numbers printed on your ID card."),
						//: INFO ALL_PLATFORMS Answer to the question 'How do I choose a secure PIN?' paragraph 2/3
						qsTr("You can change your 6-digit PIN at any time and an unlimited number of times as long as you know your valid PIN."),
						//: INFO ALL_PLATFORMS Answer to the question 'How do I choose a secure PIN?' paragraph 3/3
						qsTr("Keep your PIN secret and change it if another person becomes aware of it.")]
				}
			]
		},
		PasswordInfoContent {
			id: chooseSmartPinInfo

			//: LABEL ALL_PLATFORMS
			linkText: qsTr("What is the Smart-eID PIN?")

			//: LABEL ALL_PLATFORMS
			title: qsTr("Set up Smart-eID")

			contentList: [
				PasswordInfoContentBlock {
					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("What is the Smart-eID PIN?")
					paragraphList: [
						//: INFO ALL_PLATFORMS Answer to the question 'what is the Smart-eID pin?'
						qsTr("The Smart-eID PIN is a 6-digit PIN that you set yourself. You always need this PIN if you want to use your Smart-eID.")]
				},
				PasswordInfoContentBlock {
					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("How do I choose a secure PIN?")
					paragraphList: [
						//: INFO ALL_PLATFORMS Answer to the question 'How do I choose a secure (Smart-eID) PIN?' paragraph 1/3
						qsTr("For your 6-digit Smart-eID PIN, choose a combination of numbers that cannot be guessed - i.e. neither \"123456\", nor your date of birth, nor any other numbers printed on your ID card."),
						//: INFO ALL_PLATFORMS Answer to the question 'How do I choose a secure (Smart-eID) PIN?' paragraph 2/3
						qsTr("You can change your 6-digit Smart-eID PIN at any time and an unlimited number of times as long as you know your valid Smart-eID PIN."),
						//: INFO ALL_PLATFORMS Answer to the question 'How do I choose a secure (Smart-eID) PIN?' paragraph 3/3
						qsTr("Keep your PIN secret and change it if another person becomes aware of it.")]
				}
			]
		},
		PasswordInfoContent {
			id: transportPinInfo

			hint: PinResetInformationModel.pinResetHintTransportPin
			hintButtonText: PinResetInformationModel.pinResetActionText
			hintTitle: qsTr("I can't recall my PIN")

			//: LABEL ALL_PLATFORMS
			linkText: qsTr("What is the Transport PIN?")
			//: LABEL ALL_PLATFORMS
			title: qsTr("Transport PIN information")

			contentList: [
				PasswordInfoContentBlock {
					blockHeaderAnimation: AnimationLoader.Type.ENTER_TRANSPORT_PIN
					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("What is the Transport PIN?")
					paragraphList: [
						//: INFO ALL_PLATFORMS Answer to the question 'What is the Transport PIN?' paragraph 1/3
						qsTr("The 5-digit Transport PIN was sent to you in the PIN letter by mail after you applied for your ID card."),
						//: INFO ALL_PLATFORMS Answer to the question 'What is the Transport PIN?' paragraph 2/3
						qsTr("If you did not set a self-selected 6-digit card PIN when you picked up your ID card, you can do so using the Transport PIN."),
						//: INFO ALL_PLATFORMS Answer to the question 'What is the Transport PIN?' paragraph 3/3
						qsTr("Once you have set a card PIN, the Transport PIN loses its validity.")]
				}
			]
		},
		PasswordInfoContent {
			id: pukInfo

			hint: PinResetInformationModel.noPinAndNoPukHint
			hintButtonText: PinResetInformationModel.pinResetActionText
			hintTitle: qsTr("I can't recall my PUK")

			//: LABEL ALL_PLATFORMS
			linkText: qsTr("Where do I find the PUK?")
			//: LABEL ALL_PLATFORMS
			title: qsTr("PUK information")

			contentList: [
				PasswordInfoContentBlock {
					blockHeaderAnimation: AnimationLoader.Type.ENTER_PUK
					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("Where do I find the PUK?")
					paragraphList: [
						//: INFO ALL_PLATFORMS Answer to the question 'Where do I find the PUK?'
						qsTr("The PUK is a 10-digit number that you can find in the PIN letter that was sent to you by mail after you applied for your ID card.")]
				},
				PasswordInfoContentBlock {
					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("Why is the PUK required?")
					paragraphList: [
						//: INFO ALL_PLATFORMS Answer to the question 'Why is the PUK required?'
						qsTr("The PUK is required if the card PIN has been entered incorrectly 3 times in a row. As a result, the card PIN is blocked.")]
				},
				PasswordInfoContentBlock {
					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("What is the PUK's purpose?")
					paragraphList: [
						//: INFO ALL_PLATFORMS Answer to the question 'What is the PUK's purpose?'
						qsTr("By entering the PUK, you unblock the card PIN and have another 3 chances to enter the correct PIN.")]
				}
			]
		},
		PasswordInfoContent {
			id: canInfo

			//: LABEL ALL_PLATFORMS
			linkText: qsTr("Why is the CAN required?")
			//: LABEL ALL_PLATFORMS
			title: qsTr("CAN information")

			contentList: [
				PasswordInfoContentBlock {
					blockHeaderAnimation: AnimationLoader.Type.ENTER_CAN
					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("When is the card access number (CAN) required?")
					paragraphList: [
						//: INFO ALL_PLATFORMS Answer to the question 'When is the card access number (CAN) required?'
						qsTr("The card access number (CAN) is required when the card PIN has been entered incorrectly 2 times.")]
				},
				PasswordInfoContentBlock {
					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("Why do I have to enter the CAN before a 3rd attempt?")
					paragraphList: [
						//: INFO ALL_PLATFORMS Answer to the question 'Why do I have to enter the CAN before a 3rd attempt?'
						qsTr("A 3rd incorrect entry blocks your PIN and you can no longer use the eID function until it is unblocked. Requesting the CAN ensures that a 3rd incorrect entry can only be made with direct access to your ID card.")]
				},
				PasswordInfoContentBlock {
					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("Where can I find the CAN?")
					paragraphList: [
						//: INFO ALL_PLATFORMS Answer to the question 'Where can I find the CAN?'
						qsTr("The CAN is a 6-digit number that can be found on the bottom right of the front of the ID card.")]
				}
			]
		},
		PasswordInfoContent {
			id: canAllowedInfo

			//: LABEL ALL_PLATFORMS
			linkText: qsTr("Why is the CAN required?")
			//: LABEL ALL_PLATFORMS
			title: qsTr("CAN information")

			contentList: [
				PasswordInfoContentBlock {
					blockHeaderAnimation: AnimationLoader.Type.ENTER_CAN

					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("CAN information")
					paragraphList: [
						//: INFO ALL_PLATFORMS Description text of CAN-allowed authentication
						qsTr("The Card Access Number (CAN) allows to access the imprinted data of the ID card. The CAN is a 6-digit number that can be found on the front of the ID card. It is located at the bottom right next to the validity date (marked in red).")]
				}
			]
		},
		PasswordInfoContent {
			id: changePinInfo

			//: LABEL ALL_PLATFORMS
			linkText: qsTr("Learn more about the 2 types of PIN")
			//: LABEL ALL_PLATFORMS
			title: qsTr("Types of PIN")

			contentList: [
				PasswordInfoContentBlock {
					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("Learn more about the 2 types of PIN")
					headline: true
					paragraphList: [
						//: INFO ALL_PLATFORMS Description text explaining the PINs 1/7
						qsTr("Your ID card comes with a 5-digit 'Transport PIN' which you need to replace with a 6-digit PIN that you choose yourself.")]
				},
				PasswordInfoContentBlock {
					blockHeaderAnimation: AnimationLoader.Type.ENTER_TRANSPORT_PIN
					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("5-digit Transport PIN")
					paragraphList: [
						//: INFO ALL_PLATFORMS Description text explaining the PINs 2/7
						qsTr("The 5-digit Transport PIN was sent to you by mail after you applied for your ID card."),
						//: INFO ALL_PLATFORMS Description text explaining the PINs 3/7
						qsTr("The PIN can only be used once. When you set up the eID function, you will replace this 5-digit Transport PIN with a 6-digit card PIN that you choose yourself.")]
				},
				PasswordInfoContentBlock {
					blockHeaderAnimation: AnimationLoader.Type.ENTER_PIN
					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("6-digit PIN")
					paragraphList: WorkflowModel.isSmartSupported ? [

						//: INFO ALL_PLATFORMS Description text explaining the PINs 4/7
						qsTr("The 6-digit card PIN is a number that you choose yourself when you set up the eID function for the first time. It replaces your 5-digit Transport PIN."),
						//: INFO ALL_PLATFORMS Description text explaining the PINs 5/7
						qsTr("The Smart-eID PIN also has six digits. You also choose that PIN yourself while setting up the Smart-eID for the first time."),
						//: INFO ALL_PLATFORMS Description text explaining the PINs 6/7
						qsTr("With this 6-digit PIN you prove online that the ID card or Smart-eID belongs to you. No one can use the eID function without this PIN."),
						//: INFO ALL_PLATFORMS Description text explaining the PINs (%1 is replaced with the application name) 7/7
						qsTr("You can change your card PIN and your Smart-eID PIN at any time in %1.").arg(Qt.application.name)] : [

						//: INFO ALL_PLATFORMS Description text explaining the PINs 4/7
						qsTr("The 6-digit card PIN is a number that you choose yourself when you set up the eID function for the first time. It replaces your 5-digit Transport PIN."),
						//: INFO ALL_PLATFORMS Description text explaining the PINs 6/7
						qsTr("With this 6-digit PIN you prove online that the ID card belongs to you. No one can use the eID function without this PIN."),
						//: INFO ALL_PLATFORMS Description text explaining the PINs (%1 is replaced with the application name) 7/7
						qsTr("You can change your card PIN at any time in %1.").arg(Qt.application.name)]
				}
			]
		},
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
		},
		PasswordInfoContent {
			id: noPin

			hint: PinResetInformationModel.pinResetHintNoPin
			hintButtonText: PinResetInformationModel.pinResetActionText
			hintTitle: qsTr("Set a new PIN")

			//: LABEL ALL_PLATFORMS
			title: qsTr("No PIN known")

			contentList: [
				PasswordInfoContentBlock {
					blockHeaderAnimation: AnimationLoader.Type.ENTER_NO_PIN

					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("You do not know your PIN?")
					paragraphList: [
						//: INFO ALL_PLATFORMS
						qsTr("You have not yet set a 6-digit card PIN and cannot find the PIN letter with the Transport PIN?"),
						//: INFO ALL_PLATFORMS
						qsTr("You set a card PIN when picking up your ID card or later by yourself, but you can no longer remember it?")]
				}
			]
		}
	]
	readonly property var contentList: infoContent.contentList
	property int contentType
	readonly property string hint: infoContent.hint
	readonly property string hintButtonText: infoContent.hintButtonText
	readonly property string hintTitle: infoContent.hintTitle
	readonly property PasswordInfoContent infoContent: {
		switch (contentType) {
		case PasswordInfoData.Type.PIN:
			return pinInfo;
		case PasswordInfoData.Type.SMART_PIN:
			return smartPinInfo;
		case PasswordInfoData.Type.CHOOSE_PIN:
			return choosePinInfo;
		case PasswordInfoData.Type.CHOOSE_SMART_PIN:
			return chooseSmartPinInfo;
		case PasswordInfoData.Type.TRANSPORT_PIN:
			return transportPinInfo;
		case PasswordInfoData.Type.PUK:
			return pukInfo;
		case PasswordInfoData.Type.CAN:
			return canInfo;
		case PasswordInfoData.Type.CAN_ALLOWED:
			return canAllowedInfo;
		case PasswordInfoData.Type.CHANGE_PIN:
			return changePinInfo;
		case PasswordInfoData.Type.SMART_BLOCKING_CODE:
			return smartBlockingCodeInfo;
		case PasswordInfoData.Type.NO_PIN:
			return noPin;
		case PasswordInfoData.Type.EMPTY:
			return empty;
		default:
			return pinInfo;
		}
	}
	readonly property string linkText: infoContent.linkText
	readonly property string title: infoContent.title

	function fromPasswordType(pPasswordType, pIsCanAllowedMode = false) {
		switch (pPasswordType) {
		case PasswordType.PIN:
			return PasswordInfoData.Type.PIN;
		case PasswordType.SMART_PIN:
			return PasswordInfoData.Type.SMART_PIN;
		case PasswordType.CAN:
			return pIsCanAllowedMode ? PasswordInfoData.Type.CAN_ALLOWED : PasswordInfoData.Type.CAN;
		case PasswordType.PUK:
			return PasswordInfoData.Type.PUK;
		case PasswordType.NEW_PIN:
			return PasswordInfoData.Type.CHOOSE_PIN;
		case PasswordType.NEW_SMART_PIN:
			return PasswordInfoData.Type.CHOOSE_SMART_PIN;
		case PasswordType.NEW_PIN_CONFIRMATION:
		case PasswordType.NEW_SMART_PIN_CONFIRMATION:
			return PasswordInfoData.Type.EMPTY;
		case PasswordType.TRANSPORT_PIN:
			return PasswordInfoData.Type.TRANSPORT_PIN;
		case PasswordType.SMART_BLOCKING_CODE:
			return PasswordInfoData.Type.SMART_BLOCKING_CODE;
		default:
			return PasswordInfoData.Type.PIN;
		}
	}
}
