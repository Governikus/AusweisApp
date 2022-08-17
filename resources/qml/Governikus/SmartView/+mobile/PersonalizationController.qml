/*
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.AuthView 1.0
import Governikus.EnterPasswordView 1.0
import Governikus.ResultView 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.WhiteListClient 1.0
import Governikus.Workflow 1.0

import Governikus.Type.ConnectivityManager 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.PasswordType 1.0
import Governikus.Type.PersonalizationModel 1.0
import Governikus.Type.ReaderPlugIn 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.SurveyModel 1.0
import Governikus.Type.UiModule 1.0


Controller {
	id: controller

	readonly property string currentState: PersonalizationModel.currentState
	//: LABEL ANDROID IOS
	readonly property string smartEidTitle: qsTr("Smart-eID")
	property int workflowState: PersonalizationController.WorkflowStates.Initial
	property bool workflowProgressVisible: false
	property bool skipSelectReader: false

	enum WorkflowStates {
		Initial,
		CheckStatus,
		InstallApplet,
		TcToken,
		EditRights,
		Reader,
		Pin,
		ProcessingPhysicalEid,
		SmartPinNew,
		Personalization,
		Abort
	}

	Connections {
		target: PersonalizationModel
		function onFireCurrentStateChanged() { processStateChange() }
		// This is necessary because onCurrentStateChanged is not
		// working, when we need to process a state a second time
	}

	Connections {
		target: ConnectivityManager
		enabled: currentState === "StateCheckStatus"
		function onFireNetworkInterfaceActiveChanged() { processStateChange() }
	}

	Component {
		id: transportPinReminder

		TransportPinReminderView {
			title: smartEidTitle
			titleBarColor: Style.color.accent

			onCancel: PersonalizationModel.cancelWorkflow()

			onPinUnknown: {
				pop()
				PersonalizationModel.cancelWorkflowToChangePin()
			}

			onPinKnown: {
				pop()
				PersonalizationModel.continueWorkflow()
			}
		}
	}

	Component {
		id: checkConnectivityView

		CheckConnectivityView {
			titleBarColor: Style.color.accent_smart
			title: smartEidTitle
			onCancel: PersonalizationModel.cancelWorkflow()
		}
	}

	Component {
		id: cardPositionView

		CardPositionView {
			titleBarColor: PersonalizationModel.readerPlugInType === ReaderPlugIn.SMART ? Style.color.accent_smart : Style.color.accent
			title: smartEidTitle

			onCancelClicked: PersonalizationModel.cancelWorkflow()
			onContinueClicked: {
				pop()
				PersonalizationModel.continueWorkflow()
			}
		}
	}

	Component {
		id: smartProgress

		PersonalizationProgressView {
			workflowState: controller.workflowState
			progressBarVisible: workflowProgressVisible

			onAbortWorkflow: {
				controller.workflowState = PersonalizationController.WorkflowStates.Abort
				PersonalizationModel.cancelWorkflow()
			}
		}
	}

	Component {
		id: editRights

		EditRights {
			//: LABEL ANDROID IOS
			title: qsTr("Set up Smart-eID")
			titleBarColor: Style.color.accent_smart
			//: INFO ANDROID IOS The user is informed that the ID card needs to be read to create a Smart-eID.
			actionText: qsTr("The Smart-eID issuing authority needs to read your ID card's data in order to store it on this device:")
			//: LABEL IOS_PHONE ANDROID_PHONE
			dataText: qsTr("By entering your ID card PIN, access to the following data of your ID card will be allowed to the mentioned provider:")
			workflowModel: PersonalizationModel
		}
	}

	Component {
		id: smartWorkflow

		GeneralWorkflow {
			workflowModel: PersonalizationModel
			workflowTitle: smartEidTitle
			titleBarColor: Style.color.accent
		}
	}

	Component {
		id: enterPinView

		EnterPasswordView {
			navigationAction: NavigationAction { action: NavigationAction.Action.Cancel; onClicked: { PersonalizationModel.cancelWorkflow() } }
			//: LABEL ANDROID IOS
			title: qsTr("Set up Smart-eID")
			titleBarColor: workflowState === PersonalizationController.WorkflowStates.Pin ? Style.color.accent : Style.color.accent_smart
			enableTransportPinLink: NumberModel.passwordType === PasswordType.PIN

			onPasswordEntered: {
				replace(smartProgress)
				PersonalizationModel.continueWorkflow()
			}

			onChangePinLength: {
				replace(smartProgress)
				PersonalizationModel.requestTransportPinChange()
			}
		}
	}

	Component {
		id: abortedProgressView

		AbortedProgressView {
			titleBarColor: workflowState === PersonalizationController.WorkflowStates.ProcessingPhysicalEid ? Style.color.accent : Style.color.accent_smart

			onCancel: PersonalizationModel.cancelWorkflow()
		}
	}

	Component {
		id: resultView

		PersonalizationResultView {
			titleBarColor: workflowState === PersonalizationController.WorkflowStates.ProcessingPhysicalEid ? Style.color.accent : Style.color.accent_smart

			onContinueClicked: {
				if (PersonalizationModel.error) {
					done()
				} else {
					replace(legalInformation)
				}
			}
		}
	}

	Component {
		id: legalInformation

		PersonalizationLegalInformationView {
			onContinueClicked: done()
		}
	}

	Component {
		id: whiteListSurveyView

		WhiteListSurveyView {
			titleBarColor: root.titleBarColor

			onDone: {
				SurveyModel.setDeviceSurveyPending(pUserAccepted)
				PersonalizationModel.continueWorkflow()
			}
		}
	}

	function processStateChange() {
		switch (currentState) {
			case "Initial":
				popAll()
				skipSelectReader = false
				break
			case "StateCheckStatus":
				show(UiModule.SMART, true)
				popAll()
				if (!ConnectivityManager.networkInterfaceActive) {
					push(checkConnectivityView)
				} else {
					push(smartProgress)
					setWorkflowStateAndContinue(PersonalizationController.WorkflowStates.CheckStatus)
				}
				break
			case "StatePrepareApplet":
				setWorkflowStateAndContinue(PersonalizationController.WorkflowStates.InstallApplet)
				workflowProgressVisible = true
				break
			case "StateLoadSmartTcTokenUrl":
			case "StateGetTcToken":
				workflowProgressVisible = false
				setWorkflowStateAndContinue(PersonalizationController.WorkflowStates.TcToken)
				break
			case "StatePreVerification":
				if (!NumberModel.isCanAllowedMode && SettingsModel.transportPinReminder) {
					SettingsModel.transportPinReminder = false
					push(transportPinReminder)
				} else {
					PersonalizationModel.continueWorkflow()
				}
				break
			case "StateEditAccessRights":
				workflowState = PersonalizationController.WorkflowStates.EditRights
				replace(editRights)
				PersonalizationModel.setInitialPluginType()
				break
			case "StateSelectReader":
				if (skipSelectReader) {
					PersonalizationModel.continueWorkflow()
					break
				}

				setWorkflowStateAndContinue(PersonalizationController.WorkflowStates.Reader)
				if (!workflowActive) {
					replace(smartWorkflow)
				}
				break
			case "StateEnterPacePassword":
				setWorkflowStateAndRequestInput(PersonalizationController.WorkflowStates.Pin)
				break
			case "StateEstablishPaceChannel":
				if (skipSelectReader) {
					PersonalizationModel.continueWorkflow()
					break
				}

				setWorkflowStateAndContinue(PersonalizationController.WorkflowStates.ProcessingPhysicalEid)
				replace(smartProgress)
				break
			case "StateUnfortunateCardPosition":
				push(cardPositionView)
				break
			case "StateDidAuthenticateEac1":
				setWorkflowStateAndContinue(PersonalizationController.WorkflowStates.ProcessingPhysicalEid)
				workflowProgressVisible = true
				break
			case "StateSendDIDAuthenticateResponseEAC1":
				if (PersonalizationModel.isCancellationByUser()) {
					replace(abortedProgressView)
				}
				PersonalizationModel.continueWorkflow()
				break
			case "StateGetSessionId":
				skipSelectReader = true
				showRemoveCardFeedback(PersonalizationModel, false)
				PersonalizationModel.continueWorkflow()
				break
			case "StateEnterNewPacePin":
				setWorkflowStateAndRequestInput(PersonalizationController.WorkflowStates.SmartPinNew)
				break
			case "StateGetChallenge":
				setWorkflowStateAndContinue(PersonalizationController.WorkflowStates.Personalization)
				replace(smartProgress)
				break
			case "StateShowResult":
				if (!PersonalizationModel.error)
					replace(resultView)
				else
					PersonalizationModel.continueWorkflow()
				break
			case "StateSendWhitelistSurvey":
				if (SurveyModel.askForDeviceSurvey()) {
					replace(whiteListSurveyView)
				} else {
					PersonalizationModel.continueWorkflow()
				}
				break
			case "FinalState":
				if (PersonalizationModel.showChangePinView) {
					done()
					show(UiModule.PINMANAGEMENT)
				} else if (PersonalizationModel.error && !PersonalizationModel.hasNextWorkflowPending && !PersonalizationModel.shouldSkipResultView()) {
					replace(resultView)
				} else {
					done()
				}
				workflowProgressVisible = false
				break
			default:
				PersonalizationModel.continueWorkflow()
		}
	}

	function setWorkflowStateAndContinue(pState) {
		controller.workflowState = pState
		PersonalizationModel.continueWorkflow()
	}

	function setWorkflowStateAndRequestInput(pState) {
		controller.workflowState = pState
		if (PersonalizationModel.isBasicReader) {
			replace(enterPinView)
		} else {
			replace(smartProgress)
			PersonalizationModel.continueWorkflow()
		}
	}

	function done() {
		PersonalizationModel.continueWorkflow()
		popAll()
		show(UiModule.SMART)
	}
}
