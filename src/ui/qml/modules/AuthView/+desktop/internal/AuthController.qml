/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.Animations
import Governikus.EnterPasswordView
import Governikus.MultiInfoView
import Governikus.ProgressView
import Governikus.ResultView
import Governikus.SettingsView
import Governikus.Style
import Governikus.TitleBar
import Governikus.Type
import Governikus.Workflow

ProgressView {
	id: root

	enum WorkflowStates {
		Initial,
		Progress,
		Reader,
		Password,
		Processing
	}

	readonly property bool isInitialState: workflowState === AuthController.WorkflowStates.Initial
	readonly property bool isSelfAuth: ApplicationModel.currentWorkflow === ApplicationModel.Workflow.SELF_AUTHENTICATION
	readonly property alias networkInterfaceActive: connectivityManager.networkInterfaceActive
	property bool startedByOnboarding: false
	property bool userCancelAndManualRedirect: false
	property bool workflowProgressVisible: false
	property int workflowState: 0

	signal backToSelfAuthView
	signal backToStartPage(bool pSuccess)
	signal changeTransportPin

	function continueWorkflowIfComfortReader() {
		if (!AuthModel.isBasicReader) {
			AuthModel.continueWorkflow();
		}
	}
	function displayAuthWorkflow() {
		if (!workflowActive) {
			push(authWorkflow);
		}
	}
	function displayInputError() {
		push(inputErrorView, {
			returnCode: AuthModel.lastReturnCode,
			inputError: NumberModel.inputError,
			passwordType: NumberModel.passwordType
		});
		continueWorkflowIfComfortReader();
	}
	function displaySuccessView(pPasswordType) {
		push(inputSuccessView, {
			passwordType: pPasswordType
		});
		continueWorkflowIfComfortReader();
	}
	function processStateChange(pState) {
		switch (pState) {
		case "StateGetTcToken":
			if (connectivityManager.networkInterfaceActive) {
				pop(root);
				setAuthWorkflowStateAndContinue(AuthController.WorkflowStates.Initial);
			} else {
				push(checkConnectivityView);
			}
			break;
		case "StatePreVerification":
			if (!NumberModel.isCanAllowedMode && SettingsModel.transportPinReminder) {
				SettingsModel.transportPinReminder = false;
				push(transportPinReminderView);
			} else {
				AuthModel.continueWorkflow();
			}
			break;
		case "StateCertificateDescriptionCheck":
			if (ApplicationModel.isScreenReaderRunning && SettingsModel.autoRedirectAfterAuthentication && SettingsModel.remindUserOfAutoRedirect) {
				push(a11yAutoRedirectDecision);
			} else {
				AuthModel.continueWorkflow();
			}
			break;
		case "StateEditAccessRights":
			if (NumberModel.isCanAllowedMode && SettingsModel.skipRightsOnCanAllowed) {
				ChatModel.transferAccessRights();
				AuthModel.continueWorkflow();
			} else {
				push(editRights);
			}
			AuthModel.setInitialPluginType();
			break;
		case "StateSelectReader":
			displayAuthWorkflow();
			setAuthWorkflowStateAndContinue(AuthController.WorkflowStates.Reader);
			break;
		case "StatePreparePace":
			pop(root);
			setAuthWorkflowStateAndContinue(AuthController.WorkflowStates.Processing);
			break;
		case "StateEnterPacePassword":
			if (AuthModel.lastReturnCode === CardReturnCode.OK_CAN) {
				displaySuccessView(NumberModel.PasswordType.CAN);
				return;
			} else if (AuthModel.lastReturnCode === CardReturnCode.OK_PUK) {
				displaySuccessView(NumberModel.PasswordType.PUK);
				return;
			}
			if (NumberModel.inputError !== "") {
				displayInputError();
				NumberModel.resetInputError();
				return;
			}
			if (NumberModel.initialInputError !== "") {
				push(inputErrorView, {
					returnCode: NumberModel.passwordType === NumberModel.PasswordType.CAN ? CardReturnCode.INVALID_CAN : CardReturnCode.INVALID_PUK,
					inputError: NumberModel.initialInputError,
					passwordType: NumberModel.passwordType,
					titleVisible: false
				});
				NumberModel.setInitialInputErrorShown();
				return;
			}
			setAuthWorkflowStateAndRequestInput(AuthController.WorkflowStates.Password);
			break;
		case "StateUnfortunateCardPosition":
			push(cardPositionView);
			break;
		case "StateSendDIDAuthenticateResponseEAC1":
			userCancelAndManualRedirect = false;
			if (AuthModel.isCancellationByUser()) {
				if (SettingsModel.autoRedirectAfterAuthentication) {
					push(regularAbortedAuthView);
				} else {
					userCancelAndManualRedirect = true;
					push(confirmAbortedAuthView);
					if (root.isSelfAuth) {
						//For a SelfAuth we skip continue otherwise we run staight into FinalState and leave
						break;
					}
				}
			} else {
				workflowProgressVisible = true;
				pop(root);
			}
			setAuthWorkflowStateAndContinue(AuthController.WorkflowStates.Processing);
			break;
		case "StateActivateStoreFeedbackDialog":
			if (root.isSelfAuth) {
				showRemoveCardFeedback(AuthModel, true);
			}
			AuthModel.continueWorkflow();
			break;
		case "StateRedirectBrowser":
			if (!AuthModel.error) {
				push(redirectToProvider);
			} else if (userCancelAndManualRedirect) {
				break;
			} else {
				AuthModel.continueWorkflow();
			}
			break;
		case "FinalState":
			if (AuthModel.error) {
				if (AuthModel.shouldSkipResultView()) {
					pop(root);
					AuthModel.continueWorkflow();
					if (AuthModel.changeTransportPin) {
						changeTransportPin();
					} else {
						backToStartPage(false);
					}
				} else if (userCancelAndManualRedirect) {
					userCancelledAndBackToStart(true);
				} else {
					showRemoveCardFeedback(AuthModel, false);
					push(authResult);
				}
			} else if (root.isSelfAuth) {
				push(selfAuthData);
			} else {
				userCancelledAndBackToStart(false);
			}
			workflowProgressVisible = false;
			break;
		default:
			AuthModel.continueWorkflow();
		}
	}
	function rerunCurrentState() {
		processStateChange(AuthModel.currentState);
	}
	function setAuthWorkflowStateAndContinue(pState) {
		workflowState = pState;
		AuthModel.continueWorkflow();
	}
	function setAuthWorkflowStateAndRequestInput(pState) {
		workflowState = pState;
		if (AuthModel.isBasicReader) {
			push(enterPasswordView);
		} else {
			displayAuthWorkflow();
			AuthModel.continueWorkflow();
		}
	}
	function userCancelledAndBackToStart(pUserCancelled) {
		pop(root);
		backToStartPage(pUserCancelled);
		AuthModel.continueWorkflow();
	}

	//: LABEL DESKTOP
	headline: (AuthModel.error ? qsTr("Cancel authentication process") :
		//: INFO DESKTOP Header of the progress status message during the authentication process.
		isInitialState ? qsTr("Acquiring provider certificate") :
		//: INFO DESKTOP Header of the progress status message during the authentication process.
		qsTr("Authentication in progress"))
	progressBarVisible: workflowProgressVisible
	progressText: AuthModel.progressMessage
	progressValue: AuthModel.progressValue
	text: {
		if (isInitialState) {
			//: INFO DESKTOP Generic status message during the authentication process.
			return qsTr("Please wait a moment.");
		}
		if (AuthModel.isBasicReader) {
			//: INFO DESKTOP Second line text if a basic card reader is used and background communication with the card/server is running. Is not actually visible since the basic reader password handling is done by EnterPasswordView.
			return qsTr("Please do not move the ID card.");
		}
		if (workflowState === AuthController.WorkflowStates.Processing || workflowState === AuthController.WorkflowStates.Password) {
			//: INFO DESKTOP The card reader requests the user's attention.
			return qsTr("Please observe the display of your card reader.");
		}

		//: INFO DESKTOP Generic status message during the authentication process.
		return qsTr("Please wait a moment.");
	}

	//: LABEL DESKTOP
	title: qsTr("Identify")
	titleBarSettings: cancelNavigation

	Component.onCompleted: if (AuthModel.currentState === "StateParseTcTokenUrl")
		rerunCurrentState()

	MultiInfoData {
		id: infoData

		contentType: fromPasswordType(NumberModel.passwordType, NumberModel.isCanAllowedMode)
	}
	Connections {
		function onFireStateEntered(pState) {
			root.processStateChange(pState);
		}

		target: AuthModel
	}
	ConnectivityManager {
		id: connectivityManager

		watching: AuthModel.currentState === "StateGetTcToken"

		onNetworkInterfaceActiveChanged: {
			if (watching)
				root.rerunCurrentState();
		}
	}
	Component {
		id: a11yAutoRedirectDecision

		AutoRedirectDecision {
			title: root.title

			onLeaveView: {
				root.pop();
				AuthModel.continueWorkflow();
			}
		}
	}
	Component {
		id: checkConnectivityView

		ProgressView {

			//: INFO DESKTOP Header of the message that no network connection is present during the authentication procedure.
			headline: qsTr("No network connectivity")
			icon: "qrc:///images/no_internet.svg"
			//: INFO DESKTOP Content of the message that no network connection is present during the authentication procedure.
			text: qsTr("Please establish an internet connection.")
			tintColor: Style.color.image
			title: root.title
			titleBarSettings: cancelNavigation
		}
	}
	Component {
		id: transportPinReminderView

		TransportPinReminderView {
			title: root.title

			onCancel: AuthModel.cancelWorkflow()
			onPinKnown: {
				pop();
				AuthModel.continueWorkflow();
			}
			onTransportPinKnown: {
				pop();
				AuthModel.cancelWorkflowToChangeTransportPin();
			}
		}
	}
	Component {
		id: editRights

		EditRights {
			title: root.title
			titleBarSettings: cancelNavigation

			onRightsAccepted: {
				ChatModel.transferAccessRights();
				AuthModel.continueWorkflow();
				pop();
			}
		}
	}
	Component {
		id: authWorkflow

		GeneralWorkflow {
			title: root.title
			waitingFor: switch (root.workflowState) {
			case AuthController.WorkflowStates.Reader:
				return Workflow.WaitingFor.Reader;
			case AuthController.WorkflowStates.Processing:
				return AuthModel.isBasicReader ? Workflow.WaitingFor.None : Workflow.WaitingFor.Password;
			case AuthController.WorkflowStates.Password:
				return Workflow.WaitingFor.Password;
			default:
				return Workflow.WaitingFor.None;
			}

			titleBarSettings: TitleBarSettings {
				navigationAction: NavigationAction.Cancel
				navigationEnabled: AuthModel.isBasicReader
				showSettings: true

				onNavigationActionClicked: AuthModel.cancelWorkflow()
				onSettingsClicked: root.push(settingsView)
			}

			onSettingsRequested: root.push(settingsView)

			Component {
				id: settingsView

				TabbedReaderView {
					onLeaveView: pop()
				}
			}
		}
	}
	Component {
		id: enterPasswordView

		EnterPasswordView {
			accessibleContinueText: passwordType === NumberModel.PasswordType.PIN || passwordType === NumberModel.PasswordType.SMART_PIN || (passwordType === NumberModel.PasswordType.CAN && NumberModel.isCanAllowedMode) ? qsTr("Authenticate with provider") : ""
			moreInformationText: infoData.linkText
			passwordType: NumberModel.passwordType
			title: root.title
			titleBarSettings: cancelNavigation

			onPasswordEntered: {
				pop();
				AuthModel.continueWorkflow();
			}
			onRequestPasswordInfo: push(multiInfoView)
		}
	}
	Component {
		id: inputErrorView

		InputErrorView {
			title: root.title
			titleBarSettings: cancelNavigation

			onContinueClicked: {
				pop(root);
				if (!AuthModel.isBasicReader) {
					root.displayAuthWorkflow();
				}
				root.rerunCurrentState();
			}
			onPasswordInfoRequested: push(multiInfoView)
		}
	}
	Component {
		id: multiInfoView

		MultiInfoView {
			infoContent: infoData
			titleBarSettings: backNavigation

			onAbortCurrentWorkflow: AuthModel.cancelWorkflow()
			onLeaveView: pop()
		}
	}
	Component {
		id: inputSuccessView

		InputSuccessView {
			passwordType: NumberModel.PasswordType.PUK
			title: root.title
			titleBarSettings: cancelNavigation

			onContinueClicked: {
				pop();
				root.setAuthWorkflowStateAndRequestInput(AuthController.WorkflowStates.Password);
			}
		}
	}
	Component {
		id: cardPositionView

		ResultView {
			animationSymbol: Symbol.Type.ERROR
			animationType: AnimationLoader.NFC_RESULT
			text: AuthModel.isRemoteReader ?
			//: INFO DESKTOP The NFC signal is weak or unstable, the user is asked to change the card's position to (hopefully) reduce the distance to the NFC chip.
			qsTr("Weak NFC signal. Please\n change the card position\n remove the mobile phone case (if present)\n connect the smartphone with a charging cable") :
			//: INFO DESKTOP The NFC signal is weak or unstable, while using a stationary card reader.
			qsTr("Weak NFC signal. Please\n make sure the card is positioned correctly on the reader\n do note move the card while it is being accessed")
			title: root.title
			titleBarSettings: cancelNavigation

			onLeaveView: {
				pop();
				AuthModel.continueWorkflow();
			}
		}
	}
	Component {
		id: regularAbortedAuthView

		ProgressView {

			//: INFO DESKTOP The user aborted the authentication process, according to TR we need to inform the service provider
			headline: qsTr("Aborting process and informing the service provider")
			progressBarVisible: false
			text: {
				if (connectivityManager.networkInterfaceActive) {
					//: INFO DESKTOP Information message about cancellation process with present network connectivity
					return qsTr("Please wait a moment.");
				}
				//: INFO DESKTOP Information message about cancellation process without working network connectivity
				return qsTr("Network problems detected, trying to reach server within 30 seconds.");
			}
			title: root.title
			titleBarSettings: disabledCancelNavigation
		}
	}
	Component {
		id: confirmAbortedAuthView

		AuthCanceledView {
			startedByOnboarding: root.startedByOnboarding
			title: root.title

			titleBarSettings: TitleBarSettings {
				navigationAction: !root.isSelfAuth ? NavigationAction.Cancel : root.startedByOnboarding ? NavigationAction.Back : NavigationAction.Close
				navigationEnabled: root.isSelfAuth

				onNavigationActionClicked: AuthModel.continueWorkflow()
			}

			onLeaveView: AuthModel.continueWorkflow()
		}
	}
	Component {
		id: selfAuthData

		SelfAuthenticationData {
			okButtonText: root.startedByOnboarding ?
			//: LABEL ANDROID IOS
			qsTr("Back to setup") :
			//: LABEL ANDROID IOS
			qsTr("Back to start page")
			title: root.title

			titleBarSettings: TitleBarSettings {
				navigationAction: root.startedByOnboarding ? NavigationAction.Back : NavigationAction.Close

				onNavigationActionClicked: {
					root.pop();
					AuthModel.continueWorkflow();
					if (navigationAction === NavigationAction.Back)
						root.backToSelfAuthView();
					else
						root.backToStartPage(true);
				}
			}

			onLeaveView: {
				pop();
				AuthModel.continueWorkflow();
				root.backToStartPage(true);
			}
		}
	}
	Component {
		id: redirectToProvider

		RedirectView {
			title: root.title
			titleBarSettings: disabledCancelNavigation

			onLeaveView: {
				pop();
				AuthModel.continueWorkflow();
			}
		}
	}
	Component {
		id: authResult

		ResultView {
			id: authResultView

			animation: AuthModel.statusCodeAnimation
			buttonIcon: AuthModel.resultViewButtonIcon
			buttonText: root.startedByOnboarding ?
			//: LABEL ANDROID IOS
			qsTr("Back to setup") : AuthModel.resultViewButtonText
			header: AuthModel.resultHeader
			hintButtonText: AuthModel.statusHintActionText
			hintText: AuthModel.statusHintText
			hintTitle: AuthModel.statusHintTitle
			mailButtonVisible: AuthModel.errorIsMasked
			popupText: AuthModel.errorText
			popupTitle: AuthModel.statusCodeDisplayString
			subheader: AuthModel.errorHeader
			text: AuthModel.resultString
			title: root.title

			titleBarSettings: TitleBarSettings {
				navigationAction: root.startedByOnboarding ? NavigationAction.Action.Back : NavigationAction.Action.Close

				onNavigationActionClicked: authResultView.leaveView()
			}

			onEmailButtonPressed: AuthModel.sendResultMail()
			onHintClicked: AuthModel.invokeStatusHintAction()
			onLeaveView: {
				if (AuthModel.resultViewButtonLink !== "") {
					Qt.openUrlExternally(AuthModel.resultViewButtonLink);
				}
				root.backToStartPage(false);
				AuthModel.continueWorkflow();
			}
		}
	}
	TitleBarSettings {
		id: disabledCancelNavigation

		navigationAction: NavigationAction.Cancel
		navigationEnabled: false
	}
	TitleBarSettings {
		id: cancelNavigation

		navigationAction: NavigationAction.Cancel
		navigationEnabled: AuthModel.isBasicReader

		onNavigationActionClicked: AuthModel.cancelWorkflow()
	}
	TitleBarSettings {
		id: backNavigation

		navigationAction: NavigationAction.Back

		onNavigationActionClicked: root.pop()
	}
}
