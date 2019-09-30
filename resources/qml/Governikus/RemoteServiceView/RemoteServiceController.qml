/*
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Type.RemoteServiceModel 1.0
import Governikus.EnterPasswordView 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.ReaderPlugIn 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.ApplicationModel 1.0

Item {
	id: controller

	Connections {
		target: RemoteServiceModel
		onFireCurrentStateChanged: {
			switch (RemoteServiceModel.currentState) {
				case "Initial":
					break
				case "StateStartRemoteService":
					navBar.lockedAndHidden = true
					setWorkflowStateAndContinue("startRemoteService")
					break
				case "StateProcessRemoteMessages":
					RemoteServiceModel.continueWorkflow()
					break
				case "StateEnterPacePasswordRemote":
					setWorkflowStateAndRequestInput("establishPaceChannelRemote", RemoteServiceModel.getPasswordType())
					break
				case "StateEnterNewPacePinRemote":
					setWorkflowStateAndRequestInput("changePinRemote", "PIN_NEW")
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
		if (RemoteServiceModel.isBasicReader && RemoteServiceModel.pinPadModeOn()) {
			enterPinView.state = pEnterPinState
			firePush(enterPinView)
			ApplicationModel.nfcRunning = false
		} else {
			RemoteServiceModel.continueWorkflow()
		}
	}

	EnterPasswordView {
		id: enterPinView
		visible: false
		enableTransportPinLink: RemoteServiceModel.isSaCPinChangeWorkflow

		navigationAction: NavigationAction {
			state: "cancel"
			onClicked: {
				firePop()
				RemoteServiceModel.cancelPasswordRequest()
			}
		}

		onPasswordEntered: {
			firePop()
			RemoteServiceModel.continueWorkflow()
			ApplicationModel.nfcRunning = true
		}

		onChangePinLength: NumberModel.requestTransportPin = !NumberModel.requestTransportPin
	}
}
