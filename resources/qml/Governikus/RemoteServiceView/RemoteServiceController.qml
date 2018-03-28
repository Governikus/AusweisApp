import QtQuick 2.5

Item {
	id: controller

	Connections {
		target: remoteServiceModel
		onFireCurrentStateChanged: {
			switch (remoteServiceModel.currentState) {
				case "":
					break
				case "StateStartRemoteService":
					navBar.lockedAndHidden = true
					if (Qt.platform.os === "android") {
						remoteServiceModel.readerPlugInType = "NFC";
					} else {
						remoteServiceModel.readerPlugInType = "PCSC";
					}
					setWorkflowStateAndContinue("startRemoteService")
				case "StateProcessRemoteMessages":
					firePopAll()
					numberModel.continueWorkflow()
					break
				case "StateEstablishPaceChannel":
					enterPinView.state = "INITIAL"
					setWorkflowStateAndRequestInput("establishPaceChannel", remoteServiceModel.getPacePasswordId())
					break
				case "StateChangePinRemote":
					enterPinView.state = "INITIAL"
					setWorkflowStateAndRequestInput("changePinRemote", "PIN_NEW")
					break
				case "FinalState":
					numberModel.continueWorkflow()
					navBar.lockedAndHidden = false
					break
				default:
					numberModel.continueWorkflow()
			}
		}
	}

	function setWorkflowState(pState) {
		state = pState
	}

	function setWorkflowStateAndContinue(pState) {
		setWorkflowState(pState)
		numberModel.continueWorkflow()
	}

	function setWorkflowStateAndRequestInput(pState, pEnterPinState) {
		setWorkflowState(pState)
		if (remoteServiceModel.pinPadModeOn()) {
			firePush(enterPinView, {state: pEnterPinState})
		}
	}
}
