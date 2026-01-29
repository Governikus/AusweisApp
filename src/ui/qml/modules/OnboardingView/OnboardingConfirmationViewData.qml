/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQml

import Governikus.Animations
import Governikus.Style
import Governikus.Type

QtObject {
	enum Stage {
		CHANGE_PIN,
		CHECK_ID_CARD_CARD_READER,
		CHECK_ID_CARD_SAC,
		PAIR_DEVICE,
		PIN_CORRECT,
		READER_DETECTED
	}

	readonly property int animationSymbol: Symbol.Type.CHECK
	readonly property int animationType: {
		switch (stage) {
		case OnboardingConfirmationViewData.Stage.CHANGE_PIN:
		case OnboardingConfirmationViewData.Stage.PIN_CORRECT:
			return AnimationLoader.Type.CHANGEPIN_SUCCESS;
		case OnboardingConfirmationViewData.Stage.CHECK_ID_CARD_CARD_READER:
			return AnimationLoader.Type.WAIT_FOR_CARD_USB;
		case OnboardingConfirmationViewData.Stage.CHECK_ID_CARD_SAC:
			return AnimationLoader.Type.CARD_RESULT;
		case OnboardingConfirmationViewData.Stage.PAIR_DEVICE:
			return AnimationLoader.Type.SAC_CONNECTION;
		case OnboardingConfirmationViewData.Stage.READER_DETECTED:
			return AnimationLoader.Type.WAIT_FOR_READER;
		default:
			return AnimationLoader.Type.NONE;
		}
	}
	//: ALL_PLATFORMS Common part of the description of the confirmation view after the set up AusweisApp (with SaC) stage.
	readonly property string checkIdCardCommonString: qsTr("Now have your PIN ready.") + "<br><br>" +
	//: ALL_PLATFORMS Description of the confirmation view after the set up AusweisApp stage. %1 + %2 are for bold formatting, %3 is replaced with AusweisApp.
	(qsTr("If you have already %1chosen a 6-digit card PIN%2 for this ID card (either in the %3 or at the competent authority) %1or%2 have ordered a %1PIN Reset Letter%2, this will apply.") + "<br><br>" +
		//: ALL_PLATFORMS Description of the confirmation view after the set up AusweisApp with card reader stage. %1 + %2 are for bold formatting, %3 is replaced with AusweisApp.
		qsTr("Otherwise, use your %1Transport PIN%2 that you received by letter when you first applied for the ID card. You then replace this %1one-time PIN%2 with your personal card PIN in the %3.")).arg("<b>").arg("</b>").arg(Qt.application.name)
	readonly property string descriptionText: {
		switch (stage) {
		case OnboardingConfirmationViewData.Stage.CHANGE_PIN:
			if (!Style.is_layout_desktop) {
				return pinCorrectCommonString;
			}
		// fall through
		case OnboardingConfirmationViewData.Stage.PIN_CORRECT:
			{
				let numberOfPromptsAfterSetup = Style.is_layout_desktop ? (SettingsModel.autoStartAvailable && !SettingsModel.autoStartSetByAdmin ? 2 : 1) : 0;
				switch (numberOfPromptsAfterSetup) {
				case 0:
					//: MOBILE Description of the confirmation view after the Change PIN stage.
					return pinCorrectCommonString + "<br><br>" + qsTr("If you want to change your PIN, select the “Change PIN” button on the start page.");
				case 1:
					//: DESKTOP Description of the confirmation view about upcoming steps after the Change PIN stage.
					return pinCorrectCommonString + "<br><br>" + qsTr("You have now completed the main part of the setup - there is only one more query about personalized settings.");
				default:
					//: DESKTOP Description of the confirmation view about upcoming steps after the Change PIN stage.
					return pinCorrectCommonString + "<br><br>" + qsTr("You have now completed the main part of the setup - there are only two more queries about personalized settings.");
				}
			}
		case OnboardingConfirmationViewData.Stage.CHECK_ID_CARD_CARD_READER:
			//: ALL_PLATFORMS Description of the confirmation view after the set up AusweisApp stage.
			return qsTr("Leave the ID card on the card reader.") + "<br><br>" + checkIdCardCommonString;
		case OnboardingConfirmationViewData.Stage.CHECK_ID_CARD_SAC:
			return checkIdCardCommonString;
		case OnboardingConfirmationViewData.Stage.PAIR_DEVICE:
			//: ALL_PLATFORMS Description of the confirmation view after the set up AusweisApp with SaC stage.
			return qsTr("Your smartphone has been paired as a card reader and can be used to read your ID card in the next step.") + "<br><br>" +
			//: ALL_PLATFORMS Description of the confirmation view after pairing a SaC stage.
			qsTr("Once your devices have been paired, you only need to select \"Activate card reader\" in the card reader menu on your smartphone in future to use your smartphone as a card reader for this device.");
		case OnboardingConfirmationViewData.Stage.READER_DETECTED:
			//: ALL_PLATFORMS Description of the confirmation view after detecting an USB Reader stage.
			return qsTr("Your card reader has been recognized and can be used to read your ID card in the next step.");
		default:
			return "";
		}
	}
	readonly property string headerText: {
		switch (stage) {
		case OnboardingConfirmationViewData.Stage.CHANGE_PIN:
			//: ALL_PLATFORMS Headline of the confirmation view after the change PIN stage
			return qsTr("PIN has been successfully changed");
		case OnboardingConfirmationViewData.Stage.CHECK_ID_CARD_CARD_READER:
		case OnboardingConfirmationViewData.Stage.CHECK_ID_CARD_SAC:
			//: ALL_PLATFORMS Headline of the confirmation view after the set up AusweisApp stage
			return qsTr("ID card detected");
		case OnboardingConfirmationViewData.Stage.PAIR_DEVICE:
			//: ALL_PLATFORMS Headline of the confirmation view after pairing a SaC stage.
			return qsTr("Pairing successful");
		case OnboardingConfirmationViewData.Stage.PIN_CORRECT:
			//: ALL_PLATFORMS Headline of the confirmation view after pairing a SaC stage.
			return qsTr("PIN is correct");
		case OnboardingConfirmationViewData.Stage.READER_DETECTED:
			//: ALL_PLATFORMS Headline of the confirmation view after after detecting an USB Reader stage.
			return qsTr("Card reader detected");
		default:
			return "";
		}
	}
	readonly property string hintText: {
		switch (stage) {
		case OnboardingConfirmationViewData.Stage.CHECK_ID_CARD_SAC:
			if (Style.is_layout_desktop || nfcState === ApplicationModel.NfcState.UNAVAILABLE) {
				//: ALL_PLATFORMS Hint text of the confirmation view after the set up AusweisApp stage
				return qsTr("Keep the position of your ID card on your smartphone so that it can be read immediately to request the PIN.");
			}
		// fall through
		default:
			return "";
		}
	}
	readonly property int nfcState: ApplicationModel.nfcState
	//: DESKTOP Common part of the description of the confirmation view after the Change PIN stage.
	readonly property string pinCorrectCommonString: qsTr("You can now use your ID card in combination with your 6-digit card PIN to identify yourself online.")
	required property int stage
	readonly property string sublineText: {
		switch (stage) {
		case OnboardingConfirmationViewData.Stage.PAIR_DEVICE:
			if (!Style.is_layout_desktop) {
				//: MOBILE Text that replaces _step x of y_ on the pair device confirm page
				return qsTr("Card reader is ready");
			}
		// fall through
		default:
			return "";
		}
	}
}
