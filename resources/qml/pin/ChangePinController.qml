import QtQuick 2.5
import QtQml.StateMachine 1.0

import "../"


Item {
	id: controller
	property string readerType

	Connections {
		target: changePinModel
		onFireCurrentStateChanged: {
			switch (changePinModel.currentState) {
				case "":
					break
				case "StateSelectReaderType":
					pinView.pop(null)
					pinView.push(pinWorkflow)
					navBar.lockedAndHidden = true
					enterPinView.state = "INITIAL"
					setPinWorkflowStateAndContinue("initial")
				case "StateSelectBluetoothReader":
					readerType = "bt_"
					setPinWorkflowStateAndContinue("connect")
					break
				case "StateSelectNfcReader":
					readerType = "nfc_"
					setPinWorkflowStateAndContinue("connect")
					break
				case "StateConnectCard":
					setPinWorkflowStateAndContinue("card")
					break
				case "StateUpdateRetryCounter":
					setPinWorkflowStateAndContinue("updateretrycounter")
					break
				case "StateEstablishPacePuk":
					setPinWorkflowStateAndRequestInput("enterpuk", "PUK")
					break
				case "StateEstablishPaceCan":
					setPinWorkflowStateAndRequestInput("changepin_entercan", "CAN")
					break
				case "StateEstablishPacePin":
					setPinWorkflowStateAndRequestInput("changepin_enterpin", "PIN_OR_TRANSPORT_PIN")
					break
				case "StateChangePin":
					setPinWorkflowStateAndRequestInput("enternewpin", "PIN_NEW")
					break
				case "FinalState":
					pinView.push(pinResult)
					navBar.lockedAndHidden = true
					break
				default:
					changePinModel.continueWorkflow()
			}

		}
	}

	function setPinWorkflowStateAndContinue(pState) {
		pinWorkflow.state = readerType + pState
		changePinModel.continueWorkflow()
	}

	function setPinWorkflowStateAndRequestInput(pState, pInput) {
		pinWorkflow.state = readerType + pState
		if (changePinModel.isBasicReader()) {
			pinView.push(enterPinView, {state: pInput})
		} else {
			changePinModel.continueWorkflow()
		}
	}
}
