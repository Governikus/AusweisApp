/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

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
import Governikus.Type.AuthModel 1.0
import Governikus.Type.ChangePinModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.ConnectivityManager 1.0
import Governikus.Type.UiModule 1.0

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
		CardPosition,
		InputError,
		Data,
		Result,
		ReturnToMain,
		ReaderSettings,
		Aborting
	}

	function showWithPrecedingView(pNextView) {
		d.precedingView = d.activeView
		d.view = pNextView
		ApplicationWindow.menuBar.updateActions()
	}

	function showPasswordInfo() {
		showWithPrecedingView(IdentifyView.SubViews.PasswordInfo)
	}

	function showSettings() {
		showWithPrecedingView(IdentifyView.SubViews.ReaderSettings)
	}

	activeFocusOnTab: true
	Keys.onEscapePressed: if (d.cancelAllowed) AuthModel.cancelWorkflow()

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP
		text: qsTr("Identify")
		rootEnabled: false
		helpTopic: "authentication"
		showSettings: (identifyController.workflowState === IdentifyController.WorkflowStates.Reader ||
		               identifyController.workflowState === IdentifyController.WorkflowStates.Card)
		              && d.activeView !== IdentifyView.SubViews.Progress

		onClicked: {
			editRights.showProviderInformation(false)
			if (d.activeView === IdentifyView.SubViews.PasswordInfo) {
				d.view = SettingsModel.transportPinReminder ? IdentifyView.SubViews.TransportPinReminder : IdentifyView.SubViews.Password
				ApplicationWindow.menuBar.updateActions()
			}
			else if (d.activeView === IdentifyView.SubViews.ReaderSettings) {
				d.view = d.precedingView
				ApplicationWindow.menuBar.updateActions()
			}
		}

		customSubAction: CancelAction {
			visible: d.cancelAllowed

			onClicked: {
				if (identifyResult.visible) {
					AuthModel.continueWorkflow()
					identifyView.nextView(UiModule.DEFAULT)
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
		property int precedingView: IdentifyView.SubViews.Undefined
		readonly property int activeView: inputError.visible ? IdentifyView.SubViews.InputError : view
		readonly property bool cancelAllowed: AuthModel.isBasicReader || generalWorkflow.waitingFor != Workflow.WaitingFor.Password
	}

	TabbedReaderView {
		visible: d.activeView === IdentifyView.SubViews.ReaderSettings
		onCloseView: {
			d.view = d.precedingView
			ApplicationWindow.menuBar.updateActions()
		}
	}

	IdentifyController {
		id: identifyController

		onNextView: {
			if (pName === IdentifyView.SubViews.ReturnToMain) {
				identifyView.nextView(UiModule.DEFAULT)
				return;
			}

			d.view = pName
		}
	}

	DecisionView {
		visible: d.activeView === IdentifyView.SubViews.TransportPinReminder

		questionText: qsTr("Do you know your six-digit PIN?")
		questionSubText: "%1<br><br><a href=\"#\">%2</a>".arg(qsTr("The personal, six-digit PIN is mandatory to use the online identification function.")).arg(qsTr("More information"))

		onSubTextLinkActivated: showPasswordInfo()
		onDisagree: {
			SettingsModel.transportPinReminder = false
			AuthModel.cancelWorkflowToChangePin()
			identifyView.nextView(UiModule.PINMANAGEMENT)
		}
		onAgree: SettingsModel.transportPinReminder = false // causes fall-through to next state in IdentifyController
	}

	ProgressView {
		id: checkConnectivityView

		visible: d.activeView === IdentifyView.SubViews.Connectivity

		//: INFO DESKTOP Header of the message that no network connection is present during the authentication procedure.
		text: qsTr("No network connectivity")
		//: INFO DESKTOP Content of the message that no network connection is present during the authentication procedure.
		subText: qsTr("Please establish an internet connection.")
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
		onRequestPasswordInfo: identifyView.showPasswordInfo()
	}

	EnterPasswordView {
		id: enterPasswordView

		visible: d.activeView === IdentifyView.SubViews.Password

		enableTransportPinLink: passwordType === NumberModel.PASSWORD_PIN
		onPasswordEntered: {
			d.view = IdentifyView.SubViews.Progress
			AuthModel.continueWorkflow()
		}
		onRequestPasswordInfo: identifyView.showPasswordInfo()
		onChangePinLength: AuthModel.requestTransportPinChange()
	}

	PasswordInfoView {
		id: passwordInfoView

		visible: d.activeView === IdentifyView.SubViews.PasswordInfo

		onClose: {
			d.view = d.precedingView
			ApplicationWindow.menuBar.updateActions()
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
		//: INFO DESKTOP A weak NFC signal was detected since the card communication was aborted. The card's position needs to be adjusted to hopefully achieve better signal strength.
		text: qsTr("Weak NFC signal. Please\n- change the card position\n- remove the mobile phone case (if present)\n- connect the smartphone with a charging cable")
		onNextView: AuthModel.continueWorkflow()
	}

	ProgressView {
		visible: d.activeView === IdentifyView.SubViews.Aborting

		//: INFO DESKTOP The user aborted the authentication process, according to TR we need to inform the service provider
		text: qsTr("Aborting process and informing the service provider")
		subText: {
			if (ConnectivityManager.networkInterfaceActive) {
				//: INFO DESKTOP Information message about cancellation process with present network connectivity
				return qsTr("Please wait a moment.")
			}
			//: INFO DESKTOP Information message about cancellation process without working network connectivity
			return qsTr("Network problems detected, trying to reach server within 30 seconds.")
		}
		progressBarVisible: false
		subTextColor: !ConnectivityManager.networkInterfaceActive ? Style.color.warning_text : Style.color.secondary_text_inverse
	}

	ProgressView {
		id: identifyProgressView

		readonly property bool inProgress: identifyController.workflowState !== IdentifyController.WorkflowStates.Initial

		visible: d.activeView === IdentifyView.SubViews.Progress

		text: (inProgress ?
			   //: INFO DESKTOP Header of the progress information during the authentication process.
			   qsTr("Authentication in progress") :
			   //: INFO DESKTOP Header of the progress information during the authentication process.
			   qsTr("Acquiring provider certificate")
			  )
		subText: {
			if (!visible) {
				return ""
			}
			if (!inProgress || AuthModel.error) {
				//: INFO DESKTOP Generic progress status message while no card communication is active.
				return qsTr("Please wait a moment.")
			}
			if (AuthModel.isBasicReader) {
				//: INFO DESKTOP Second line text if a basic card reader is used and data is exchanged with the card/server in the background. Is not actually visible since the basic reader password handling is done by EnterPasswordView.
				return qsTr("Please do not move the ID card.")
			}
			if (!!NumberModel.inputError) {
				return NumberModel.inputError
			}
			//: INFO DESKTOP Generic progress status message during authentication.
			return qsTr("Please wait a moment.")
		}
		subTextColor: !AuthModel.isBasicReader && NumberModel.inputError ? Style.color.warning_text : Style.color.secondary_text_inverse
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
		header: AuthModel.errorHeader
		hintText: AuthModel.statusHintText
		hintButtonText: AuthModel.statusHintActionText
		//: INFO DESKTOP Error code (string) of current GlobalStatus code, shown as header of popup.
		popupTitle: qsTr("Error code: %1").arg(AuthModel.statusCodeString)
		popupText: AuthModel.errorText
		supportButtonsVisible: AuthModel.errorIsMasked

		onNextView: {
			identifyView.nextView(pName)
			AuthModel.continueWorkflow()
		}
		onEmailButtonPressed: AuthModel.sendResultMail()
		onHintClicked: AuthModel.invokeStatusHintAction()
	}
}
