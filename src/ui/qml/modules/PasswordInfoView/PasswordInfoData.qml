/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQml
import Governikus.Animations
import Governikus.Type

QtObject {
	enum Type {
		PIN,
		SMART_PIN,
		CHOOSE_PIN,
		CHOOSE_SMART_PIN,
		TRANSPORT_PIN,
		TRANSPORT_PIN_NOT_WORKING,
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
			title: qsTr("What is the card PIN?")

			contentList: [
				PasswordInfoContentBlock {
					blockHeaderAnimation: AnimationLoader.Type.ENTER_PIN
					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("What is the card PIN?")
					paragraphList: [
						//: INFO ALL_PLATFORMS Answer to the question 'what is the card pin?'
						qsTr("The card PIN is a 6-digit PIN that you set yourself. You always need this %1PIN%2 if you want to use the %1eID function%2.").arg("<b>").arg("</b>")]
				},
				PasswordInfoContentBlock {
					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("Where can I find the card PIN?")
					paragraphList: {
						if (ApplicationModel.isSmartSupported) {
							//: INFO ALL_PLATFORMS Answer to the question 'Where can I find the card PIN?' (%1 is replaced with the application name)
							return [qsTr("You set the card PIN either directly when you picked up your ID card at the citizens' office (Bürgeramt) or later in %1 using the 5-digit Transport PIN. Only when you have set a 6-digit PIN of your own choice can you use the eID function and set up a Smart-eID.").arg(Qt.application.name)];
						}
						return [
							//: INFO ALL_PLATFORMS Answer to the question 'Where can I find the card PIN?' - Intro
							qsTr("The card PIN was %1either%2...").arg("<b>").arg("</b>"),
							//: INFO ALL_PLATFORMS Answer to the question 'Where can I find the card PIN?' - Parapgraph 1/3
							"  • %1".arg(qsTr("set by you when picking up the ID card %1at the citiziens' office%2").arg("<b>").arg("</b>")),
							//: INFO ALL_PLATFORMS Answer to the question 'Where can I find the card PIN?' - Parapgraph 2/3
							"  • %1".arg(qsTr("set %1in the %3 using%2 the 5-digit %1Transport PIN%2").arg("<b>").arg("</b>").arg(Qt.application.name)),
							//: INFO ALL_PLATFORMS Answer to the question 'Where can I find the card PIN?' - Parapgraph 3/3
							"  • %1".arg(qsTr("requested using the %1PIN Reset Service%2 - you may continue using the 6-digit card PIN from the letter or change it via \"Change PIN\" to a self chosen card PIN").arg("<b>").arg("</b>"))];
					}
				},
				PasswordInfoContentBlock {
					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("I just have a 5-digit Transport PIN")
					paragraphList: [
						//: INFO ALL_PLATFORMS Explanation if only the Transport PIN is at hand
						qsTr("You need to change the %1 5-digit Transport PIN%2 to your personal card PIN. Use %1Change PIN > Transport PIN%2 from the startpage to do so.").arg("<b>").arg("</b>")]
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
			title: qsTr("What is the Transport PIN?")

			contentList: [
				PasswordInfoContentBlock {
					blockHeaderAnimation: AnimationLoader.Type.ENTER_TRANSPORT_PIN
					//: LABEL ALL_PLATFORMS
					paragraphList: [
						//: INFO ALL_PLATFORMS Answer to the question 'What is the Transport PIN?'
						qsTr("The Transport PIN is a %1one-time PIN%2 which is required for the initial %1setup of your ID card%2. Once you replace the %1Transport PIN with a card PIN%2, it is tied to your ID card and valid for all devices.").arg("<b>").arg("</b>")]
				},
				PasswordInfoContentBlock {
					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("Where do I find the Transport PIN?")
					paragraphList: [
						//: INFO ALL_PLATFORMS Answer to the question 'Where do I find the Transport PIN?' paragraph 1/2
						qsTr("The 5-digit Transport PIN is a %1one-time PIN%2, that you reveived per %1letter%2 when applying for the ID card.").arg("<b>").arg("</b>"),
						//: INFO ALL_PLATFORMS Answer to the question 'Where do I find the Transport PIN?' paragraph 2/2
						qsTr("When setting up your ID card, you will %1replace%2 this 5-digit %1Transport PIN with%2 a 6-digit, %1self chosen card PIN%2.").arg("<b>").arg("</b>")]
				}
			]
		},
		PasswordInfoContent {
			id: transportPinNotWorkingInfo

			hint: PinResetInformationModel.pinResetHintTransportPin
			hintButtonText: PinResetInformationModel.pinResetActionText
			hintTitle: qsTr("I can't recall my PIN")

			//: LABEL ALL_PLATFORMS
			linkText: qsTr("My Transport PIN does not work")
			//: LABEL ALL_PLATFORMS
			title: qsTr("My Transport PIN does not work")

			contentList: [
				PasswordInfoContentBlock {
					//: LABEL ALL_PLATFORMS
					paragraphList: [
						//: INFO ALL_PLATFORMS Introduction why the Transport PIN might not work.
						qsTr("If your Transport PIN from the PIN letter does work, this might be caused by the following reasons:"),
						//: INFO ALL_PLATFORMS Information why the Transport PIN is void: a 6-digit PIN was already set
						"  • %1".arg(qsTr("You already set a 6-digit PIN either at the competent authority or using the %1.").arg(Qt.application.name)),
						//: INFO ALL_PLATFORMS Information why the Transport PIN is void: the PIN Reset Service was used
						"  • %1".arg(qsTr("You applied for PIN Reset Letter (letter containing a 6-digit PIN and corresponding Activation Code)."))]
				},
				PasswordInfoContentBlock {
					blockHeaderAnimation: AnimationLoader.Type.ENTER_TRANSPORT_PIN
					paragraphList: [
						//: INFO ALL_PLATFORMS Information that the Transport PIN is void once a 6-digit PIN was set.
						qsTr("Once you set a 6-digit PIN, the Transport PIN is no longer valid. You may then only use the 6-digit PIN to authenticate yourself.")]
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
			title: qsTr("Where do I find the PUK?")

			contentList: [
				PasswordInfoContentBlock {
					blockHeaderAnimation: AnimationLoader.Type.ENTER_PUK
					//: LABEL ALL_PLATFORMS
					paragraphList: [
						//: INFO ALL_PLATFORMS Answer to the question 'Where do I find the PUK?'
						qsTr("The PUK is a %1 10-digit number%2 that you can find in the %1PIN letter%2 that was sent to you by mail after you %1applied for your ID card%2. You may find it to the %1right%2 of the 5-digit %1Transport PIN%2.").arg("<b>").arg("</b>")]
				},
				PasswordInfoContentBlock {
					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("Why is the PUK required?")
					paragraphList: [
						//: INFO ALL_PLATFORMS Answer to the question 'Why is the PUK required?'
						qsTr("The PUK is required if the %1card PIN has been entered incorrectly 3 times%2 in a row. As a result, the card PIN is blocked. By entering the PUK you will %1unblock the card PIN%2 and have %1 3 more attempts%2 to enter the correct PIN.").arg("<b>").arg("</b>")]
				},
				PasswordInfoContentBlock {
					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("My PUK does not work")
					paragraphList: [
						//: INFO ALL_PLATFORMS Text block regaring "My PUK does not work"
						qsTr("Make sure, that the letter at hand is the %1letter for your current ID card%2. The PIN letter is valid only for your current ID card. With a new ID card, you receive a new PIN letter with a new PUK.").arg("<b>").arg("</b>"),
						//: INFO ALL_PLATFORMS Text block regaring "My PUK does not work"
						qsTr("If you applied for a PIN Reset letter, make sure that you don't mix up the letters. The %1PIN letter you need contains the 5-digit Transport PIN and the PUK%2. The PIN Reset letter contains the 6-digit card PIN and the activation code.").arg("<b>").arg("</b>")]
				}
			]
		},
		PasswordInfoContent {
			id: canInfo

			//: LABEL ALL_PLATFORMS
			linkText: qsTr("Why is the CAN required?")
			//: LABEL ALL_PLATFORMS
			title: qsTr("Why is the CAN required?")

			contentList: [
				PasswordInfoContentBlock {
					blockHeaderAnimation: AnimationLoader.Type.ENTER_CAN
					paragraphList: [
						//: INFO ALL_PLATFORMS Answer to the question 'When is the card access number (CAN) required?'
						qsTr("The card access number (CAN) is required when the %1card PIN has been entered incorrectly 2 times%2.").arg("<b>").arg("</b>")]
				},
				PasswordInfoContentBlock {
					blockImage: "qrc:///images/can_info.svg"
					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("Where can I find the CAN?")
					paragraphList: [
						//: INFO ALL_PLATFORMS Answer to the question 'Where can I find the CAN?'
						qsTr("The CAN is a 6-digit number that can be found on the %1bottom right%2 of the %1front of the ID card%2.").arg("<b>").arg("</b>")]
				},
				PasswordInfoContentBlock {
					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("Why do I have to enter the CAN before a 3rd attempt?")
					paragraphList: [
						//: INFO ALL_PLATFORMS Answer to the question 'Why do I have to enter the CAN before a 3rd attempt?'
						qsTr("A 3rd incorrect entry blocks your PIN and you can no longer use the eID function until it is unblocked. Requesting the CAN ensures that %1direct access to your ID card%2 is given.").arg("<b>").arg("</b>")]
				}
			]
		},
		PasswordInfoContent {
			id: canAllowedInfo

			//: LABEL ALL_PLATFORMS
			linkText: qsTr("Why is the CAN required?")
			//: LABEL ALL_PLATFORMS
			title: qsTr("Why is the CAN required?")

			contentList: [
				PasswordInfoContentBlock {
					blockHeaderAnimation: AnimationLoader.Type.ENTER_CAN

					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("Why is the CAN required?")
					paragraphList: [
						//: INFO ALL_PLATFORMS Description text of CAN-allowed authentication
						qsTr("The Card Access Number (CAN) allows to access the imprinted data of the ID card. The CAN is a 6-digit number that can be found on the front of the ID card. It is located at the bottom right next to the validity date (marked in red).")]
				}
			]
		},
		PasswordInfoContent {
			id: changePinInfo

			hint: PinResetInformationModel.pinResetHintTransportPin
			hintButtonText: PinResetInformationModel.pinResetActionText
			//: LABEL ALL_PLATFORMS
			hintTitle: qsTr("I can't recall my PIN")

			//: LABEL ALL_PLATFORMS
			linkText: qsTr("How do the PIN types differ?")
			//: LABEL ALL_PLATFORMS
			title: qsTr("How do the PIN types differ?")

			contentList: [
				PasswordInfoContentBlock {
					blockHeaderAnimation: AnimationLoader.Type.ENTER_TRANSPORT_PIN
					paragraphList: [
						//: INFO ALL_PLATFORMS Description text explaining the PINs 1/7
						qsTr("Your ID card comes with a 5-digit %1Transport PIN%2 which you need to %1replace with%2 a 6-digit %1card PIN%2 that you choose yourself.").arg("<b>").arg("</b>")]
				},
				PasswordInfoContentBlock {
					blockHeaderAnimation: AnimationLoader.Type.ENTER_PIN
					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("5-digit Transport PIN")
					paragraphList: [
						//: INFO ALL_PLATFORMS Description text explaining the PINs 2/7
						qsTr("The 5-digit Transport PIN is a %1one-time PIN%2 that was sent to you by %1mail%2 after you applied for your ID card.").arg("<b>").arg("</b>"),
						//: INFO ALL_PLATFORMS Description text explaining the PINs 3/7
						qsTr("When you set up the eID function, you will %1replace%2 this 5-digit %1Transport PIN%2 with a 6-digit %1card PIN that you choose yourself%2.").arg("<b>").arg("</b>")]
				},
				PasswordInfoContentBlock {
					//: LABEL ALL_PLATFORMS
					blockTitle: qsTr("6-digit PIN")
					paragraphList: ApplicationModel.isSmartSupported ? [

						//: INFO ALL_PLATFORMS Description text explaining the PINs 4/7
						qsTr("The 6-digit card PIN is a %1number that you choose yourself%2 when you set up the eID function for the first time. It %1replaces%2 your %1 5-digit Transport PIN%2.").arg("<b>").arg("</b>"),
						//: INFO ALL_PLATFORMS Description text explaining the PINs 5/7
						qsTr("The Smart-eID PIN also has six digits. You also choose that PIN yourself while setting up the Smart-eID for the first time."),
						//: INFO ALL_PLATFORMS Description text explaining the PINs 6/7
						qsTr("With this 6-digit PIN you prove online that the ID card or Smart-eID belongs to you. No one can use the eID function without this PIN."),
						//: INFO ALL_PLATFORMS Description text explaining the PINs (%1 is replaced with the application name) 7/7
						qsTr("You can change your card PIN and your Smart-eID PIN at any time in %1.").arg(Qt.application.name)] : [

						//: INFO ALL_PLATFORMS Description text explaining the PINs 4/7
						qsTr("The 6-digit card PIN is a %1number that you choose yourself%2 when you set up the eID function for the first time. It %1replaces%2 your %1 5-digit Transport PIN%2.").arg("<b>").arg("</b>"),
						//: INFO ALL_PLATFORMS Description text explaining the PINs 6/7
						qsTr("With this 6-digit PIN you prove online that the ID card belongs to you. %1No one can use the eID function without this PIN%2.").arg("<b>").arg("</b>"),
						//: INFO ALL_PLATFORMS Description text explaining the PINs (%1 is replaced with the application name) 7/7
						qsTr("If you applied for a %1PIN Reset letter%2, a new card PIN was already set for you. You can change your card PIN at %1any time in %3%2.").arg("<b>").arg("</b>").arg(Qt.application.name)]
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
		case PasswordInfoData.Type.TRANSPORT_PIN_NOT_WORKING:
			return transportPinNotWorkingInfo;
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
		case NumberModel.PasswordType.PIN:
			return PasswordInfoData.Type.PIN;
		case NumberModel.PasswordType.SMART_PIN:
			return PasswordInfoData.Type.SMART_PIN;
		case NumberModel.PasswordType.CAN:
			return pIsCanAllowedMode ? PasswordInfoData.Type.CAN_ALLOWED : PasswordInfoData.Type.CAN;
		case NumberModel.PasswordType.PUK:
			return PasswordInfoData.Type.PUK;
		case NumberModel.PasswordType.NEW_PIN:
			return PasswordInfoData.Type.CHOOSE_PIN;
		case NumberModel.PasswordType.NEW_SMART_PIN:
			return PasswordInfoData.Type.CHOOSE_SMART_PIN;
		case NumberModel.PasswordType.NEW_PIN_CONFIRMATION:
		case NumberModel.PasswordType.NEW_SMART_PIN_CONFIRMATION:
			return PasswordInfoData.Type.EMPTY;
		case NumberModel.PasswordType.TRANSPORT_PIN:
			return PasswordInfoData.Type.TRANSPORT_PIN;
		case NumberModel.PasswordType.SMART_BLOCKING_CODE:
			return PasswordInfoData.Type.SMART_BLOCKING_CODE;
		default:
			return PasswordInfoData.Type.PIN;
		}
	}
}
