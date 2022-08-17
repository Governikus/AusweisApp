/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15

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
import Governikus.Type.PasswordType 1.0
import Governikus.Type.ConnectivityManager 1.0
import Governikus.Type.UiModule 1.0

SectionPage {
	id: authView

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
		updateTitleBarActions()
	}

	function showPasswordInfo() {
		showWithPrecedingView(AuthView.SubViews.PasswordInfo)
	}

	function showSettings() {
		showWithPrecedingView(AuthView.SubViews.ReaderSettings)
	}

	Keys.onEscapePressed: if (d.cancelAllowed) AuthModel.cancelWorkflow()

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP
		text: qsTr("Identify")
		rootEnabled: false
		helpTopic: "authentication"
		showSettings: authController.workflowState === AuthController.WorkflowStates.Reader

		onClicked: {
			editRights.showProviderInformation(false)
			if (d.activeView === AuthView.SubViews.PasswordInfo) {
				d.view = SettingsModel.transportPinReminder ? AuthView.SubViews.TransportPinReminder : AuthView.SubViews.Password
				updateTitleBarActions()
			}
			else if (d.activeView === AuthView.SubViews.ReaderSettings) {
				d.view = d.precedingView
				updateTitleBarActions()
			}
		}

		customSubAction: CancelAction {
			visible: d.cancelAllowed

			onClicked: {
				if (authResult.visible) {
					AuthModel.continueWorkflow()
					authView.nextView(UiModule.DEFAULT)
				} else {
					AuthModel.cancelWorkflow()
				}
			}
		}

		customSettingsHandler: authView.showSettings
	}

	QtObject {
		id: d

		property int view: AuthView.SubViews.Undefined
		property int precedingView: AuthView.SubViews.Undefined
		readonly property int activeView: inputError.visible ? AuthView.SubViews.InputError : view
		readonly property bool cancelAllowed: AuthModel.isBasicReader || generalWorkflow.waitingFor !== Workflow.WaitingFor.Password
	}

	TabbedReaderView {
		visible: d.activeView === AuthView.SubViews.ReaderSettings
		onCloseView: {
			d.view = d.precedingView
			updateTitleBarActions()
		}
	}

	AuthController {
		id: authController

		onNextView: pName => {
			if (pName === AuthView.SubViews.ReturnToMain) {
				if (AuthModel.showChangePinView) {
					authView.nextView(UiModule.PINMANAGEMENT)
				} else {
					authView.nextView(UiModule.DEFAULT)
				}
				return;
			}

			d.view = pName
		}
	}

	DecisionView {
		visible: d.activeView === AuthView.SubViews.TransportPinReminder

		questionText: qsTr("Do you know your six-digit PIN?")
		questionSubText: qsTr("The personal, six-digit PIN is mandatory to use the online identification function.")
		moreInformationVisible: true

		onMoreInformationClicked: showPasswordInfo()
		onDisagree: AuthModel.cancelWorkflowToChangePin()
		onAgree: {
			d.view = AuthView.SubViews.Progress
			AuthModel.continueWorkflow()
		}
	}

	ProgressView {
		id: checkConnectivityView

		visible: d.activeView === AuthView.SubViews.Connectivity

		//: INFO DESKTOP Header of the message that no network connection is present during the authentication procedure.
		text: qsTr("No network connectivity")
		//: INFO DESKTOP Content of the message that no network connection is present during the authentication procedure.
		subText: qsTr("Please establish an internet connection.")
		subTextColor: Constants.red
	}

	EditRights {
		id: editRights

		visible: d.activeView === AuthView.SubViews.AccessRights
	}

	GeneralWorkflow {
		id: generalWorkflow

		visible: d.activeView === AuthView.SubViews.Workflow

		isPinChange: false
		waitingFor: switch (authController.workflowState) {
						case AuthController.WorkflowStates.Reader:
							return Workflow.WaitingFor.Reader
						case AuthController.WorkflowStates.Password:
							return Workflow.WaitingFor.Password
						default:
							return Workflow.WaitingFor.None
		}
		onSettingsRequested: authView.showSettings()
		onRequestPasswordInfo: authView.showPasswordInfo()
	}

	EnterPasswordView {
		id: enterPasswordView

		visible: d.activeView === AuthView.SubViews.Password

		enableTransportPinLink: passwordType === PasswordType.PIN
		onPasswordEntered: {
			d.view = AuthView.SubViews.Progress
			AuthModel.continueWorkflow()
		}

		onRequestPasswordInfo: authView.showPasswordInfo()
		onChangePinLength: AuthModel.requestTransportPinChange()
	}

	PasswordInfoView {
		id: passwordInfoView

		visible: d.activeView === AuthView.SubViews.PasswordInfo

		onClose: {
			d.view = d.precedingView
			updateTitleBarActions()
		}
	}

	ResultView {
		id: inputError

		property bool errorConfirmed: false

		visible: !errorConfirmed && NumberModel.hasPasswordError && d.view !== AuthView.SubViews.Result

		resultType: ResultView.Type.IsError
		text: NumberModel.inputError
		onNextView: errorConfirmed = true

		Connections {
			target: NumberModel
			function onFireInputErrorChanged() { inputError.errorConfirmed = false }
		}
	}

	ResultView {
		id: cardPositionView

		visible: d.activeView === AuthView.SubViews.CardPosition

		resultType: ResultView.Type.IsInfo
		text: AuthModel.isRemoteReader ?
			//: INFO DESKTOP A weak NFC signal was detected since the card communication was aborted. The card's position needs to be adjusted to hopefully achieve better signal strength.
			qsTr("Weak NFC signal. Please\n- change the card position\n- remove the mobile phone case (if present)\n- connect the smartphone with a charging cable") :
			//: INFO DESKTOP The NFC signal is weak or unstable, while using a stationary card reader.
			qsTr("Weak NFC signal. Please\n- make sure the card is positioned correctly on the reader\n- do note move the card while it is being accessed")
		onNextView: AuthModel.continueWorkflow()
	}

	ProgressView {
		visible: d.activeView === AuthView.SubViews.Aborting

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
		subTextColor: !ConnectivityManager.networkInterfaceActive ? Style.color.warning_text : Style.color.secondary_text
	}

	ProgressView {
		readonly property bool isInitialState: authController.workflowState === AuthController.WorkflowStates.Initial

		visible: d.activeView === AuthView.SubViews.Progress

		text: (isInitialState ?
			   //: INFO DESKTOP Header of the progress information during the authentication process.
			   qsTr("Acquiring provider certificate") :
			   //: INFO DESKTOP Header of the progress information during the authentication process.
			   qsTr("Authentication in progress")
			  )
		subText: {
			if (!visible) {
				return ""
			}
			if (isInitialState || AuthModel.error) {
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
		subTextColor: !AuthModel.isBasicReader && NumberModel.inputError ? Style.color.warning_text : Style.color.secondary_text
		progressValue: AuthModel.progressValue
		progressText: AuthModel.progressMessage
		progressBarVisible: authController.workflowProgressVisible
	}

	SelfAuthenticationData {
		visible: d.activeView === AuthView.SubViews.Data

		onVisibleChanged: updateTitleBarActions()
		onNextView: pName => {
			authView.nextView(pName)
			AuthModel.continueWorkflow()
		}
	}

	ResultView {
		id: authResult

		visible: d.activeView === AuthView.SubViews.Result

		resultType: AuthModel.resultString ? ResultView.Type.IsError : ResultView.Type.IsSuccess
		text: AuthModel.resultString
		header: AuthModel.errorHeader
		hintText: AuthModel.statusHintText
		hintButtonText: AuthModel.statusHintActionText
		//: INFO DESKTOP Error code (string) of current GlobalStatus code, shown as header of popup.
		popupTitle: qsTr("Error code: %1").arg(AuthModel.statusCodeString)
		popupText: AuthModel.errorText
		supportButtonsVisible: AuthModel.errorIsMasked

		onNextView: pName => {
			authView.nextView(pName)
			AuthModel.continueWorkflow()
		}
		onEmailButtonPressed: AuthModel.sendResultMail()
		onHintClicked: AuthModel.invokeStatusHintAction()
	}
}
