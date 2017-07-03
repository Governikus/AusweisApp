import QtQuick 2.5
import QtQml.StateMachine 1.0

import "../"


Item {
	id: controller
	property string readerType

	Connections {
		target: authModel
		onFireCurrentStateChanged: {
			switch (authModel.currentState) {
				case "":
					break;
				case "StateGetTcToken":
					enterPinView.state = "INITIAL"
					identifyView.push(identifyProgressView)
					navBar.lockedAndHidden = true
					navBar.state = "identify"
					navBar.currentIndex = 0
					setIdentifyWorkflowStateAndContinue("initial")
					break;
				case "StateEditAccessRights":
					if (applicationModel.currentWorkflow === "selfauthentication") {
						identifyView.push(identifyWorkflow)
						chatModel.transferAccessRights()
						authModel.continueWorkflow()
					}
					if (applicationModel.currentWorkflow === "authentication") {
						identifyView.push(identifyEditChatView)
					}
					break
				case "StateSelectNfcReader":
					readerType = "nfc_"
					identifyView.push(identifyWorkflow)
					setIdentifyWorkflowStateAndContinue("connect")
					break
				case "StateSelectBluetoothReader":
					readerType = "bt_"
					identifyView.push(identifyWorkflow)
					setIdentifyWorkflowStateAndContinue("connect")
					break
				case "StateConnectCard":
					setIdentifyWorkflowStateAndContinue("card")
					break
				case "StateUpdateRetryCounter":
					setIdentifyWorkflowStateAndContinue("updateretrycounter")
					break
				case "StateEstablishPaceCan":
					setIdentifyWorkflowStateAndRequestInput("identify_entercan", "CAN")
					break
				case "StateEstablishPacePin":
					setIdentifyWorkflowStateAndRequestInput("identify_enterpin", "PIN")
					break
				case "FinalState":
					navBar.lockedAndHidden = true
					if (authModel.error)
						identifyView.push(identifyResult)
					else {
						if (applicationModel.currentWorkflow === "selfauthentication") {
							identifyView.push(selfAuthenticationData)
						} else {
							authModel.continueWorkflow()
							titleBar.reset()
							identifyView.pop(null)
							navBar.lockedAndHidden = false
						}
					}
					break
				default:
					authModel.continueWorkflow()
			}
		}
	}

	function setIdentifyWorkflowStateAndContinue(pState) {
		identifyWorkflow.state = readerType + pState
		authModel.continueWorkflow()
	}

	function setIdentifyWorkflowStateAndRequestInput(pState, pInput) {
		identifyWorkflow.state = readerType + pState
		if (authModel.isBasicReader()) {
			identifyView.push(enterPinView, {state: pInput})
		} else {
			authModel.continueWorkflow()
		}
	}
}
