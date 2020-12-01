/*
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.ChangePinModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.ReaderPlugIn 1.0


Item {
	enum WorkflowStates {
			Initial,
			Reader,
			Card,
			Update,
			Puk,
			Can,
			Pin,
			NewPin,
			Processing
	}

	QtObject {
		id: d
		readonly property int readerPlugInType: ChangePinModel.readerPlugInType
	}

	id: controller
	readonly property string currentState: ChangePinModel.currentState

	property bool showRemoveCardFeedback: false
	property int workflowState: 0

	Connections {
		target: ChangePinModel

		onFireNewContextSet: {
			pinProgressView.wasNewPin = false
			navBar.showPin(true)
			enterPinView.state = "INITIAL"
			controller.workflowState = ChangePinController.WorkflowStates.Initial
			ChangePinModel.setInitialPluginType()
		}

		onFireCurrentStateChanged: processStateChange()
		// This is necessary because onCurrentStateChanged is not
		// working, when we need to process a state a second time
	}

	function processStateChange() {
		switch (currentState) {
			case "Initial":
				break
			case "StateSelectReader":
				fireReplace(pinWorkflow)
				setPinWorkflowStateAndContinue(ChangePinController.WorkflowStates.Reader)
				break
			case "StateConnectCard":
				setPinWorkflowStateAndContinue(ChangePinController.WorkflowStates.Card)
				break
			case "StatePreparePace":
				fireReplace(pinProgressView)
				setPinWorkflowStateAndContinue(ChangePinController.WorkflowStates.Update)
				break
			case "StateEnterPacePassword":
				if (NumberModel.passwordType === NumberModel.PASSWORD_PIN) {
					setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.Pin, "PIN")
				}
				else if (NumberModel.passwordType === NumberModel.PASSWORD_CAN) {
					setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.Can, "CAN")
				}
				else if (NumberModel.passwordType === NumberModel.PASSWORD_PUK) {
					setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.Puk, "PUK")
				}
				break
			case "StateUnfortunateCardPosition":
				baseItem.firePush(cardPositionView)
				break
			case "StateEnterNewPacePin":
				setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.NewPin, "PIN_NEW")
				break
			case "StateCleanUpReaderManager":
				controller.showRemoveCardFeedback = ChangePinModel.selectedReaderHasCard() && !ChangePinModel.error;
				setPinWorkflowStateAndContinue(ChangePinController.WorkflowStates.Processing)
				break
			case "FinalState":
				if (controller.showRemoveCardFeedback) {
					controller.showRemoveCardFeedback = false
					//: INFO ANDROID IOS Hint that the ID card may be removed from the card reader since the PIN was changed successfully.
					ApplicationModel.showFeedback(qsTr("You may now remove your ID card from the device."))
				}
				if (ChangePinModel.shouldSkipResultView()) {
					ChangePinModel.continueWorkflow()
					firePopAll()
					navBar.lockedAndHidden = false
					break
				}
				baseItem.firePush(pinResult)
				navBar.lockedAndHidden = true
				break
			default:
				ChangePinModel.continueWorkflow()
		}
	}

	function setPinWorkflowStateAndContinue(pState) {
		controller.workflowState = pState
		ChangePinModel.continueWorkflow()
	}

	function setPinWorkflowStateAndRequestInput(pState, pInput) {
		controller.workflowState = pState
		if (ChangePinModel.isBasicReader) {
			enterPinView.state = pInput
			baseItem.firePush(enterPinView)
		} else {
			ChangePinModel.continueWorkflow()
		}
	}
}
