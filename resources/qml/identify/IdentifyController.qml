import QtQuick 2.5

import "../"


Item {
	id: controller
	readonly property string statePrefix: readerType === "BLUETOOTH" ? "bt_" : "nfc_"
	property string readerType
	property bool showRemoveCardFeedback: false

	states: [
		State {
			when: authModel.currentState === "StateGetTcToken" && !connectivityManager.networkInterfaceActive
			StateChangeScript {
				script: identifyView.push(checkConnectivityView)
			}
		},
		State {
			when: authModel.currentState === "StateGetTcToken" && connectivityManager.networkInterfaceActive
			StateChangeScript {
				script: {
					identifyView.push(identifyProgressView)
					numberModel.continueWorkflow()
				}
			}
		}
	]

	Connections {
		target: authModel
		onFireCurrentStateChanged: {
			switch (authModel.currentState) {
				case "":
					break;
				case "StateGetTcToken":
					enterPinView.state = "INITIAL"
					navBar.lockedAndHidden = true
					navBar.state = "identify"
					navBar.currentIndex = 0
					identifyWorkflow.state = "initial"
					break
				case "StateEditAccessRights":
					if (applicationModel.currentWorkflow === "selfauthentication") {
						identifyView.push(identifyWorkflow)
						chatModel.transferAccessRights()
						numberModel.continueWorkflow()
					}
					if (applicationModel.currentWorkflow === "authentication") {
						identifyView.push(identifyEditChatView)
					}
					readerType = Qt.platform.os === "android" ? "NFC" : "PCSC"
					break
				case "StateSelectReaderType":
					authModel.setReaderType(readerType);
					setIdentifyWorkflowStateAndContinue("initial")
					break
				case "StateSelectNfcReader":
					identifyView.push(identifyWorkflow)
					setIdentifyWorkflowStateAndContinue("connect")
					break
				case "StateSelectBluetoothReader":
					identifyView.push(identifyWorkflow)
					setIdentifyWorkflowState("connect")
					if (applicationModel.bluetoothEnabled && !applicationModel.locationPermissionRequired) {
						numberModel.continueWorkflow()
					}
					break
				case "StateConnectCard":
					setIdentifyWorkflowStateAndContinue("card")
					break
				case "StateUpdateRetryCounter":
					setIdentifyWorkflowStateAndContinue("updateretrycounter")
					break
				case "StateEstablishPaceCan":
					setIdentifyWorkflowStateAndRequestInput("identify_entercan", "CAN")
					break
				case "StateEstablishPacePin":
					setIdentifyWorkflowStateAndRequestInput("identify_enterpin", "PIN")
					break
				case "StateEstablishPacePuk":
					authModel.cancelWorkflowOnPinBlocked()
					break
				case "StateCleanUpReaderManager":
					controller.showRemoveCardFeedback = numberModel.cardConnected && !authModel.isError;
					numberModel.continueWorkflow()
					break;
				case "FinalState":
					navBar.lockedAndHidden = true
					if (controller.showRemoveCardFeedback) {
						controller.showRemoveCardFeedback = false
						qmlExtension.showFeedback(qsTr("You may now remove your ID card from the device."))
					}

					if (authModel.error)
						identifyView.push(identifyResult)
					else {
						if (applicationModel.currentWorkflow === "selfauthentication") {
							identifyView.push(selfAuthenticationData)
						} else {
							numberModel.continueWorkflow()
							titleBar.reset()
							identifyView.pop(null)
							navBar.lockedAndHidden = false
						}
					}
					break
				default:
					numberModel.continueWorkflow()
			}
		}
	}

	function setIdentifyWorkflowState(pState) {
		identifyWorkflow.state = statePrefix + pState
	}

	function setIdentifyWorkflowStateAndContinue(pState) {
		setIdentifyWorkflowState(pState);
		numberModel.continueWorkflow()
	}

	function setIdentifyWorkflowStateAndRequestInput(pState, pInput) {
		identifyWorkflow.state = statePrefix + pState
		if (authModel.isBasicReader()) {
			identifyView.push(enterPinView, {state: pInput})
		} else {
			numberModel.continueWorkflow()
		}
	}
}
