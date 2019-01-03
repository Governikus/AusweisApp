import QtQuick 2.10

import Governikus.Type.RemoteServiceModel 1.0
import Governikus.EnterPinView 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.ReaderPlugIn 1.0
import Governikus.Type.NumberModel 1.0

Item {
	id: controller

	Connections {
		target: RemoteServiceModel
		onFireCurrentStateChanged: {
			switch (RemoteServiceModel.currentState) {
				case "":
					break
				case "StateStartRemoteService":
					navBar.lockedAndHidden = true
					if (Qt.platform.os === "android") {
						RemoteServiceModel.readerPlugInType = ReaderPlugIn.NFC;
					} else {
						RemoteServiceModel.readerPlugInType = ReaderPlugIn.PCSC;
					}
					setWorkflowStateAndContinue("startRemoteService")
					break
				case "StateProcessRemoteMessages":
					firePopAll()
					RemoteServiceModel.continueWorkflow()
					break
				case "StateEnterPacePasswordRemote":
					if (RemoteServiceModel.isBasicReader) {
						enterPinView.state = "INITIAL"
						setWorkflowStateAndRequestInput("establishPaceChannelRemote", RemoteServiceModel.getPacePasswordId())
					} else {
						RemoteServiceModel.continueWorkflow()
					}
					break
				case "StateEnterNewPacePinRemote":
					if (RemoteServiceModel.isBasicReader) {
						enterPinView.state = "INITIAL"
						setWorkflowStateAndRequestInput("changePinRemote", "PIN_NEW")
					} else {
						RemoteServiceModel.continueWorkflow()
					}
					break
				case "FinalState":
					RemoteServiceModel.continueWorkflow()
					navBar.lockedAndHidden = false
					break
				default:
					RemoteServiceModel.continueWorkflow()
			}
		}
	}

	function setWorkflowState(pState) {
		state = pState
	}

	function setWorkflowStateAndContinue(pState) {
		setWorkflowState(pState)
		RemoteServiceModel.continueWorkflow()
	}

	function setWorkflowStateAndRequestInput(pState, pEnterPinState) {
		setWorkflowState(pState)
		if (RemoteServiceModel.pinPadModeOn()) {
			enterPinView.state = pEnterPinState
			firePush(enterPinView)
		}
	}

	EnterPinView {
		id: enterPinView
		visible: false
		enableTransportPinLink: RemoteServiceModel.isSaCPinChangeWorkflow

		leftTitleBarAction: TitleBarAction {
			state: "cancel"
			onClicked: {
				firePop()
				RemoteServiceModel.cancelPasswordRequest()
			}
		}

		onPinEntered: {
			firePop()
			RemoteServiceModel.continueWorkflow()
		}

		onChangePinLength: NumberModel.requestTransportPin = !NumberModel.requestTransportPin
	}
}
