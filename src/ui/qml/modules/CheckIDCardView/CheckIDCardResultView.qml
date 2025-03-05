/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQml.Models

import Governikus.Animations
import Governikus.CheckResultView
import Governikus.Global
import Governikus.Type

CheckResultView {
	id: root

	property bool usedInOnboarding: false

	animationSymbol: d.success ? Symbol.Type.CHECK : Symbol.Type.ERROR
	animationType: {
		if (d.isPcsc) {
			return AnimationLoader.CARD_RESULT;
		}
		if (d.isNfc) {
			return AnimationLoader.NFC_RESULT;
		}
		if (d.isRemote) {
			return AnimationLoader.SAC_RESULT;
		}
		return AnimationLoader.NONE;
	}
	buttonIcon: {
		if (d.successNfc) {
			return "qrc:///images/mobile/mydata_button.svg";
		}
		if (d.successPcsc || d.successRemote) {
			return "qrc:///images/material_arrow_right.svg";
		}
		return "qrc:///images/help.svg";
	}
	buttonLayoutDirection: d.successPcsc || d.successRemote ? Qt.RightToLeft : Qt.LeftToRight
	buttonText: {
		switch (result) {
		case CheckIDCardModel.Result.SUCCESS:
			//: LABEL ALL_PLATFORMS
			return qsTr("Continue");
		case CheckIDCardModel.Result.PIN_DEACTIVATED:
		case CheckIDCardModel.Result.PIN_SUSPENDED:
		case CheckIDCardModel.Result.PIN_BLOCKED:
			//: LABEL ALL_PLATFORMS
			return qsTr("How to proceed?");
		default:
			//: LABEL ALL_PLATFORMS
			return qsTr("What does that mean?");
		}
	}
	//: LABEL ALL_PLATFORMS
	header: qsTr("Test Result")
	model: resultModel
	//: LABEL ALL_PLATFORMS
	title: qsTr("Check device and ID card")

	GText {
		font.weight: Font.Bold
		horizontalAlignment: Text.AlignHCenter
		//: LABEL ALL_PLATFORMS
		text: (d.successNfc && !root.usedInOnboarding) ? qsTr("You may now try the function: \"See my personal data\". Press the \"%1\" button to do so now.").arg(root.buttonText) : ""
		visible: text !== ""
	}
	ObjectModel {
		id: resultModel

		ResultEntry {
			resultType: ResultEntry.Type.IsSuccess
			text: d.isPcsc ?
			//: LABEL ALL_PLATFORMS
			qsTr("Card reader supported") :
			//: LABEL ALL_PLATFORMS
			qsTr("Smartphone as card reader detected")
			visible: !d.isNfc
		}
		ResultEntry {
			readonly property bool nfcSupported: root.result !== CheckIDCardModel.Result.NO_READER

			resultType: nfcSupported ? ResultEntry.Type.IsSuccess : ResultEntry.Type.IsError
			text: nfcSupported ?
			//: LABEL ALL_PLATFORMS
			qsTr("NFC supported") :
			//: LABEL ALL_PLATFORMS
			qsTr("Your device doesn't support NFC")
			visible: d.isNfc
		}
		ResultEntry {

			//: LABEL ALL_PLATFORMS
			text: qsTr("NFC enabled")
			visible: d.isNfc && root.result > CheckIDCardModel.Result.NO_READER
		}
		ResultEntry {
			resultType: ResultEntry.Type.IsInfo

			//: LABEL ALL_PLATFORMS
			text: qsTr("No supported card detected")
			visible: root.result === CheckIDCardModel.Result.UNKNOWN_CARD_DETECTED
		}
		ResultEntry {

			//: LABEL ALL_PLATFORMS
			text: qsTr("ID card detected")
			visible: root.result > CheckIDCardModel.Result.UNKNOWN_CARD_DETECTED
		}
		ResultEntry {
			readonly property bool insufficientApduLength: root.result === CheckIDCardModel.Result.INSUFFICIENT_APDU_LENGTH

			resultType: insufficientApduLength ? ResultEntry.Type.IsError : ResultEntry.Type.IsSuccess
			text: insufficientApduLength ?
			//: LABEL ALL_PLATFORMS
			qsTr("Extended length not supported") :
			//: LABEL ALL_PLATFORMS
			qsTr("Extended length supported")
			visible: !d.isPcsc && root.result >= CheckIDCardModel.Result.INSUFFICIENT_APDU_LENGTH
		}
		ResultEntry {
			readonly property bool cardAccessFailed: root.result === CheckIDCardModel.Result.CARD_ACCESS_FAILED

			resultType: cardAccessFailed ? ResultEntry.Type.IsError : ResultEntry.Type.IsSuccess
			text: cardAccessFailed ?
			//: LABEL ALL_PLATFORMS
			qsTr("ID card access failed") :
			//: LABEL ALL_PLATFORMS
			qsTr("ID card access successful")
			visible: root.result >= CheckIDCardModel.Result.CARD_ACCESS_FAILED
		}
		ResultEntry {
			readonly property bool pinDeactivated: root.result === CheckIDCardModel.Result.PIN_DEACTIVATED

			resultType: pinDeactivated ? ResultEntry.Type.IsError : ResultEntry.Type.IsSuccess
			text: pinDeactivated ?
			//: LABEL ALL_PLATFORMS
			qsTr("eID function disabled") :
			//: LABEL ALL_PLATFORMS
			qsTr("eID function enabled")
			visible: root.result >= CheckIDCardModel.Result.PIN_DEACTIVATED
		}
		ResultEntry {
			resultType: ResultEntry.Type.IsInfo

			//: LABEL ALL_PLATFORMS
			text: qsTr("ID card PIN suspended")
			visible: root.result === CheckIDCardModel.Result.PIN_SUSPENDED
		}
		ResultEntry {
			resultType: ResultEntry.Type.IsInfo

			//: LABEL ALL_PLATFORMS
			text: qsTr("ID card PIN blocked")
			visible: root.result === CheckIDCardModel.Result.PIN_BLOCKED
		}
		ResultEntry {

			//: LABEL ALL_PLATFORMS
			text: qsTr("ID card functional")
			visible: root.result === CheckIDCardModel.Result.SUCCESS
		}
	}
	QtObject {
		id: d

		readonly property bool isNfc: root.pluginType === ReaderManagerPluginType.ReaderManagerPluginType.NFC
		readonly property bool isPcsc: root.pluginType === ReaderManagerPluginType.ReaderManagerPluginType.PCSC
		readonly property bool isRemote: root.pluginType === ReaderManagerPluginType.ReaderManagerPluginType.REMOTE_IFD
		readonly property bool success: root.result === CheckIDCardModel.Result.SUCCESS
		readonly property bool successNfc: isNfc && success
		readonly property bool successPcsc: isPcsc && success
		readonly property bool successRemote: isRemote && success
	}
}
