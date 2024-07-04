/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.View
import Governikus.Type

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

	function processStateChange(pState) {
		switch (pState) {
		case "StateSelectReader":
			controller.nextView(ChangePinView.SubViews.Workflow);
			setPinWorkflowStateAndContinue(ChangePinController.WorkflowStates.Reader);
			break;
		case "StateEnterPacePassword":
			setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.Password);
			break;
		case "StateUnfortunateCardPosition":
			controller.nextView(ChangePinView.SubViews.CardPosition);
			break;
		case "StateEnterNewPacePin":
			setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.Password);
			break;
		case "StateCleanUpReaderManager":
			setPinWorkflowStateAndContinue(ChangePinController.WorkflowStates.Processing);
			break;
		case "FinalState":
			showRemoveCardFeedback(ChangePinModel, false);
			if (ChangePinModel.shouldSkipResultView()) {
				controller.nextView(ChangePinView.SubViews.ReturnToMain);
				ChangePinModel.continueWorkflow();
			} else {
				controller.nextView(ChangePinView.SubViews.Result);
			}
			break;
		default:
			ChangePinModel.continueWorkflow();
		}
	}
	function setPinWorkflowStateAndContinue(pState) {
		controller.workflowState = pState;
		ChangePinModel.continueWorkflow();
	}
	function setPinWorkflowStateAndRequestInput(pState) {
		controller.workflowState = pState;
		if (ChangePinModel.isBasicReader) {
			controller.nextView(ChangePinView.SubViews.Password);
		} else {
			ChangePinModel.continueWorkflow();
		}
	}

	Connections {
		function onFireStateEntered(pState) {
			processStateChange(pState);
		}
		function onFireWorkflowStarted() {
			ChangePinModel.setInitialPluginType();
		}

		target: ChangePinModel
	}
}
