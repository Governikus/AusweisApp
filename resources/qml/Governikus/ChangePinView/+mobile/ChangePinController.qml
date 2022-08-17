/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.View 1.0
import Governikus.Type.ChangePinModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.PasswordType 1.0
import Governikus.Type.UiModule 1.0


Controller {
	enum WorkflowStates {
			Initial,
			Reader,
			Update,
			Puk,
			Can,
			TransportPin,
			Pin,
			NewPin,
			Processing
	}

	id: controller
	readonly property string currentState: ChangePinModel.currentState
	property bool wasNewPin: false

	property int workflowState: 0

	Connections {
		target: ChangePinModel

		function onFireNewContextSet() {
			controller.wasNewPin = false
			show(UiModule.PINMANAGEMENT, true)
			controller.workflowState = ChangePinController.WorkflowStates.Initial
			ChangePinModel.setInitialPluginType()
		}

		function onFireCurrentStateChanged() { processStateChange() }
		// This is necessary because onCurrentStateChanged is not
		// working, when we need to process a state a second time
	}

	function processStateChange() {
		switch (currentState) {
			case "Initial":
				break
			case "StateSelectReader":
				if (!workflowActive) {
					replace(pinWorkflow)
				}
				setPinWorkflowStateAndContinue(ChangePinController.WorkflowStates.Reader)
				break
			case "StatePreparePace":
				replace(pinProgressView)
				setPinWorkflowStateAndContinue(ChangePinController.WorkflowStates.Update)
				break
			case "StateEnterPacePassword":
				if (NumberModel.passwordType === PasswordType.TRANSPORT_PIN) {
					setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.TransportPin)
				}
				if (NumberModel.passwordType === PasswordType.PIN) {
					setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.Pin)
				}
				else if (NumberModel.passwordType === PasswordType.CAN) {
					setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.Can)
				}
				else if (NumberModel.passwordType === PasswordType.PUK) {
					setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.Puk)
				}
				else if (NumberModel.passwordType === PasswordType.SMART_PIN) {
					setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.Pin)
				}
				break
			case "StateUnfortunateCardPosition":
				push(cardPositionView)
				break
			case "StateEnterNewPacePin":
				setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.NewPin)
				break
			case "StateCleanUpReaderManager":
				setPinWorkflowStateAndContinue(ChangePinController.WorkflowStates.Processing)
				break
			case "FinalState":
				showRemoveCardFeedback(ChangePinModel, false)
				if (ChangePinModel.shouldSkipResultView()) {
					ChangePinModel.continueWorkflow()
					popAll()
					setLockedAndHidden(false)
					break
				}
				push(pinResult)
				break
			default:
				ChangePinModel.continueWorkflow()
		}
	}

	function setPinWorkflowStateAndContinue(pState) {
		controller.workflowState = pState
		ChangePinModel.continueWorkflow()
	}

	function setPinWorkflowStateAndRequestInput(pState) {
		controller.workflowState = pState
		if (ChangePinModel.isBasicReader) {
			push(enterPinView)
		} else {
			ChangePinModel.continueWorkflow()
		}
	}
}
