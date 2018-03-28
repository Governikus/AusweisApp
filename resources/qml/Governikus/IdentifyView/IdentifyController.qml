import QtQuick 2.5


Item {
	id: controller
	readonly property string currentState: authModel.currentState
	property bool showRemoveCardFeedback: false

	property bool locationPermissionConfirmed: false
	onLocationPermissionConfirmedChanged: {
		// If the user has given location permission: continue Bluetooth workflow.
		if (authModel.readerPlugInType === "BLUETOOTH" && applicationModel.bluetoothEnabled && applicationModel.locationPermissionRequired && locationPermissionConfirmed) {
			numberModel.continueWorkflow()
		}
	}

	states: [
		State {
			when: authModel.currentState === "StateGetTcToken" && !connectivityManager.networkInterfaceActive
			StateChangeScript {
				script: firePush(checkConnectivityView, {})
			}
		},
		State {
			when: authModel.currentState === "StateGetTcToken" && connectivityManager.networkInterfaceActive
			StateChangeScript {
				script: {
					firePush(identifyProgressView, {})
					numberModel.continueWorkflow()
				}
			}
		}
	]

	Component.onCompleted: if (authModel.currentState === "StateProcessing") processStateChange()

	Connections {
		target: authModel
		onFireCurrentStateChanged: processStateChange()
		// This is necessary because onCurrentStateChanged is not
		// working, when we need to process a state a second time
	}

	function processStateChange() {
		switch (authModel.currentState) {
			case "":
				break;
			case "StateGetTcToken":
				enterPinView.state = "INITIAL"
				identifyWorkflow.state = "initial"
				navBar.lockedAndHidden = true
				navBar.state = "identify"
				navBar.currentIndex = 0
				break
			case "StateEditAccessRights":
				firePush(identifyViewContent, {})

				authModel.setInitialPluginType()
				break
			case "StateSelectReader":
				firePush(identifyWorkflow, {})
				if (authModel.readerPlugInType === "BLUETOOTH" && applicationModel.bluetoothEnabled && applicationModel.locationPermissionRequired && !locationPermissionConfirmed) {
					// Stop the workflow here until the user has confirmed the location permission.
					setIdentifyWorkflowState("reader")
				}
				else
				{
					setIdentifyWorkflowStateAndContinue("reader")
				}
				break
			case "StateConnectCard":
				setIdentifyWorkflowStateAndContinue("card")
				break
			case "StateHandleRetryCounter":
				if (!authModel.isBasicReader) {
					firePush(identifyProgressView, {})
				}
				setIdentifyWorkflowStateAndContinue("updateretrycounter")
				break
			case "StateEstablishPaceCan":
				setIdentifyWorkflowStateAndRequestInput("identify_entercan", "CAN")
				break
			case "StateEstablishPacePin":
				setIdentifyWorkflowStateAndRequestInput("identify_enterpin", "PIN")
				break
			case "StateDidAuthenticateEac1":
				identifyProgressView.progressBarVisible = true
				setIdentifyProgressViewValue(1)
				setIdentifyWorkflowStateAndContinue("identify_processing")
				break
			case "StateEstablishPacePuk":
				authModel.cancelWorkflowOnPinBlocked()
				break
			case "StateDidAuthenticateEac2":
				setIdentifyProgressViewValue(2)
				numberModel.continueWorkflow()
				break
			case "StateTransmit":
				setIdentifyProgressViewValue(3)
				numberModel.continueWorkflow()
				break
			case "StateCleanUpReaderManager":
				controller.showRemoveCardFeedback = numberModel.cardConnected && !authModel.isError;
				numberModel.continueWorkflow()
				break;
			case "StateCheckRefreshAddress":
				setIdentifyProgressViewValue(4)
				numberModel.continueWorkflow()
				break
			case "StateWriteHistory":
				setIdentifyProgressViewValue(5)
				numberModel.continueWorkflow()
				break
			case "FinalState":
				navBar.lockedAndHidden = true
				if (controller.showRemoveCardFeedback) {
					controller.showRemoveCardFeedback = false
					qmlExtension.showFeedback(qsTr("You may now remove your ID card from the device."))
				}

				if (authModel.error)
					firePush(identifyResult, {})
				else {
					if (applicationModel.currentWorkflow === "selfauthentication") {
						firePush(selfAuthenticationData, {})
					} else {
						numberModel.continueWorkflow()
						firePopAll()
						navBar.lockedAndHidden = false
					}
				}
				identifyProgressView.progressBarVisible = false
				setIdentifyProgressViewValue(0)
				break
			default:
				numberModel.continueWorkflow()
		}
	}

	function setIdentifyProgressViewValue(value){
		identifyProgressView.progressValue = value
	}

	function setIdentifyWorkflowState(pState) {
		identifyWorkflow.state = pState
	}

	function setIdentifyWorkflowStateAndContinue(pState) {
		setIdentifyWorkflowState(pState);
		numberModel.continueWorkflow()
	}

	function setIdentifyWorkflowStateAndRequestInput(pState, pInput) {
		identifyWorkflow.state = pState
		if (authModel.isBasicReader) {
			firePush(enterPinView, {state: pInput})
		} else {
			numberModel.continueWorkflow()
		}
	}
}
