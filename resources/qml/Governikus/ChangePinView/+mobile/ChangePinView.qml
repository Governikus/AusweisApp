/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.EnterPasswordView 1.0
import Governikus.Global 1.0
import Governikus.AuthView 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.ProgressView 1.0
import Governikus.ResultView 1.0
import Governikus.View 1.0
import Governikus.Workflow 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.ChangePinModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.PasswordType 1.0
import Governikus.Type.UiModule 1.0
import Governikus.Type.ReaderPlugIn 1.0


SectionPage {
	id: baseItem

	readonly property bool isSmartWorkflow: ChangePinModel.readerPlugInType === ReaderPlugIn.SMART

	title: changePinController.currentState === "" ?
			//: LABEL ANDROID IOS
			qsTr("Change my (Transport) PIN") :
			NumberModel.passwordType === PasswordType.TRANSPORT_PIN ?
			//: LABEL ANDROID IOS
			qsTr("Change Transport PIN") :
			//: LABEL ANDROID IOS
			qsTr("Change PIN")
	titleBarColor: isSmartWorkflow ? Style.color.accent_smart : Style.color.accent

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back
		onClicked: show(UiModule.DEFAULT)
	}

	ChangePinController {
		id: changePinController
	}

	Connections {
		target: ChangePinModel
		//: INFO ANDROID IOS The ID card has just been unblocked and the user can now continue with their PIN change.
		function onFireOnPinUnlocked() {
			ApplicationModel.showFeedback(qsTr("Your ID card PIN is unblocked. You now have three more attempts to change your PIN."))
		}
	}

	sectionPageFlickable: changePinViewContent
	ChangePinViewContent {
		id: changePinViewContent

		anchors.fill: parent

		onNoPinAvailable: {
			setLockedAndHidden()
			push(pinUnknownView)
		}
		onMoreInformationRequested: push(passwordInfoView)
	}

	Component {
		id: pinWorkflow

		GeneralWorkflow {
			controller: changePinController
			workflowModel: ChangePinModel
			workflowTitle: baseItem.title
			titleBarColor: baseItem.titleBarColor
		}
	}

	Component {
		id: pinUnknownView

		PinUnknownView {
			onClose: {
				pop()
				setLockedAndHidden(false)
			}
		}
	}

	Component {
		id: passwordInfoView

		PasswordInfoView {
			changePinInfo: true
			navigationAction: NavigationAction { action: NavigationAction.Action.Back; onClicked: pop() }
		}
	}

	Component {
		id: cardPositionView

		CardPositionView {
			title: baseItem.title

			onCancelClicked: ChangePinModel.cancelWorkflow()
			onContinueClicked: {
				pop()
				ChangePinModel.continueWorkflow()
			}
		}
	}

	Component {
		id: pinResult

		ResultView {
			title: baseItem.title
			titleBarColor: baseItem.titleBarColor
			resultType: ChangePinModel.error ? ResultView.Type.IsError : ResultView.Type.IsSuccess
			text: ChangePinModel.resultString
			hintText: ChangePinModel.statusHintText
			hintButtonText: ChangePinModel.statusHintActionText

			onCancelClicked: continueClicked()
			onContinueClicked: {
				ChangePinModel.continueWorkflow()
				setLockedAndHidden(false)
				popAll()
			}
			onHintClicked: ChangePinModel.invokeStatusHintAction()
		}
	}

	Component {
		id: enterPinView

		EnterPasswordView {
			navigationAction: NavigationAction { action: NavigationAction.Action.Cancel; onClicked: ChangePinModel.cancelWorkflow() }
			title: baseItem.title
			titleBarColor: baseItem.titleBarColor

			onPasswordEntered: {
				changePinController.wasNewPin = pWasNewPin
				pop()
				ChangePinModel.continueWorkflow()
			}
		}
	}

	Component {
		id: pinProgressView

		ProgressView {
			navigationAction: NavigationAction {
				action: ChangePinModel.isBasicReader ? NavigationAction.Action.Cancel : NavigationAction.Action.None
				onClicked: ChangePinModel.cancelWorkflow()
			}
			title: baseItem.title
			titleBarColor: baseItem.titleBarColor
			text: {
				if (isSmartWorkflow) {
					return changePinController.wasNewPin
					//: LABEL ANDROID IOS Processing screen label while the card communication is running after the new Smart-eID PIN has been entered during PIN change process.
					? qsTr("Setting new Smart-eID PIN")
					//: LABEL ANDROID IOS Processing screen label while the card communication is running before the new ID card PIN has been entered during PIN change process.
					: qsTr("Change Smart-eID PIN")
				}
				return changePinController.wasNewPin
				//: LABEL ANDROID IOS Processing screen label while the card communication is running after the new ID card PIN has been entered during PIN change process.
				? qsTr("Setting new ID card PIN")
				//: LABEL ANDROID IOS Processing screen label while the card communication is running before the new ID card PIN has been entered during PIN change process.
				: qsTr("Change ID card PIN")
			}
			subText: {
				if (!visible) {
					return ""
				}

				if (isSmartWorkflow) {
					//: INFO ANDROID IOS Generic progress message during PIN change process.
					return qsTr("Please wait a moment.")
				}

				if (ChangePinModel.isBasicReader) {
					 //: INFO ANDROID IOS Loading screen during PIN change process, data communication is currently ongoing. Message is usually not visible since the password handling with basic reader is handled by EnterPasswordView.
					return qsTr("Please do not move the ID card.")
				}

				if(NumberModel.inputError !== "") {
					return NumberModel.inputError
				}

				if (changePinController.workflowState === ChangePinController.WorkflowStates.Update
					|| changePinController.workflowState === ChangePinController.WorkflowStates.Pin
					|| changePinController.workflowState === ChangePinController.WorkflowStates.NewPin) {
					//: INFO ANDROID IOS Either an comfort card reader or smartphone-as-card-reader is used, the user needs to react to request on that device.
					return qsTr("Please observe the display of your card reader.")
				}

				if (changePinController.workflowState === ChangePinController.WorkflowStates.Can) {
					//: INFO ANDROID IOS The wrong ID card PIN was entered twice, the next attempt requires additional verification via CAN.
					return qsTr("A wrong ID card PIN has been entered twice on your ID card. For a third attempt, please first enter the six-digit Card Access Number (CAN). You can find your CAN in the bottom right on the front of your ID card.")
				}

				if (changePinController.workflowState === ChangePinController.WorkflowStates.Puk) {
					 //: INFO ANDROID IOS The ID card PIN (including the CAN) was entered wrongfully three times, the PUK is required to unlock the ID card.
					return qsTr("You have entered an incorrect, six-digit ID card PIN thrice, your ID card PIN is now blocked. To remove the block, the ten-digit PUK must be entered first.")
				}

				//: INFO ANDROID IOS Generic progress message during PIN change process.
				return qsTr("Please wait a moment.")
			}
			subTextColor: !ChangePinModel.isBasicReader && (NumberModel.inputError
															|| changePinController.workflowState === ChangePinController.WorkflowStates.Can
															|| changePinController.workflowState === ChangePinController.WorkflowStates.Puk) ? Constants.red : Style.color.secondary_text
		}
	}
}
