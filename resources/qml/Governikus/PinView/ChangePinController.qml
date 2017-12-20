import QtQuick 2.5

Item {
	id: controller
	readonly property string currentState: changePinModel.currentState
	property bool showRemoveCardFeedback: false

	property bool locationPermissionConfirmed: false
	onLocationPermissionConfirmedChanged: {
		// If the user has given location permission: continue Bluetooth workflow.
		if (changePinModel.readerPlugInType === "BLUETOOTH" && applicationModel.bluetoothEnabled && applicationModel.locationPermissionRequired && locationPermissionConfirmed) {
			numberModel.continueWorkflow()
		}
	}

	Connections {
		target: changePinModel

		onFireNewContextSet: {
			if (pinView.stack.currentItem !== pinWorkflow)
			{
				baseItem.firePopAll()
				baseItem.firePush(pinWorkflow, {})

				if (Qt.platform.os === "android") {
					changePinModel.readerPlugInType = "NFC"
				} else if (Qt.platform.os === "ios") {
					changePinModel.readerPlugInType = "BLUETOOTH"
				} else {
					changePinModel.readerPlugInType = "PCSC"
				}
			}

			navBar.lockedAndHidden = true
			enterPinView.state = "INITIAL"
			setPinWorkflowState("initial")
		}

		onFireCurrentStateChanged: processStateChange()
		// This is necessary because onCurrentStateChanged is not
		// working, when we need to process a state a second time
	}

	function processStateChange() {
		switch (changePinModel.currentState) {
			case "":
				break
			case "StateSelectReader":
				firePush(pinWorkflow, {})
				if (changePinModel.readerPlugInType === "BLUETOOTH" && applicationModel.bluetoothEnabled && applicationModel.locationPermissionRequired && !locationPermissionConfirmed) {
					// Stop the workflow here until the user has confirmed the location permission.
					setPinWorkflowState("reader")
				}
				else
				{
					setPinWorkflowStateAndContinue("reader")
				}
				break
			case "StateConnectCard":
				setPinWorkflowStateAndContinue("card")
				break
			case "StateHandleRetryCounter":
				if (!changePinModel.isBasicReader) {
					firePush(pinProgressView, {})
				}
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
				controller.showRemoveCardFeedback = numberModel.cardConnected && !changePinModel.error;
				numberModel.continueWorkflow()
				break;
			case "FinalState":
				if (controller.showRemoveCardFeedback) {
					controller.showRemoveCardFeedback = false
					qmlExtension.showFeedback(qsTr("You may now remove your ID card from the device."))
				}
				baseItem.firePush(pinResult, {})
				navBar.lockedAndHidden = true
				break
			default:
				numberModel.continueWorkflow()
		}
	}

	function setPinWorkflowState(pState) {
		pinWorkflow.state = pState
	}

	function setPinWorkflowStateAndContinue(pState) {
		setPinWorkflowState(pState)
		numberModel.continueWorkflow()
	}

	function setPinWorkflowStateAndRequestInput(pState, pInput) {
		pinWorkflow.state = pState
		if (changePinModel.isBasicReader) {
			baseItem.firePush(enterPinView, {state: pInput})
		} else {
			numberModel.continueWorkflow()
		}
	}
}
