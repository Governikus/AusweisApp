/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQml.Models 2.15

import Governikus.CheckResultView 1.0
import Governikus.Global 1.0
import Governikus.ResultView 1.0
import Governikus.Type.CheckIDCardModel 1.0

CheckResultView {
	id: root

	signal restartCheck()
	signal startAuth()

	model: resultModel
	resultType: result === CheckIDCardModel.SUCCESS ? ResultView.Type.IsSuccess : ResultView.Type.IsError
	//: LABEL ANDROID IOS
	title: qsTr("Check device and ID card")
	//: LABEL ANDROID IOS
	header: qsTr("Test Result")
	//: LABEL ANDROID IOS
	buttonText: {
		switch (result) {
			case CheckIDCardModel.SUCCESS:
				//: LABEL ANDROID IOS
				return qsTr("Continue")
			case CheckIDCardModel.PIN_DEACTIVATED:
			case CheckIDCardModel.PIN_SUSPENDED:
			case CheckIDCardModel.PIN_BLOCKED:
				//: LABEL ANDROID IOS
				return qsTr("How to proceed?")
			default:
				//: LABEL ANDROID IOS
				return qsTr("What does that mean?")
		}
	}
	buttonIcon: result === CheckIDCardModel.SUCCESS ? "qrc:///images/identify.svg" : "qrc:///images/material_help.svg"

	onContinueClicked: {
		if (result === CheckIDCardModel.SUCCESS) {
			root.startAuth()
		} else {
			push(checkIDCardSuggestionView, { result: root.result } )
		}
	}

	GText {
		visible: text !== ""
		width: parent.width

		font.bold: true
		text: result === CheckIDCardModel.SUCCESS ? qsTr("You may now try the function: \"See my personal data\". Press the Continue button to do so now.") : ""
	}

	Component {
		id: checkIDCardSuggestionView

		CheckIDCardSuggestionView {
			onRestartCheck: root.restartCheck()
			onStartAuth: root.startAuth()
			onCancelClicked: root.cancelClicked()
		}
	}

	ObjectModel {
		id: resultModel

		ResultEntry {
			readonly property bool nfcSupported: result !== CheckIDCardModel.NO_NFC

			//: LABEL ANDROID IOS
			text: nfcSupported ?
				//: LABEL ANDROID IOS
				qsTr("NFC supported") :
				//: LABEL ANDROID IOS
				qsTr("Your device doesn't support NFC")
			resultType: nfcSupported ? ResultView.Type.IsSuccess : ResultView.Type.IsError
		}

		ResultEntry {
			visible: result > CheckIDCardModel.NO_NFC

			//: LABEL ANDROID IOS
			text: qsTr("NFC enabled")
		}

		ResultEntry {
			visible: result === CheckIDCardModel.UNKNOWN_CARD_DETECTED

			//: LABEL ANDROID IOS
			text: qsTr("No supported card detected")
			resultType: ResultView.Type.IsInfo
		}

		ResultEntry {
			visible: result >= CheckIDCardModel.ID_CARD_DETECTED

			//: LABEL ANDROID IOS
			text: qsTr("ID card detected")
		}

		ResultEntry {
			readonly property bool insufficientApduLength: result === CheckIDCardModel.INSUFFICIENT_APDU_LENGTH

			visible: result >= CheckIDCardModel.INSUFFICIENT_APDU_LENGTH && Constants.is_layout_android

			//: LABEL ANDROID IOS
			text: insufficientApduLength ?
				//: LABEL ANDROID IOS
				qsTr("Extended length not supported") :
				//: LABEL ANDROID IOS
				qsTr("Extended length supported")
			resultType: insufficientApduLength ? ResultView.Type.IsError : ResultView.Type.IsSuccess
		}

		ResultEntry {
			readonly property bool cardAccessFailed: result === CheckIDCardModel.CARD_ACCESS_FAILED

			visible: result >= CheckIDCardModel.CARD_ACCESS_FAILED

			text: cardAccessFailed ?
				//: LABEL ANDROID IOS
				qsTr("ID card access failed") :
				//: LABEL ANDROID IOS
				qsTr("ID card access successful")
			resultType: cardAccessFailed ? ResultView.Type.IsError : ResultView.Type.IsSuccess
		}

		ResultEntry {
			readonly property bool pinDeactivated: result === CheckIDCardModel.PIN_DEACTIVATED

			visible: result >= CheckIDCardModel.PIN_DEACTIVATED

			text: pinDeactivated ?
				//: LABEL ANDROID IOS
				qsTr("Online identification feature disabled") :
				//: LABEL ANDROID IOS
				qsTr("Online identification feature enabled")
			resultType: pinDeactivated ? ResultView.Type.IsError : ResultView.Type.IsSuccess
		}

		ResultEntry {
			visible: result === CheckIDCardModel.PIN_SUSPENDED

			//: LABEL ANDROID IOS
			text: qsTr("ID card PIN suspended")
			resultType: ResultView.Type.IsInfo
		}

		ResultEntry {
			visible: result === CheckIDCardModel.PIN_BLOCKED

			//: LABEL ANDROID IOS
			text: qsTr("ID card PIN blocked")
			resultType: ResultView.Type.IsInfo
		}

		ResultEntry {
			visible: result === CheckIDCardModel.SUCCESS

			//: LABEL ANDROID IOS
			text: qsTr("ID card functional")
		}
	}
}
