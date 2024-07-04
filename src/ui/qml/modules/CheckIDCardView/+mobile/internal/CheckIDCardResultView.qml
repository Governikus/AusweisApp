/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQml.Models
import Governikus.CheckResultView
import Governikus.Global
import Governikus.ResultView
import Governikus.Style
import Governikus.Type

CheckResultView {
	id: root

	signal restartCheck
	signal startAuth

	buttonIcon: result === CheckIDCardModel.Result.SUCCESS ? "qrc:///images/mobile/mydata_button.svg" : "qrc:///images/mobile/help.svg"
	//: LABEL ANDROID IOS
	buttonText: {
		switch (result) {
		case CheckIDCardModel.Result.SUCCESS:
			//: LABEL ANDROID IOS
			return qsTr("Continue");
		case CheckIDCardModel.Result.PIN_DEACTIVATED:
		case CheckIDCardModel.Result.PIN_SUSPENDED:
		case CheckIDCardModel.Result.PIN_BLOCKED:
			//: LABEL ANDROID IOS
			return qsTr("How to proceed?");
		default:
			//: LABEL ANDROID IOS
			return qsTr("What does that mean?");
		}
	}
	//: LABEL ANDROID IOS
	header: qsTr("Test Result")
	icon: result === CheckIDCardModel.Result.SUCCESS ? "qrc:///images/mobile/workflow_success_nfc_%1.svg".arg(Style.currentTheme.name) : "qrc:///images/workflow_error_nfc_%1.svg".arg(Style.currentTheme.name)
	model: resultModel
	//: LABEL ANDROID IOS
	title: qsTr("Check device and ID card")

	onContinueClicked: {
		if (result === CheckIDCardModel.Result.SUCCESS) {
			root.startAuth();
		} else {
			push(checkIDCardSuggestionView, {
				"result": root.result
			});
		}
	}

	GText {
		font.bold: true
		horizontalAlignment: Text.AlignHCenter
		text: result === CheckIDCardModel.Result.SUCCESS ? qsTr("You may now try the function: \"See my personal data\". Press the Continue button to do so now.") : ""
		visible: text !== ""
		width: parent.width
	}
	Component {
		id: checkIDCardSuggestionView

		CheckIDCardSuggestionView {
			onCancelClicked: root.cancelClicked()
			onRestartCheck: root.restartCheck()
			onStartAuth: root.startAuth()
		}
	}
	ObjectModel {
		id: resultModel

		ResultEntry {
			readonly property bool nfcSupported: result !== CheckIDCardModel.Result.NO_NFC

			resultType: nfcSupported ? ResultEntry.Type.IsSuccess : ResultEntry.Type.IsError

			//: LABEL ANDROID IOS
			text: nfcSupported ?
			//: LABEL ANDROID IOS
			qsTr("NFC supported") :
			//: LABEL ANDROID IOS
			qsTr("Your device doesn't support NFC")
		}
		ResultEntry {

			//: LABEL ANDROID IOS
			text: qsTr("NFC enabled")
			visible: result > CheckIDCardModel.Result.NO_NFC
		}
		ResultEntry {
			resultType: ResultEntry.Type.IsInfo

			//: LABEL ANDROID IOS
			text: qsTr("No supported card detected")
			visible: result === CheckIDCardModel.Result.UNKNOWN_CARD_DETECTED
		}
		ResultEntry {

			//: LABEL ANDROID IOS
			text: qsTr("ID card detected")
			visible: result >= CheckIDCardModel.Result.ID_CARD_DETECTED
		}
		ResultEntry {
			readonly property bool insufficientApduLength: result === CheckIDCardModel.Result.INSUFFICIENT_APDU_LENGTH

			resultType: insufficientApduLength ? ResultEntry.Type.IsError : ResultEntry.Type.IsSuccess

			//: LABEL ANDROID IOS
			text: insufficientApduLength ?
			//: LABEL ANDROID IOS
			qsTr("Extended length not supported") :
			//: LABEL ANDROID IOS
			qsTr("Extended length supported")
			visible: result >= CheckIDCardModel.Result.INSUFFICIENT_APDU_LENGTH && Constants.is_layout_android
		}
		ResultEntry {
			readonly property bool cardAccessFailed: result === CheckIDCardModel.Result.CARD_ACCESS_FAILED

			resultType: cardAccessFailed ? ResultEntry.Type.IsError : ResultEntry.Type.IsSuccess
			text: cardAccessFailed ?
			//: LABEL ANDROID IOS
			qsTr("ID card access failed") :
			//: LABEL ANDROID IOS
			qsTr("ID card access successful")
			visible: result >= CheckIDCardModel.Result.CARD_ACCESS_FAILED
		}
		ResultEntry {
			readonly property bool pinDeactivated: result === CheckIDCardModel.Result.PIN_DEACTIVATED

			resultType: pinDeactivated ? ResultEntry.Type.IsError : ResultEntry.Type.IsSuccess
			text: pinDeactivated ?
			//: LABEL ANDROID IOS
			qsTr("eID function disabled") :
			//: LABEL ANDROID IOS
			qsTr("eID function enabled")
			visible: result >= CheckIDCardModel.Result.PIN_DEACTIVATED
		}
		ResultEntry {
			resultType: ResultEntry.Type.IsInfo

			//: LABEL ANDROID IOS
			text: qsTr("ID card PIN suspended")
			visible: result === CheckIDCardModel.Result.PIN_SUSPENDED
		}
		ResultEntry {
			resultType: ResultEntry.Type.IsInfo

			//: LABEL ANDROID IOS
			text: qsTr("ID card PIN blocked")
			visible: result === CheckIDCardModel.Result.PIN_BLOCKED
		}
		ResultEntry {

			//: LABEL ANDROID IOS
			text: qsTr("ID card functional")
			visible: result === CheckIDCardModel.Result.SUCCESS
		}
	}
}
