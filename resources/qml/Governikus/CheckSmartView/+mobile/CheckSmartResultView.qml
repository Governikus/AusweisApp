/*
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQml.Models 2.15

import Governikus.CheckResultView 1.0
import Governikus.Global 1.0
import Governikus.ResultView 1.0
import Governikus.Style 1.0
import Governikus.Type.SmartModel 1.0

CheckResultView {
	id: root

	signal startAuth()
	signal checkDevice()
	signal runSmartSetup()

	model: resultModel
	resultType: {
		switch (result) {
			case SmartModel.SMART_UNAVAILABLE:
			case SmartModel.SMART_UNUSABLE:
				return ResultView.Type.IsError

			case SmartModel.SMART_NO_PROVISIONING:
			case SmartModel.SMART_NO_PERSONALIZATION:
			case SmartModel.SMART_UPDATING_STATUS:
				return ResultView.Type.IsInfo

			default:
				return ResultView.Type.IsSuccess
		}
	}
	//: LABEL ANDROID IOS
	title: qsTr("Check Smart-eID")
	//: LABEL ANDROID IOS
	header: qsTr("Result of Smart-eID check")
	buttonText: {
		switch (result) {
			case SmartModel.SMART_READY:
				//: LABEL ANDROID IOS
				return qsTr("Continue")

			case SmartModel.SMART_UPDATING_STATUS:
				return ""

			default:
				//: LABEL ANDROID IOS
				return qsTr("What does that mean?")
		}
	}
	buttonIcon: result === SmartModel.SMART_READY ? "qrc:///images/identify.svg" : "qrc:///images/material_help.svg"
	titleBarColor: Style.color.accent_smart

	onContinueClicked: {
		if (result === SmartModel.SMART_READY) {
			root.startAuth()
		} else {
			push(checkSmartSuggestionView, { result: root.result } )
		}
	}

	GText {
		visible: text !== ""
		width: parent.width

		font.bold: true
		text: result === SmartModel.SMART_READY ? qsTr("You may now try the function: \"See my personal data\". Press the Continue button to do so now.") : ""
	}

	Component {
		id: checkSmartSuggestionView

		CheckSmartSuggestionView {
			onCheckDevice: root.checkDevice()
			onRunSmartSetup: root.runSmartSetup()
			onCancelClicked: root.cancelClicked()
		}
	}

	ObjectModel {
		id: resultModel

		ResultEntry {
			text: resultType === ResultView.Type.IsSuccess
				  //: LABEL ANDROID IOS
				  ? qsTr("Supported")
				  //: LABEL ANDROID IOS
				  : qsTr("Not supported")
			resultType: result !== SmartModel.SMART_UNAVAILABLE ? ResultView.Type.IsSuccess : ResultView.Type.IsError
		}

		ResultEntry {
			visible: result >= SmartModel.SMART_NO_PROVISIONING

			text: resultType === ResultView.Type.IsSuccess
				  //: LABEL ANDROID IOS
				  ? qsTr("Prepared")
				  //: LABEL ANDROID IOS
				  : qsTr("Not prepared")
			resultType: result === SmartModel.SMART_NO_PROVISIONING ? ResultView.Type.IsError : ResultView.Type.IsSuccess
		}

		ResultEntry {
			visible: result >= SmartModel.SMART_NO_PERSONALIZATION

			text: resultType === ResultView.Type.IsSuccess
				  //: LABEL ANDROID IOS
				  ? qsTr("Set up")
				  //: LABEL ANDROID IOS
				  : qsTr("Not set up")
			resultType: result === SmartModel.SMART_NO_PERSONALIZATION ? ResultView.Type.IsError : ResultView.Type.IsSuccess
		}

		ResultEntry {
			visible: result === SmartModel.SMART_UNUSABLE

			//: LABEL ANDROID IOS
			text: qsTr("Invalid")
			resultType: ResultView.Type.IsError
		}

		ResultEntry {
			visible: result === SmartModel.SMART_READY

			//: LABEL ANDROID IOS
			text: qsTr("Ready for use")
		}
	}
}
