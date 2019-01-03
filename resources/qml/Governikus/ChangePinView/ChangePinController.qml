import QtQuick 2.10

import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.ChangePinModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.ReaderPlugIn 1.0
import Governikus.Type.PacePasswordId 1.0


Item {
	enum WorkflowStates {
			Initial,
			Reader,
			Card,
			Update,
			Puk,
			Can,
			Pin,
			NewPin,
			Processing
	}

	QtObject {
		id: d
		readonly property int readerPlugInType: ChangePinModel.readerPlugInType
	}

	id: controller
	readonly property string currentState: ChangePinModel.currentState
	readonly property bool bluetoothEnabled: ApplicationModel.bluetoothEnabled

	property bool showRemoveCardFeedback: false
	property int workflowState: 0

	function sufficientBluetoothRights() {
		return ApplicationModel.bluetoothEnabled && (!ApplicationModel.locationPermissionRequired || locationPermissionConfirmed)
	}

	property bool locationPermissionConfirmed: false
	onLocationPermissionConfirmedChanged: {
		// If the user has given location permission: continue Bluetooth workflow.
		if (d.readerPlugInType === ReaderPlugIn.BLUETOOTH && sufficientBluetoothRights()) {
			ChangePinModel.continueWorkflow()
		}
	}

	onBluetoothEnabledChanged: {
		if (d.readerPlugInType === ReaderPlugIn.BLUETOOTH && sufficientBluetoothRights()) {
			ChangePinModel.continueWorkflow()
		}
	}

	Connections {
		target: ChangePinModel

		onFireNewContextSet: {
			navBar.lockedAndHidden = true
			navBar.state = "pin"
			navBar.currentIndex = 3
			enterPinView.state = "INITIAL"
			controller.workflowState = ChangePinController.WorkflowStates.Initial
			ChangePinModel.setInitialPluginType()
		}

		onFireCurrentStateChanged: processStateChange()
		// This is necessary because onCurrentStateChanged is not
		// working, when we need to process a state a second time
	}

	function processStateChange() {
		switch (currentState) {
			case "":
				break
			case "StateSelectReader":
				fireReplace(pinWorkflow)
				if (d.readerPlugInType === ReaderPlugIn.BLUETOOTH && !sufficientBluetoothRights()) {
					// Stop the workflow here until the user has enabled bluetooth and confirmed the location permission.
					controller.workflowState = ChangePinController.WorkflowStates.Reader
				}
				else {
					setPinWorkflowStateAndContinue(ChangePinController.WorkflowStates.Reader)
				}
				break
			case "StateConnectCard":
				setPinWorkflowStateAndContinue(ChangePinController.WorkflowStates.Card)
				break
			case "StatePreparePace":
				fireReplace(pinProgressView)
				setPinWorkflowStateAndContinue(ChangePinController.WorkflowStates.Update)
				break
			case "StateEnterPacePassword":
				if (NumberModel.establishPaceChannelType == PacePasswordId.PACE_PIN) {
					setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.Pin, "PIN")
				}
				else if (NumberModel.establishPaceChannelType == PacePasswordId.PACE_CAN) {
					setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.Can, "CAN")
				}
				else if (NumberModel.establishPaceChannelType == PacePasswordId.PACE_PUK) {
					setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.Puk, "PUK")
				}
				break
			case "StateUnfortunateCardPosition":
				baseItem.firePush(cardPositionView)
				break
			case "StateEnterNewPacePin":
				setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.NewPin, "PIN_NEW")
				break
			case "StateCleanUpReaderManager":
				controller.showRemoveCardFeedback = ChangePinModel.selectedReaderHasCard() && !ChangePinModel.error;
				setPinWorkflowStateAndContinue(ChangePinController.WorkflowStates.Processing)
				break
			case "FinalState":
				if (controller.showRemoveCardFeedback) {
					controller.showRemoveCardFeedback = false
					qmlExtension.showFeedback(qsTr("You may now remove your ID card from the device."))
				}
				baseItem.firePush(pinResult)
				navBar.lockedAndHidden = true
				break
			default:
				ChangePinModel.continueWorkflow()
		}
	}

	function setPinWorkflowStateAndContinue(pState) {
		controller.workflowState = pState
		ChangePinModel.continueWorkflow()
	}

	function setPinWorkflowStateAndRequestInput(pState, pInput) {
		controller.workflowState = pState
		if (ChangePinModel.isBasicReader) {
			enterPinView.state = pInput
			baseItem.firePush(enterPinView)
		} else {
			ChangePinModel.continueWorkflow()
		}
	}
}
