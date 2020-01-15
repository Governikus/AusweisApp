/*
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.EnterPasswordView 1.0
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.ProgressView 1.0
import Governikus.ResultView 1.0
import Governikus.SettingsView 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Workflow 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ChangePinModel 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.NumberModel 1.0


SectionPage
{
	id: identifyView

	enum SubViews {
		Undefined,
		TransportPinReminder,
		Connectivity,
		Progress,
		AccessRights,
		Workflow,
		Password,
		PasswordInfo,
		PasswordLength,
		CardPosition,
		InputError,
		Data,
		Result,
		ReturnToMain,
		ReaderSettings,
		Aborting
	}

	function showSettings() {
		readerView.precedingView = d.activeView
		d.view = IdentifyView.SubViews.ReaderSettings
		ApplicationWindow.menuBar.updateActions()
	}

	activeFocusOnTab: true
	Keys.onEscapePressed: if (d.cancelAllowed) AuthModel.cancelWorkflow()

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP_QML
		text: qsTr("Identify") + SettingsModel.translationTrigger
		rootEnabled: false
		helpTopic: "authentication"
		showSettings: (identifyController.workflowState === IdentifyController.WorkflowStates.Initial ||
		               identifyController.workflowState === IdentifyController.WorkflowStates.Reader ||
		               identifyController.workflowState === IdentifyController.WorkflowStates.Card)
		              && d.activeView !== IdentifyView.SubViews.Progress

		onClicked: {
			editRights.showProviderInformation(false)
			if (d.activeView === IdentifyView.SubViews.PasswordInfo || d.activeView === IdentifyView.SubViews.PasswordLength) {
				d.view = IdentifyView.SubViews.Password
				ApplicationWindow.menuBar.updateActions()
			}
			else if (d.activeView === IdentifyView.SubViews.ReaderSettings) {
				d.view = readerView.precedingView
				ApplicationWindow.menuBar.updateActions()
			}
		}

		customSubAction: CancelAction {
			visible: d.cancelAllowed

			onClicked: {
				if (identifyResult.visible) {
					AuthModel.continueWorkflow()
					identifyView.nextView(SectionPage.Views.Main)
				} else {
					AuthModel.cancelWorkflow()
				}
			}
		}

		customSettingsHandler: identifyView.showSettings
	}

	QtObject {
		id: d

		property int view: IdentifyView.SubViews.Undefined
		readonly property int activeView: inputError.visible ? IdentifyView.SubViews.InputError : view
		readonly property bool cancelAllowed: AuthModel.isBasicReader || generalWorkflow.waitingFor != Workflow.WaitingFor.Password
	}

	TabbedReaderView {
		id: readerView

		property int precedingView: IdentifyView.SubViews.Undefined

		visible: d.activeView === IdentifyView.SubViews.ReaderSettings
		onCloseView: {
			d.view = precedingView
			ApplicationWindow.menuBar.updateActions()
		}
	}

	IdentifyController {
		id: identifyController

		onNextView: {
			if (pName === IdentifyView.SubViews.ReturnToMain) {
				identifyView.nextView(SectionPage.Views.Main)
				return;
			}

			d.view = pName
		}
	}

	BinaryDecisionView {
		visible: d.activeView === IdentifyView.SubViews.TransportPinReminder

		questionText: qsTr("Did you change the transport PIN already?") + SettingsModel.translationTrigger
		questionSubText: qsTr("Prior to the first use of the online identification function you have to replace the transport PIN by an individual 6-digit PIN. Online identification with transport PIN is not possible.") + SettingsModel.translationTrigger
		disagreeText: qsTr("No, change transport PIN now") + SettingsModel.translationTrigger

		onDisagree: {
			SettingsModel.transportPinReminder = false
			NumberModel.requestTransportPin = true
			ChangePinModel.startWorkflow()
			AuthModel.cancelWorkflowToChangePin()
		}
		onAgree: SettingsModel.transportPinReminder = false // causes fall-through to next state in IdentifyController
	}

	ProgressView {
		id: checkConnectivityView

		visible: d.activeView === IdentifyView.SubViews.Connectivity

		//: INFO DESKTOP_QML Header of the message that no network connection is present during the authentication procedure.
		text: qsTr("No network connectivity") + SettingsModel.translationTrigger
		//: INFO DESKTOP_QML Content of the message that no network connection is present during the authentication procedure.
		subText: qsTr("Please establish an internet connection.") + SettingsModel.translationTrigger
		subTextColor: Constants.red
	}

	EditRights {
		id: editRights

		visible: d.activeView === IdentifyView.SubViews.AccessRights
	}

	GeneralWorkflow {
		id: generalWorkflow

		visible: d.activeView === IdentifyView.SubViews.Workflow

		isPinChange: false
		waitingFor: switch (identifyController.workflowState) {
						case IdentifyController.WorkflowStates.Reader:
							return Workflow.WaitingFor.Reader
						case IdentifyController.WorkflowStates.Card:
							return Workflow.WaitingFor.Card
						case IdentifyController.WorkflowStates.Password:
							return Workflow.WaitingFor.Password
						default:
							return Workflow.WaitingFor.None
		}
		onSettingsRequested: identifyView.showSettings()
	}

	EnterPasswordView {
		id: enterPasswordView

		visible: d.activeView === IdentifyView.SubViews.Password

		onPasswordEntered: {
			d.view = IdentifyView.SubViews.Progress
			AuthModel.continueWorkflow()
		}

		onChangePinLength: {
			d.view = IdentifyView.SubViews.PasswordLength
			ApplicationWindow.menuBar.updateActions()
		}

		onRequestPasswordInfo: {
			d.view = IdentifyView.SubViews.PasswordInfo
			ApplicationWindow.menuBar.updateActions()
		}
	}

	PasswordInfoView {
		id: passwordInfoView

		visible: d.activeView === IdentifyView.SubViews.PasswordInfo

		onClose: {
			d.view = IdentifyView.SubViews.Password
			ApplicationWindow.menuBar.updateActions()
		}
	}

	ResultView {
		id: passwordLengthView

		visible: d.activeView === IdentifyView.SubViews.PasswordLength

		titleBarAction: TitleBarAction {
			//: LABEL DESKTOP_QML
			text: qsTr("Transport PIN") + SettingsModel.translationTrigger
			rootEnabled: false
			helpTopic: "pinEntry"
			customSubAction: CancelAction {
				onClicked:  {
					d.view = IdentifyView.SubViews.Password
					ApplicationWindow.menuBar.updateActions()
				}
			}
		}
		resultType: ResultView.Type.IsInfo
		//: INFO DESKTOP_QML The user clicked that the current PIN has 5 digits (transport PIN) which needs to be changed to a 6-digit PIN. The current process will be aborted and needs to be restarted *manually* by the user.
		text: qsTr("First you have to change your 5-digit transport PIN you received in your in PIN letter into a 6-digit PIN. You are currently leaving the started process and are forwarded to the PIN management. Please restart the desired process after the PIN has been changed.") + SettingsModel.translationTrigger
		onNextView: {
			ChangePinModel.startWorkflow()
			AuthModel.cancelWorkflowToChangePin()
		}
	}

	ResultView {
		id: inputError

		property bool errorConfirmed: false

		visible: !errorConfirmed && NumberModel.hasPasswordError && d.view != IdentifyView.SubViews.Result

		resultType: ResultView.Type.IsError
		text: NumberModel.inputError
		onNextView: errorConfirmed = true

		Connections {
			target: NumberModel
			onFireInputErrorChanged: inputError.errorConfirmed = false
		}
	}

	ResultView {
		id: cardPositionView

		visible: d.activeView === IdentifyView.SubViews.CardPosition

		resultType: ResultView.Type.IsInfo
		//: INFO DESKTOP_QML A weak NFC signal was detected since the card communication was aborted. The card's position needs to be adjusted to hopefully achieve better signal strength.
		text: qsTr("Weak NFC signal. Please\n- change the card position\n- remove the mobile phone case (if present)\n- connect the smartphone with a charging cable") + SettingsModel.translationTrigger
		onNextView: AuthModel.continueWorkflow()
	}

	ProgressView {
		visible: d.activeView === IdentifyView.SubViews.Aborting

		//: INFO DESKTOP_QML The user aborted the authentication process, according to TR we need to inform the service provider
		text: qsTr("Aborting process and informing the service provider") + SettingsModel.translationTrigger
		subText: {
			SettingsModel.translationTrigger
			if (connectivityManager.networkInterfaceActive) {
				//: INFO DESKTOP_QML Information message about cancellation process with present network connectivity
				return qsTr("Please wait a moment...")
			}
			//: INFO DESKTOP_QML Information message about cancellation process without working network connectivity
			return qsTr("Network problemes detected, trying to reach server within 30 seconds.")
		}
		progressBarVisible: false
		subTextColor: !connectivityManager.networkInterfaceActive ? Style.color.warning_text : Style.color.secondary_text_inverse
	}

	ProgressView {
		id: identifyProgressView

		readonly property bool inProgress: identifyController.workflowState !== IdentifyController.WorkflowStates.Initial

		visible: d.activeView === IdentifyView.SubViews.Progress

		text: (inProgress ?
			   //: INFO DESKTOP_QML Header of the progress information during the authentication process.
			   qsTr("Authentication in progress") :
			   //: INFO DESKTOP_QML Header of the progress information during the authentication process.
			   qsTr("Acquiring provider certificate")
			  ) + SettingsModel.translationTrigger
		subText: {
			SettingsModel.translationTrigger;
			if (!visible) {
				return ""
			}
			if (!inProgress || AuthModel.error) {
				//: INFO DESKTOP_QML Generic progress status message while no card communication is active.
				return qsTr("Please wait a moment...")
			}
			if (AuthModel.isBasicReader) {
				//: INFO DESKTOP_QML Second line text if a basic card reader is used and data is exchanged with the card/server in the background. Is not actually visible since the basic reader password handling is done by EnterPasswordView.
				return qsTr("Please don't move the ID card...")
			}
			if (!!NumberModel.inputError) {
				return NumberModel.inputError
			}
			if (NumberModel.pinDeactivated) {
				//: INFO DESKTOP_QML The online authentication feature of the ID card is deactivated and needs to be activated by the local authorities.
				return qsTr("The online identification function of your ID card is not activated. Please contact the authority responsible for issuing your identification card to activate the online identification function.")
			}
			//: INFO DESKTOP_QML Generic progress status message during authentication.
			return qsTr("Please wait a moment...")
		}
		subTextColor: !AuthModel.isBasicReader && (NumberModel.inputError || NumberModel.pinDeactivated) ? Style.color.warning_text : Style.color.secondary_text_inverse
		progressValue: AuthModel.progressValue
		progressText: AuthModel.progressMessage
		progressBarVisible: identifyController.workflowProgressVisible
	}

	SelfAuthenticationData {
		visible: d.activeView === IdentifyView.SubViews.Data

		onVisibleChanged: ApplicationWindow.menuBar.updateActions()
		onNextView: {
			identifyView.nextView(pName)
			AuthModel.continueWorkflow()
		}
	}

	ResultView {
		id: identifyResult

		visible: d.activeView === IdentifyView.SubViews.Result

		resultType: AuthModel.resultString ? ResultView.Type.IsError : ResultView.Type.IsSuccess
		text: AuthModel.resultString
		onNextView: {
			identifyView.nextView(pName)
			AuthModel.continueWorkflow()
		}
		emailButtonVisible: AuthModel.error && !AuthModel.isCancellationByUser()
		onEmailButtonPressed: AuthModel.sendResultMail()
	}
}
