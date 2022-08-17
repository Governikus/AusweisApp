/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.View 1.0
import Governikus.Type.ChangePinModel 1.0
import Governikus.Type.NumberModel 1.0


Controller {
	id: controller

	enum WorkflowStates {
			Initial,
			Reader,
			Update,
			Password,
			Processing
	}


	property int workflowState: ChangePinController.WorkflowStates.Initial

	Connections {
		target: ChangePinModel

		function onFireCurrentStateChanged() { processStateChange() }
		// This is necessary because onCurrentStateChanged is not
		// working, when we need to process a state a second time.
	}

	function processStateChange() {
		switch (ChangePinModel.currentState) {
			case "Initial":
				ChangePinModel.setInitialPluginType()
				break
			case "StateSelectReader":
				controller.nextView(ChangePinView.SubViews.Workflow)
				setPinWorkflowStateAndContinue(ChangePinController.WorkflowStates.Reader)
				break
			case "StateEnterPacePassword":
				setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.Password)
				break
			case "StateUnfortunateCardPosition":
				controller.nextView(ChangePinView.SubViews.CardPosition)
				break
			case "StateEnterNewPacePin":
				setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.Password)
				break
			case "StateCleanUpReaderManager":
				setPinWorkflowStateAndContinue(ChangePinController.WorkflowStates.Processing)
				break
			case "FinalState":
				showRemoveCardFeedback(ChangePinModel, false)
				if (ChangePinModel.shouldSkipResultView()) {
					controller.nextView(ChangePinView.SubViews.ReturnToMain)
					ChangePinModel.continueWorkflow()
				} else {
					controller.nextView(ChangePinView.SubViews.Result)
				}
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
			controller.nextView(ChangePinView.SubViews.Password)
		} else {
			ChangePinModel.continueWorkflow()
		}
	}
}
