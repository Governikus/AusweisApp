import QtQuick 2.5
import QtQml.StateMachine 1.0

import "../"


Item {
	id: controller
	readonly property string statePrefix: readerType === "BLUETOOTH" ? "bt_" : "nfc_"
	property string readerType
	property bool showRemoveCardFeedback: false

	Connections {
		target: changePinModel
		onFireCurrentStateChanged: {
			switch (changePinModel.currentState) {
				case "":
					break
				case "StateSelectReaderType":
					if(pinView.stack.currentItem !== pinWorkflow)
					{
						pinView.pop(null)
						pinView.push(pinWorkflow)
						readerType = Qt.platform.os === "android" ? "NFC" : "PCSC"
					}
					navBar.lockedAndHidden = true
					enterPinView.state = "INITIAL"
					changePinModel.setReaderType(readerType)
					setPinWorkflowStateAndContinue("initial")
					break
				case "StateSelectBluetoothReader":
					readerType = "BLUETOOTH"
					setPinWorkflowState("connect")
					if (applicationModel.bluetoothEnabled && !applicationModel.locationPermissionRequired) {
						numberModel.continueWorkflow()
					}
					break
				case "StateSelectNfcReader":
					readerType = "NFC"
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
				case "StateCleanUpReaderManager":
					controller.showRemoveCardFeedback = numberModel.cardConnected && changePinModel.changedPinSuccessfully;
					numberModel.continueWorkflow()
					break;
				case "FinalState":
					if (controller.showRemoveCardFeedback) {
						controller.showRemoveCardFeedback = false
						qmlExtension.showFeedback(qsTr("You may now remove your ID card from the device."))
					}
					pinView.push(pinResult)
					navBar.lockedAndHidden = true
					break
				default:
					numberModel.continueWorkflow()
			}

		}
	}

	function setPinWorkflowState(pState) {
		pinWorkflow.state = statePrefix + pState
	}

	function setPinWorkflowStateAndContinue(pState) {
		setPinWorkflowState(pState)
		numberModel.continueWorkflow()
	}

	function setPinWorkflowStateAndRequestInput(pState, pInput) {
		pinWorkflow.state = statePrefix + pState
		if (changePinModel.isBasicReader()) {
			pinView.push(enterPinView, {state: pInput})
		} else {
			numberModel.continueWorkflow()
		}
	}
}
