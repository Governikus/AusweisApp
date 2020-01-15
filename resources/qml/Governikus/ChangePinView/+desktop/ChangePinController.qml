/*
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.ChangePinModel 1.0
import Governikus.Type.NumberModel 1.0


Controller {
	id: controller

	enum WorkflowStates {
			Initial,
			Reader,
			Card,
			Update,
			Password,
			Processing
	}


	property int workflowState: ChangePinController.WorkflowStates.Initial

	QtObject {
		id: d

		property bool showRemoveCardFeedback: false
	}

	Connections {
		target: ChangePinModel

		onFireCurrentStateChanged: processStateChange()
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
			case "StateConnectCard":
				setPinWorkflowStateAndContinue(ChangePinController.WorkflowStates.Card)
				break
			case "StateEnterPacePassword":
				setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.Password)
				break
			case "StateUnfortunateCardPosition":
				controller.nextView(ChangePinView.SubViews.CardPosition)
				break
			case "StateEnterNewPacePin":
				NumberModel.requestNewPin()
				setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.Password)
				break
			case "StateCleanUpReaderManager":
				d.showRemoveCardFeedback = ChangePinModel.selectedReaderHasCard() && !ChangePinModel.error;
				setPinWorkflowStateAndContinue(ChangePinController.WorkflowStates.Processing)
				break
			case "FinalState":
				if (d.showRemoveCardFeedback) {
					d.showRemoveCardFeedback = false
					//: INFO DESKTOP_QML Changing the PIN was successful; hint that the ID card may now be removed from the card reader.
					ApplicationModel.showFeedback(qsTr("You may now remove your ID card from the device."))
				}
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
