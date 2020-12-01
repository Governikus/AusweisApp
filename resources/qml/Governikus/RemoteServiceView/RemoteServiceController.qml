/*
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Type.RemoteServiceModel 1.0
import Governikus.EnterPasswordView 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.ReaderPlugIn 1.0
import Governikus.Type.NumberModel 1.0

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
			firePushWithProperties(enterPinView, {state: pEnterPinState})
		} else {
			RemoteServiceModel.continueWorkflow()
		}
	}

	Component {
		id: enterPinView

		EnterPasswordView {
			id: passwordView

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
			}

			onChangePinLength: NumberModel.requestTransportPin = !NumberModel.requestTransportPin

			Connections {
				target: RemoteServiceModel
				onFireConnectedChanged: if (!RemoteServiceModel.connectedToPairedDevice && passwordView.StackView.visible) firePop()
			}
		}
	}
}
