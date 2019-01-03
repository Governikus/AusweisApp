import QtQuick 2.10

import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.AuthModel 1.0

SectionPage {
	enum WorkflowStates {
		Initial,
		Reader,
		Card,
		Update,
		Can,
		Pin,
		Processing
	}

	id: controller
	readonly property string currentState: AuthModel.currentState

	property bool connectedToCard: false
	property int workflowState: 0
	property int workflowProgressValue: 0
	property bool workflowProgressVisible: false

	states: [
		State {
			when: AuthModel.currentState === "StateGetTcToken" && !connectivityManager.networkInterfaceActive
			StateChangeScript {
				script: controller.nextView(IdentifyView.SubViews.Connectivity)
			}
		},
		State {
			when: AuthModel.currentState === "StateGetTcToken" && connectivityManager.networkInterfaceActive
			StateChangeScript {
				script: {
					controller.nextView(IdentifyView.SubViews.Progress)
					AuthModel.continueWorkflow()
				}
			}
		}
	]

	Component.onCompleted: if (AuthModel.currentState === "StateProcessing") processStateChange()

	Connections {
		target: AuthModel
		onFireCurrentStateChanged: processStateChange()
		// This is necessary because onCurrentStateChanged is not
		// working, when we need to process a state a second time
	}

	function showRemoveCardFeedback() {
		if (controller.connectedToCard) {
			controller.connectedToCard = false
			qmlExtension.showFeedback(qsTr("You may now remove your ID card from the device."))
		}
	}

	function processStateChange() {
		switch (AuthModel.currentState) {
			case "":
				break;
			case "StateGetTcToken":
				// enterPinView.state = "INITIAL"
				controller.workflowState = IdentifyController.WorkflowStates.Initial
				break
			case "StateEditAccessRights":
				controller.nextView(IdentifyView.SubViews.AccessRights)
				AuthModel.setInitialPluginType()
				break
			case "StateSelectReader":
				controller.nextView(IdentifyView.SubViews.Workflow)
				setIdentifyWorkflowStateAndContinue(IdentifyController.WorkflowStates.Reader)
				break
			case "StateConnectCard":
				setIdentifyWorkflowStateAndContinue(IdentifyController.WorkflowStates.Card)
				break
			case "StateHandleRetryCounter":
				controller.nextView(IdentifyView.SubViews.Progress)
				setIdentifyWorkflowStateAndContinue(IdentifyController.WorkflowStates.Update)
				break
			case "StateEstablishPaceCan":
				setIdentifyWorkflowStateAndRequestInput(IdentifyController.WorkflowStates.Can, "CAN")
				break
			case "StateEstablishPacePin":
				setIdentifyWorkflowStateAndRequestInput(IdentifyController.WorkflowStates.Pin, "PIN")
				break
			case "StateDidAuthenticateEac1":
				controller.workflowProgressVisible = true
				controller.workflowProgressValue = 1
				setIdentifyWorkflowStateAndContinue(IdentifyController.WorkflowStates.Processing)
				break
			case "StateEstablishPacePuk":
				AuthModel.cancelWorkflowOnPinBlocked()
				break
			case "StateDidAuthenticateEac2":
				controller.workflowProgressValue = 2
				AuthModel.continueWorkflow()
				break
			case "StateTransmit":
				controller.workflowProgressValue = 3
				AuthModel.continueWorkflow()
				break
			case "StateCleanUpReaderManager":
				controller.connectedToCard = AuthModel.selectedReaderHasCard() && !AuthModel.isError;
				AuthModel.continueWorkflow()
				break;
			case "StateCheckRefreshAddress":
				controller.workflowProgressValue = 4
				AuthModel.continueWorkflow()
				break
			case "StateWriteHistory":
				showRemoveCardFeedback()
				controller.workflowProgressValue = 5
				AuthModel.continueWorkflow()
				break
			case "StateShowSelfInfo":
				controller.nextView(IdentifyView.SubViews.Data)
				break
			case "FinalState":
				if (AuthModel.error) {
					showRemoveCardFeedback()
					controller.nextView(IdentifyView.SubViews.Result)
				} else {
					AuthModel.continueWorkflow()
					controller.nextView(IdentifyView.SubViews.ReturnToMain)
				}
				controller.workflowProgressVisible = false
				controller.workflowProgressValue = 0
				break
			default:
				AuthModel.continueWorkflow()
		}
	}

	function setIdentifyWorkflowStateAndContinue(pState) {
		controller.workflowState = pState
		AuthModel.continueWorkflow()
	}

	function setIdentifyWorkflowStateAndRequestInput(pState, pInput) {
		controller.workflowState = pState
		if (AuthModel.isBasicReader) {
			// enterPinView.state = pInput
			controller.nextView(IdentifyView.SubViews.Password)
		} else {
			AuthModel.continueWorkflow()
		}
	}
}
