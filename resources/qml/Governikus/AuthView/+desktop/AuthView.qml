/*
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
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

	function showPasswordInfo() {
		showWithPrecedingView(AuthView.SubViews.PasswordInfo);
	}
	function showSettings() {
		showWithPrecedingView(AuthView.SubViews.ReaderSettings);
	}
	function showWithPrecedingView(pNextView) {
		d.precedingView = d.activeView;
		d.view = pNextView;
		updateTitleBarActions();
	}

	titleBarAction: TitleBarAction {
		customSettingsHandler: authView.showSettings
		helpTopic: "authentication"
		rootEnabled: false
		showSettings: authController.workflowState === AuthController.WorkflowStates.Reader
		//: LABEL DESKTOP
		text: qsTr("Identify")

		customSubAction: CancelAction {
			visible: d.cancelAllowed

			onClicked: {
				if (authResult.visible) {
					AuthModel.continueWorkflow();
					authView.nextView(UiModule.DEFAULT);
				} else {
					AuthModel.cancelWorkflow();
				}
			}
		}

		onClicked: {
			editRights.showProviderInformation(false);
			if (d.activeView === AuthView.SubViews.PasswordInfo) {
				d.view = SettingsModel.transportPinReminder ? AuthView.SubViews.TransportPinReminder : AuthView.SubViews.Password;
				updateTitleBarActions();
			} else if (d.activeView === AuthView.SubViews.ReaderSettings) {
				d.view = d.precedingView;
				updateTitleBarActions();
			}
		}
	}

	Keys.onEscapePressed: if (d.cancelAllowed)
		AuthModel.cancelWorkflow()

	QtObject {
		id: d

		readonly property int activeView: inputError.visible ? AuthView.SubViews.InputError : view
		readonly property bool cancelAllowed: AuthModel.isBasicReader || generalWorkflow.waitingFor !== Workflow.WaitingFor.Password
		property int precedingView: AuthView.SubViews.Undefined
		property int view: AuthView.SubViews.Undefined
	}
	TabbedReaderView {
		visible: d.activeView === AuthView.SubViews.ReaderSettings

		onCloseView: {
			d.view = d.precedingView;
			updateTitleBarActions();
		}
	}
	AuthController {
		id: authController
		onNextView: pName => {
			if (pName === AuthView.SubViews.ReturnToMain) {
				if (AuthModel.showChangePinView) {
					authView.nextView(UiModule.PINMANAGEMENT);
				} else {
					authView.nextView(UiModule.DEFAULT);
				}
				return;
			}
			d.view = pName;
		}
	}
	DecisionView {
		moreInformationVisible: true
		questionSubText: qsTr("The personal, six-digit PIN is mandatory to use the online identification function.")
		questionText: qsTr("Do you know your six-digit PIN?")
		visible: d.activeView === AuthView.SubViews.TransportPinReminder

		onAgree: {
			d.view = AuthView.SubViews.Progress;
			AuthModel.continueWorkflow();
		}
		onDisagree: AuthModel.cancelWorkflowToChangePin()
		onMoreInformationClicked: showPasswordInfo()
	}
	ProgressView {
		id: checkConnectivityView
		//: INFO DESKTOP Content of the message that no network connection is present during the authentication procedure.
		subText: qsTr("Please establish an internet connection.")
		subTextColor: Constants.red

		//: INFO DESKTOP Header of the message that no network connection is present during the authentication procedure.
		text: qsTr("No network connectivity")
		visible: d.activeView === AuthView.SubViews.Connectivity
	}
	EditRights {
		id: editRights
		visible: d.activeView === AuthView.SubViews.AccessRights
	}
	GeneralWorkflow {
		id: generalWorkflow
		isPinChange: false
		visible: d.activeView === AuthView.SubViews.Workflow
		waitingFor: switch (authController.workflowState) {
		case AuthController.WorkflowStates.Reader:
			return Workflow.WaitingFor.Reader;
		case AuthController.WorkflowStates.Password:
			return Workflow.WaitingFor.Password;
		default:
			return Workflow.WaitingFor.None;
		}

		onRequestPasswordInfo: authView.showPasswordInfo()
		onSettingsRequested: authView.showSettings()
	}
	EnterPasswordView {
		id: enterPasswordView
		enableTransportPinLink: passwordType === PasswordType.PIN
		visible: d.activeView === AuthView.SubViews.Password

		onChangePinLength: AuthModel.requestTransportPinChange()
		onPasswordEntered: {
			d.view = AuthView.SubViews.Progress;
			AuthModel.continueWorkflow();
		}
		onRequestPasswordInfo: authView.showPasswordInfo()
	}
	PasswordInfoView {
		id: passwordInfoView
		visible: d.activeView === AuthView.SubViews.PasswordInfo

		onClose: {
			d.view = d.precedingView;
			updateTitleBarActions();
		}
	}
	ResultView {
		id: inputError

		property bool errorConfirmed: false

		resultType: ResultView.Type.IsError
		text: NumberModel.inputError
		visible: !errorConfirmed && NumberModel.hasPasswordError && d.view !== AuthView.SubViews.Result

		onNextView: errorConfirmed = true

		Connections {
			function onFireInputErrorChanged() {
				inputError.errorConfirmed = false;
			}

			target: NumberModel
		}
	}
	ResultView {
		id: cardPositionView
		resultType: ResultView.Type.IsInfo
		text: AuthModel.isRemoteReader ?
		//: INFO DESKTOP A weak NFC signal was detected since the card communication was aborted. The card's position needs to be adjusted to hopefully achieve better signal strength.
		qsTr("Weak NFC signal. Please\n- change the card position\n- remove the mobile phone case (if present)\n- connect the smartphone with a charging cable") :
		//: INFO DESKTOP The NFC signal is weak or unstable, while using a stationary card reader.
		qsTr("Weak NFC signal. Please\n- make sure the card is positioned correctly on the reader\n- do note move the card while it is being accessed")
		visible: d.activeView === AuthView.SubViews.CardPosition

		onNextView: AuthModel.continueWorkflow()
	}
	ProgressView {
		progressBarVisible: false
		subText: {
			if (ConnectivityManager.networkInterfaceActive) {
				//: INFO DESKTOP Information message about cancellation process with present network connectivity
				return qsTr("Please wait a moment.");
			}
			//: INFO DESKTOP Information message about cancellation process without working network connectivity
			return qsTr("Network problems detected, trying to reach server within 30 seconds.");
		}
		subTextColor: !ConnectivityManager.networkInterfaceActive ? Style.color.warning_text : Style.color.secondary_text

		//: INFO DESKTOP The user aborted the authentication process, according to TR we need to inform the service provider
		text: qsTr("Aborting process and informing the service provider")
		visible: d.activeView === AuthView.SubViews.Aborting
	}
	ProgressView {
		readonly property bool isInitialState: authController.workflowState === AuthController.WorkflowStates.Initial

		progressBarVisible: authController.workflowProgressVisible
		progressText: AuthModel.progressMessage
		progressValue: AuthModel.progressValue
		subText: {
			if (!visible) {
				return "";
			}
			if (isInitialState || AuthModel.error) {
				//: INFO DESKTOP Generic progress status message while no card communication is active.
				return qsTr("Please wait a moment.");
			}
			if (AuthModel.isBasicReader) {
				//: INFO DESKTOP Second line text if a basic card reader is used and data is exchanged with the card/server in the background. Is not actually visible since the basic reader password handling is done by EnterPasswordView.
				return qsTr("Please do not move the ID card.");
			}
			if (!!NumberModel.inputError) {
				return NumberModel.inputError;
			}
			//: INFO DESKTOP Generic progress status message during authentication.
			return qsTr("Please wait a moment.");
		}
		subTextColor: !AuthModel.isBasicReader && NumberModel.inputError ? Style.color.warning_text : Style.color.secondary_text
		text: (isInitialState ?
			//: INFO DESKTOP Header of the progress information during the authentication process.
			qsTr("Acquiring provider certificate") :
			//: INFO DESKTOP Header of the progress information during the authentication process.
			qsTr("Authentication in progress"))
		visible: d.activeView === AuthView.SubViews.Progress
	}
	SelfAuthenticationData {
		visible: d.activeView === AuthView.SubViews.Data

		onNextView: pName => {
			authView.nextView(pName);
			AuthModel.continueWorkflow();
		}
		onVisibleChanged: updateTitleBarActions()
	}
	ResultView {
		id: authResult
		header: AuthModel.errorHeader
		hintButtonText: AuthModel.statusHintActionText
		hintText: AuthModel.statusHintText
		popupText: AuthModel.errorText
		//: INFO DESKTOP Error code (string) of current GlobalStatus code, shown as header of popup.
		popupTitle: qsTr("Error code: %1").arg(AuthModel.statusCodeString)
		resultType: AuthModel.resultString ? ResultView.Type.IsError : ResultView.Type.IsSuccess
		supportButtonsVisible: AuthModel.errorIsMasked
		text: AuthModel.resultString
		visible: d.activeView === AuthView.SubViews.Result

		onEmailButtonPressed: AuthModel.sendResultMail()
		onHintClicked: AuthModel.invokeStatusHintAction()
		onNextView: pName => {
			authView.nextView(pName);
			AuthModel.continueWorkflow();
		}
	}
}
