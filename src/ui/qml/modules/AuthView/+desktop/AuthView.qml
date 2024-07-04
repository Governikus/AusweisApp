/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.EnterPasswordView
import Governikus.Global
import Governikus.Style
import Governikus.PasswordInfoView
import Governikus.ProgressView
import Governikus.ResultView
import Governikus.SettingsView
import Governikus.TitleBar
import Governikus.View
import Governikus.Workflow
import Governikus.Type

SectionPage {
	id: authView

	enum SubViews {
		Undefined,
		TransportPinReminder,
		TransportPinReminderInfo,
		Connectivity,
		Progress,
		AccessRights,
		Workflow,
		WorkflowError,
		Password,
		PasswordInfo,
		CardPosition,
		InputError,
		InitialInputError,
		Data,
		PinUnlocked,
		Result,
		RedirectToProvider,
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
		rootEnabled: false
		showSettings: authController.workflowState === AuthController.WorkflowStates.Reader
		//: LABEL DESKTOP
		text: qsTr("Identify")

		customSubAction: NavigationAction {
			enabled: d.cancelAllowed

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
			switch (d.activeView) {
			case AuthView.SubViews.TransportPinReminderInfo:
			case AuthView.SubViews.PasswordInfo:
			case AuthView.SubViews.ReaderSettings:
				d.view = d.precedingView;
				updateTitleBarActions();
				break;
			case AuthView.SubViews.Data:
				authView.nextView(UiModule.SELF_AUTHENTICATION);
				AuthModel.continueWorkflow();
				break;
			}
		}
	}

	Keys.onEscapePressed: if (d.cancelAllowed)
		AuthModel.cancelWorkflow()

	QtObject {
		id: d

		readonly property int activeView: inputError.visible ? AuthView.SubViews.InputError : pinUnlocked.visible ? AuthView.SubViews.PinUnlocked : initialInputError.visible ? AuthView.SubViews.InitialInputError : view
		readonly property bool cancelAllowed: (AuthModel.isBasicReader || generalWorkflow.waitingFor !== Workflow.WaitingFor.Password) && activeView !== AuthView.SubViews.RedirectToProvider && activeView !== AuthView.SubViews.Result
		property int enteredPasswordType: NumberModel.PasswordType.PIN
		readonly property int passwordType: NumberModel.passwordType
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
		moreInformationText: infoData.linkText
		moreInformationVisible: true
		questionSubText: qsTr("Online identification with Transport PIN is not possible. The self-selected, 6-digit ID card PIN is mandatory to use the eID function.")
		questionText: qsTr("Do you know your 6-digit ID card PIN?")
		visible: d.activeView === AuthView.SubViews.TransportPinReminder

		onAgree: {
			d.view = AuthView.SubViews.Progress;
			AuthModel.continueWorkflow();
		}
		onDisagree: AuthModel.cancelWorkflowToChangePin()
		onMoreInformationClicked: showWithPrecedingView(AuthView.SubViews.TransportPinReminderInfo)
	}
	ProgressView {
		id: checkConnectivityView

		//: INFO DESKTOP Content of the message that no network connection is present during the authentication procedure.
		subText: qsTr("Please establish an internet connection.")
		subTextColor: Style.color.textNormal.basic

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

		onDeviceUnpaired: function (pDeviceName) {
			deviceUnpairedView.deviceName = pDeviceName;
			showWithPrecedingView(AuthView.SubViews.WorkflowError);
		}
		onSettingsRequested: authView.showSettings()
	}
	ResultView {
		id: deviceUnpairedView

		property string deviceName

		icon: "qrc:///images/workflow_error_no_sak_%1.svg".arg(Style.currentTheme.name)
		//: INFO DESKTOP The paired devices was removed since it did not respond to connection attempts. It needs to be paired again if it should be used as card reader.
		text: qsTr("The device \"%1\" was unpaired because it did not react to connection attempts. Pair the device again to use it as a card reader.").arg(deviceName)
		visible: d.activeView === AuthView.SubViews.WorkflowError

		onNextView: d.view = AuthView.SubViews.Workflow
	}
	EnterPasswordView {
		id: enterPasswordView

		//: LABEL DESKTOP A11y button to confirm the PIN and start the provider authentication
		accessibleContinueText: passwordType === NumberModel.PasswordType.PIN || passwordType === NumberModel.PasswordType.SMART_PIN || (passwordType === NumberModel.PasswordType.CAN && NumberModel.isCanAllowedMode) ? qsTr("Authenticate with provider") : ""
		moreInformationText: infoData.linkText
		passwordType: d.passwordType
		visible: d.activeView === AuthView.SubViews.Password

		onPasswordEntered: pPasswordType => {
			d.enteredPasswordType = pPasswordType;
			d.view = AuthView.SubViews.Progress;
			AuthModel.continueWorkflow();
		}
		onRequestPasswordInfo: authView.showPasswordInfo()
	}
	PasswordInfoData {
		id: infoData

		contentType: d.activeView === AuthView.SubViews.TransportPinReminder || d.activeView === AuthView.SubViews.TransportPinReminderInfo ? PasswordInfoData.Type.CHANGE_PIN : fromPasswordType(d.passwordType, NumberModel.isCanAllowedMode)
	}
	PasswordInfoView {
		id: passwordInfoView

		infoContent: infoData
		visible: d.activeView === AuthView.SubViews.PasswordInfo || d.activeView === AuthView.SubViews.TransportPinReminderInfo

		titleBarAction.customSubAction: NavigationAction {
			type: NavigationAction.Action.Back

			onClicked: passwordInfoView.close()
		}

		onAbortCurrentWorkflow: AuthModel.cancelWorkflow()
		onClose: {
			d.view = d.precedingView;
			updateTitleBarActions();
		}
	}
	InputErrorView {
		id: inputError

		property bool errorConfirmed: false

		inputError: NumberModel.inputError
		passwordType: NumberModel.passwordType
		returnCode: AuthModel.lastReturnCode
		visible: !errorConfirmed && NumberModel.inputError !== "" && d.view !== AuthView.SubViews.Result && d.view !== AuthView.SubViews.PasswordInfo

		onContinueClicked: {
			errorConfirmed = true;
			if (AuthModel.isBasicReader) {
				d.view = AuthView.SubViews.Password;
			}
		}
		onPasswordInfoRequested: showPasswordInfo()

		Connections {
			function onFireInputErrorChanged() {
				inputError.errorConfirmed = false;
			}

			target: NumberModel
		}
	}
	InputErrorView {
		id: initialInputError

		inputError: NumberModel.initialInputError
		passwordType: NumberModel.passwordType
		returnCode: NumberModel.passwordType === NumberModel.PasswordType.CAN ? CardReturnCode.INVALID_CAN : CardReturnCode.INVALID_PUK
		titleVisible: false
		visible: NumberModel.initialInputError !== "" && d.view !== AuthView.SubViews.Result && d.view !== AuthView.SubViews.PasswordInfo

		onContinueClicked: {
			NumberModel.setInitialInputErrorShown();
			if (AuthModel.isBasicReader) {
				d.view = AuthView.SubViews.Password;
			}
		}
		onPasswordInfoRequested: showPasswordInfo()
	}
	InputSuccessView {
		id: pinUnlocked

		property bool confirmed: true

		passwordType: NumberModel.PasswordType.PUK
		visible: !confirmed && (d.view === AuthView.SubViews.Password || generalWorkflow.waitingFor === Workflow.WaitingFor.Password)

		onContinueClicked: confirmed = true

		Connections {
			function onFireOnCanSuccess() {
				pinUnlocked.passwordType = NumberModel.PasswordType.CAN;
				pinUnlocked.confirmed = false;
			}
			function onFireOnPasswordUsed() {
				pinUnlocked.confirmed = true;
			}
			function onFireOnPinUnlocked() {
				pinUnlocked.passwordType = NumberModel.PasswordType.PUK;
				pinUnlocked.confirmed = false;
			}

			target: AuthModel
		}
	}
	ResultView {
		id: cardPositionView

		icon: "qrc:///images/workflow_error_nfc_%1.svg".arg(Style.currentTheme.name)
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
			if (authController.networkInterfaceActive) {
				//: INFO DESKTOP Information message about cancellation process with present network connectivity
				return qsTr("Please wait a moment.");
			}
			//: INFO DESKTOP Information message about cancellation process without working network connectivity
			return qsTr("Network problems detected, trying to reach server within 30 seconds.");
		}
		subTextColor: Style.color.textNormal.basic

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
		subTextColor: Style.color.textNormal.basic
		text: (isInitialState ?
			//: INFO DESKTOP Header of the progress information during the authentication process.
			qsTr("Acquiring provider certificate") :
			//: INFO DESKTOP Header of the progress information during the authentication process.
			qsTr("Authentication in progress"))
		visible: d.activeView === AuthView.SubViews.Progress
	}
	SelfAuthenticationData {
		visible: d.activeView === AuthView.SubViews.Data

		onAccept: {
			authView.nextView(UiModule.DEFAULT);
			AuthModel.continueWorkflow();
		}
		onVisibleChanged: updateTitleBarActions()
	}
	ResultView {
		id: authResult

		buttonIcon: AuthModel.resultViewButtonIcon
		buttonText: AuthModel.resultViewButtonText
		header: AuthModel.resultHeader
		hintButtonText: AuthModel.statusHintActionText
		hintText: AuthModel.statusHintText
		hintTitle: AuthModel.statusHintTitle
		icon: AuthModel.statusCodeImage !== "" ? AuthModel.statusCodeImage.arg(Style.currentTheme.name) : ""
		mailButtonVisible: AuthModel.errorIsMasked
		popupText: AuthModel.errorText
		//: INFO DESKTOP Error code (string) of current GlobalStatus code, shown as header of popup.
		popupTitle: qsTr("Error code: %1").arg(AuthModel.statusCodeString)
		subheader: AuthModel.errorHeader
		text: AuthModel.resultString
		visible: d.activeView === AuthView.SubViews.Result

		onEmailButtonPressed: AuthModel.sendResultMail()
		onHintClicked: AuthModel.invokeStatusHintAction()
		onNextView: pName => {
			if (AuthModel.resultViewButtonLink !== "") {
				Qt.openUrlExternally(AuthModel.resultViewButtonLink);
			}
			authView.nextView(pName);
			AuthModel.continueWorkflow();
		}
	}
	RedirectView {
		id: redirectView

		visible: d.activeView === AuthView.SubViews.RedirectToProvider

		onNextView: AuthModel.continueWorkflow()
	}
}
