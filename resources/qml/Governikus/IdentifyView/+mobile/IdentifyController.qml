/*
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.ReaderPlugIn 1.0


Item {
	enum WorkflowStates {
		Initial,
		Reader,
		Card,
		Update,
		Can,
		Pin,
		Processing
	}

	QtObject {
		id: d
		readonly property int readerPlugInType: AuthModel.readerPlugInType
	}

	id: controller
	readonly property string currentState: AuthModel.currentState
	readonly property bool bluetoothEnabled: ApplicationModel.bluetoothEnabled

	property bool connectedToCard: false
	property int workflowState: 0
	property bool workflowProgressVisible: false

	function sufficientBluetoothRights() {
		return ApplicationModel.bluetoothEnabled && (!ApplicationModel.locationPermissionRequired || locationPermissionConfirmed)
	}

	property bool locationPermissionConfirmed: false
	onLocationPermissionConfirmedChanged: {
		// If the user has given location permission: continue Bluetooth workflow.
		if (d.readerPlugInType === ReaderPlugIn.BLUETOOTH && sufficientBluetoothRights()) {
			AuthModel.continueWorkflow()
		}
	}

	onBluetoothEnabledChanged: {
		// If the user has activated bluetooth and no location permission is needed: continue Bluetooth workflow.
		if (d.readerPlugInType === ReaderPlugIn.BLUETOOTH && sufficientBluetoothRights()) {
			AuthModel.continueWorkflow()
		}
	}

	states: [
		State {
			when: AuthModel.currentState === "StateGetTcToken" && !connectivityManager.networkInterfaceActive
			StateChangeScript {
				script: firePush(checkConnectivityView)
			}
		},
		State {
			when: AuthModel.currentState === "StateGetTcToken" && connectivityManager.networkInterfaceActive
			StateChangeScript {
				script: {
					firePush(identifyProgressView)
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

			// The feedback notification will crash Apple's VoiceOver if it happens at the same time the app is redirecting back to the browser. This
			// happens with both the iOS toasts and our own toast-like replacement. To work around this, we'll only show the notification during a
			// self-authentication on iOS with VoiceOver running:
			if (Qt.platform.os !== "ios" || !ApplicationModel.isScreenReaderRunning() || ApplicationModel.currentWorkflow === "selfauthentication") {
				//: INFO ANDROID IOS The authentication process is completed, the id card may (and should) be removed from the card reader.
				ApplicationModel.showFeedback(qsTr("You may now remove your ID card from the device."))
			}
		}
	}

	function processStateChange() {
		switch (AuthModel.currentState) {
			case "Initial":
				break;
			case "StateGetTcToken":
				enterPinView.state = "INITIAL"
				controller.workflowState = IdentifyController.WorkflowStates.Initial
				navBar.lockedAndHidden = true
				navBar.state = "identify"
				navBar.currentIndex = 0
				break
			case "StateEditAccessRights":
				fireReplace(editRights)
				AuthModel.setInitialPluginType()
				break
			case "StateSelectReader":
				fireReplace(identifyWorkflow)
				if (d.readerPlugInType === ReaderPlugIn.BLUETOOTH && !sufficientBluetoothRights()) {
					// Stop the workflow here until the user has enabled bluetooth and confirmed the location permission.
					controller.workflowState = IdentifyController.WorkflowStates.Reader
				}
				else
				{
					setIdentifyWorkflowStateAndContinue(IdentifyController.WorkflowStates.Reader)
				}
				break
			case "StateConnectCard":
				setIdentifyWorkflowStateAndContinue(IdentifyController.WorkflowStates.Card)
				break
			case "StatePreparePace":
				fireReplace(identifyProgressView)
				setIdentifyWorkflowStateAndContinue(IdentifyController.WorkflowStates.Update)
				break
			case "StateEnterPacePassword":
				if (NumberModel.passwordType === NumberModel.PASSWORD_PIN) {
					setIdentifyWorkflowStateAndRequestInput(IdentifyController.WorkflowStates.Pin, "PIN")
				}
				else if (NumberModel.passwordType === NumberModel.PASSWORD_CAN) {
					setIdentifyWorkflowStateAndRequestInput(IdentifyController.WorkflowStates.Can, "CAN")
				}
				else if (NumberModel.passwordType === NumberModel.PASSWORD_PUK) {
					AuthModel.cancelWorkflowOnPinBlocked()
				}
				break
			case "StateUnfortunateCardPosition":
				//: INFO IOS The NFC signal is weak or unstable. The scan is stopped with this information in the iOS dialog.
				ApplicationModel.stopNfcScanWithError(qsTr("Weak NFC signal") + SettingsModel.translationTrigger)
				firePush(cardPositionView)
				break
			case "StateDidAuthenticateEac1":
				controller.workflowProgressVisible = true
				setIdentifyWorkflowStateAndContinue(IdentifyController.WorkflowStates.Processing)
				break
			case "StateSendDIDAuthenticateResponseEAC1":
				fireReplace(identifyProgressView)
				AuthModel.continueWorkflow()
				break
			case "StateCleanUpReaderManager":
				controller.connectedToCard = AuthModel.selectedReaderHasCard() && !AuthModel.isError;
				AuthModel.continueWorkflow()
				break;
			case "StateWriteHistory":
				showRemoveCardFeedback()
				AuthModel.continueWorkflow()
				break
			case "StateShowSelfInfo":
				firePush(selfAuthenticationData)
				break
			case "StateSendWhitelistSurvey":
				if (SettingsModel.askForDeviceSurvey() && !AuthModel.error && d.readerPlugInType === ReaderPlugIn.NFC) {
					firePush(whiteListSurveyView)
				} else {
					AuthModel.continueWorkflow()
				}
				break
			case "FinalState":
				navBar.lockedAndHidden = true
				if (AuthModel.error && !AuthModel.hasNextWorkflowPending && !AuthModel.shouldSkipResultView()) {
					showRemoveCardFeedback()
					firePush(identifyResult)
				} else {
					AuthModel.continueWorkflow()
					firePopAll()
					navBar.lockedAndHidden = false
				}
				controller.workflowProgressVisible = false
				break
			default:
				AuthModel.continueWorkflow()
		}
	}

	function setIdentifyWorkflowStateAndContinue(pState) {
		controller.workflowState = pState;
		AuthModel.continueWorkflow()
	}

	function setIdentifyWorkflowStateAndRequestInput(pState, pInput) {
		controller.workflowState = pState
		if (AuthModel.isBasicReader) {
			enterPinView.state = pInput
			firePush(enterPinView)
			ApplicationModel.nfcRunning = false
		} else {
			AuthModel.continueWorkflow()
		}
	}
}
