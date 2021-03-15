/*
 * \copyright Copyright (c) 2020-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.ResultView 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.CheckIDCardModel 1.0
import Governikus.Type.UiModule 1.0

ResultView {
	id: root

	signal restartCheck()

	property int result

	//: LABEL ANDROID IOS
	Accessible.name: qsTr("Check device and ID card")
	//: LABEL ANDROID IOS
	Accessible.description: qsTr("This is the device and ID card check of the AusweisApp2.")

	navigationAction: NavigationAction {
		state: "cancel"
		onClicked: {
			navBar.lockedAndHidden = false
			firePopAll()
		}
	}

	resultType: result === CheckIDCardModel.SUCCESS  ? ResultView.Type.IsSuccess : ResultView.Type.IsError

	//: LABEL ANDROID IOS
	title: qsTr("Check device and ID card")
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

	button.tintIcon: true
	button.icon.source: result === CheckIDCardModel.SUCCESS ?
		"qrc:///images/identify.svg" :
		"qrc:///images/material_help.svg"

	onClicked: {
		if (result === CheckIDCardModel.SUCCESS) {
			// We need to keep a reference to the navbar since after calling firePopAll() 'navBar' becomes undefined
			let navigation = navBar
			firePopAll()
			navigation.show(UiModule.IDENTIFY)
		} else {
			firePushWithProperties(checkIDCardSuggestionView, { result: root.result } )
		}
	}

	Component {
		id: checkIDCardSuggestionView

		CheckIDCardSuggestionView {
			onRestartCheck: root.restartCheck()
		}
	}

	Component.onCompleted: navBar.lockedAndHidden = true

	customContent: ColumnLayout {
		anchors {
			left: parent.left
			right: parent.right
			top: parent.top
			topMargin: Constants.component_spacing
		}

		spacing: Constants.component_spacing

		GPane {
			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width
			Layout.alignment: Qt.AlignHCenter

			GText {
				anchors {
					left: parent.left
					right: parent.right
				}

				//: LABEL ANDROID IOS
				text: qsTr("Test Result")
				textStyle: Style.text.header_accent
				horizontalAlignment: Text.AlignHCenter
			}

			ColumnLayout {
				width: parent.width

				spacing: Constants.text_spacing

				CheckIDCardResultEntry {
					readonly property bool nfcSupported: result !== CheckIDCardModel.NO_NFC

					//: LABEL ANDROID IOS
					text: nfcSupported ?
						//: LABEL ANDROID IOS
						qsTr("NFC supported") :
						//: LABEL ANDROID IOS
						qsTr("Your device doesn't support NFC")
					resultType: nfcSupported ? ResultView.Type.IsSuccess : ResultView.Type.IsError
				}

				CheckIDCardResultEntry {
					visible: result > CheckIDCardModel.NO_NFC

					//: LABEL ANDROID IOS
					text: qsTr("NFC enabled")
				}

				CheckIDCardResultEntry {
					visible: result === CheckIDCardModel.UNKNOWN_CARD_DETECTED

					//: LABEL ANDROID IOS
					text: qsTr("No supported card detected")
					resultType: ResultView.Type.IsInfo
				}

				CheckIDCardResultEntry {
					visible: result >= CheckIDCardModel.ID_CARD_DETECTED

					//: LABEL ANDROID IOS
					text: qsTr("ID card detected")
				}

				CheckIDCardResultEntry {
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

				CheckIDCardResultEntry {
					readonly property bool cardAccessFailed: result === CheckIDCardModel.CARD_ACCESS_FAILED

					visible: result >= CheckIDCardModel.CARD_ACCESS_FAILED

					text: cardAccessFailed ?
						//: LABEL ANDROID IOS
						qsTr("ID card access failed") :
						//: LABEL ANDROID IOS
						qsTr("ID card access successful")
					resultType: cardAccessFailed ? ResultView.Type.IsError : ResultView.Type.IsSuccess
				}

				CheckIDCardResultEntry {
					readonly property bool pinDeactivated: result === CheckIDCardModel.PIN_DEACTIVATED

					visible: result >= CheckIDCardModel.PIN_DEACTIVATED

					text: pinDeactivated ?
						//: LABEL ANDROID IOS
						qsTr("Online identification feature disabled") :
						//: LABEL ANDROID IOS
						qsTr("Online identification feature enabled")
					resultType: pinDeactivated ? ResultView.Type.IsError : ResultView.Type.IsSuccess
				}

				CheckIDCardResultEntry {
					visible: result === CheckIDCardModel.PIN_SUSPENDED

					//: LABEL ANDROID IOS
					text: qsTr("ID card PIN suspended")
					resultType: ResultView.Type.IsInfo
				}

				CheckIDCardResultEntry {
					visible: result === CheckIDCardModel.PIN_BLOCKED

					//: LABEL ANDROID IOS
					text: qsTr("ID card PIN blocked")
					resultType: ResultView.Type.IsInfo
				}

				CheckIDCardResultEntry {
					visible: result === CheckIDCardModel.SUCCESS

					//: LABEL ANDROID IOS
					text: qsTr("ID card functional")
				}
			}
		}

		GText {
			visible: result === CheckIDCardModel.SUCCESS

			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width
			Layout.alignment: Qt.AlignHCenter
			Layout.topMargin: Constants.component_spacing

			//: LABEL ANDROID IOS
			text: qsTr("You may now try the function: \"See my personal data\".")
			horizontalAlignment: Text.AlignHCenter
		}
	}
}
