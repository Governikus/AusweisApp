/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.EnterPasswordView
import Governikus.TitleBar
import Governikus.MultiInfoView
import Governikus.ProgressView
import Governikus.ResultView
import Governikus.WhiteListClient
import Governikus.View
import Governikus.Workflow
import Governikus.Type

Controller {
	id: root

	property bool autoInsertCard: false
	property bool hideTechnologySwitch: false
	property var initialPlugin: null
	property bool isInitialState: true
	readonly property bool isSelfAuth: ApplicationModel.currentWorkflow === ApplicationModel.Workflow.SELF_AUTHENTICATION
	readonly property bool isSmartWorkflow: AuthModel.readerPluginType === ReaderManagerPluginType.SMART
	readonly property int passwordType: NumberModel.passwordType
	readonly property bool smartEidUsed: isSmartWorkflow && !isInitialState
	property bool startedByOnboarding: false
	property string title
	property bool userCancelAndManualRedirect: false
	property bool workflowProgressVisible: false

	signal changeTransportPin
	signal requestBack
	signal workflowFinished(bool pSuccess)

	function continueWorkflowIfComfortReader() {
		if (!AuthModel.isBasicReader) {
			AuthModel.continueWorkflow();
		}
	}
	function displayInputError() {
		replace(inputErrorView, {
			returnCode: AuthModel.lastReturnCode,
			inputError: NumberModel.inputError,
			passwordType: NumberModel.passwordType
		});
		continueWorkflowIfComfortReader();
	}
	function displaySuccessView(pPasswordType) {
		replace(inputSuccessView, {
			passwordType: pPasswordType
		});
		continueWorkflowIfComfortReader();
	}
	function processStateChange(pState) {
		switch (pState) {
		case "StateGetTcToken":
			if (connectivityManager.networkInterfaceActive) {
				push(progressView);
				AuthModel.continueWorkflow();
			} else {
				push(checkConnectivityView);
			}
			break;
		case "StatePreVerification":
			if (!NumberModel.isCanAllowedMode && SettingsModel.transportPinReminder) {
				SettingsModel.transportPinReminder = false;
				push(transportPinReminder);
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
				replace(editRights);
			}
			AuthModel.setInitialPluginType();
			break;
		case "StateSelectReader":
			if (!workflowActive) {
				replace(authWorkflow, {
					cardInitiallyAppeared: AuthModel.cardInitiallyAppeared
				});
			}
			isInitialState = false;
			AuthModel.continueWorkflow();
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
				replace(inputErrorView, {
					returnCode: NumberModel.passwordType === NumberModel.PasswordType.CAN ? CardReturnCode.INVALID_CAN : CardReturnCode.INVALID_PUK,
					inputError: NumberModel.initialInputError,
					passwordType: NumberModel.passwordType,
					titleVisible: false
				});
				NumberModel.setInitialInputErrorShown();
				return;
			}
			requestInput();
			break;
		case "StateEstablishPaceChannel":
			if (AuthModel.isBasicReader && !(stackView.currentItem instanceof ProgressView)) {
				replace(progressView);
			}
			AuthModel.continueWorkflow();
			break;
		case "StateUnfortunateCardPosition":
			push(cardPositionView);
			break;
		case "StateDidAuthenticateEac1":
			if (workflowActive) {
				replace(progressView);
			}
			root.workflowProgressVisible = true;
			AuthModel.continueWorkflow();
			break;
		case "StateSendDIDAuthenticateResponseEAC1":
			userCancelAndManualRedirect = false;
			if (AuthModel.isCancellationByUser()) {
				if (SettingsModel.autoRedirectAfterAuthentication) {
					push(regularAbortedAuthView);
				} else {
					userCancelAndManualRedirect = true;
					push(confirmAbortedAuthView);
					if (ApplicationModel.currentWorkflow === ApplicationModel.Workflow.SELF_AUTHENTICATION) {
						//For a SelfAuth we skip continue otherwise we run staight into FinalState and leave
						break;
					}
				}
			}
			AuthModel.continueWorkflow();
			break;
		case "StateActivateStoreFeedbackDialog":
			if (ApplicationModel.currentWorkflow === ApplicationModel.Workflow.SELF_AUTHENTICATION) {
				showRemoveCardFeedback(AuthModel, true);
			}
			AuthModel.continueWorkflow();
			break;
		case "StateShowResult":
			replace(selfAuthenticationData);
			break;
		case "StateSendWhitelistSurvey":
			if (SurveyModel.askForDeviceSurvey()) {
				replace(whiteListSurveyView);
			} else {
				AuthModel.continueWorkflow();
			}
			break;
		case "StateRedirectBrowser":
			if (!AuthModel.error) {
				replace(redirectToProvider);
			} else if (userCancelAndManualRedirect) {
				break;
			} else {
				AuthModel.continueWorkflow();
			}
			break;
		case "FinalState":
			if (AuthModel.changeTransportPin) {
				AuthModel.continueWorkflow();
				changeTransportPin();
			} else if (userCancelAndManualRedirect) {
				AuthModel.continueWorkflow();
			} else if (AuthModel.error && !AuthModel.hasNextWorkflowPending && !AuthModel.shouldSkipResultView()) {
				showRemoveCardFeedback(AuthModel, false);
				replace(authResult);
			} else {
				AuthModel.continueWorkflow();
			}
			root.workflowProgressVisible = false;
			break;
		default:
			AuthModel.continueWorkflow();
		}
	}
	function requestInput() {
		if (AuthModel.isBasicReader) {
			replace(enterPinView, {
				passwordType: NumberModel.passwordType
			});
		} else {
			replace(progressView);
			AuthModel.continueWorkflow();
		}
	}
	function rerunCurrentState() {
		processStateChange(AuthModel.currentState);
	}

	Connections {
		function onFireStateEntered(pState) {
			root.processStateChange(pState);
		}
		function onFireWorkflowFinished(pSuccess) {
			root.workflowFinished(pSuccess);
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
		id: progressView

		ProgressView {
			//: LABEL ANDROID IOS
			headline: (AuthModel.error ? qsTr("Cancel authentication process") :
				//: INFO ANDROID IOS Header of the progress status message during the authentication process.
				root.isInitialState ? qsTr("Acquiring provider certificate") :
				//: INFO ANDROID IOS Header of the progress status message during the authentication process.
				qsTr("Authentication in progress"))
			progressBarVisible: root.workflowProgressVisible
			progressText: AuthModel.progressMessage
			progressValue: AuthModel.progressValue
			smartEidUsed: root.smartEidUsed
			text: {
				//: INFO ANDROID IOS Generic status message during the authentication process.
				if (root.isInitialState || AuthModel.error || root.isSmartWorkflow) {
					return qsTr("Please wait a moment.");
				}
				if (AuthModel.isBasicReader) {
					//: INFO ANDROID IOS Second line text if a basic card reader is used and background communication with the card/server is running. Is not actually visible since the basic reader password handling is done by EnterPasswordView.
					return qsTr("Please do not move the ID card.");
				}
				//: INFO ANDROID IOS The card reader requests the user's attention.
				return qsTr("Please observe the display of your card reader.");
			}
			//: LABEL ANDROID IOS
			title: qsTr("Identify")

			navigationAction: NavigationAction {
				action: AuthModel.isBasicReader || root.workflowProgressVisible ? NavigationAction.Action.Cancel : NavigationAction.Action.None

				onClicked: AuthModel.cancelWorkflow()
			}
		}
	}
	Component {
		id: editRights

		EditRights {
			title: root.title

			onRightsAccepted: {
				ChatModel.transferAccessRights();
				AuthModel.continueWorkflow();
			}
		}
	}
	Component {
		id: selfAuthenticationData

		SelfAuthenticationData {
			okButtonText: root.startedByOnboarding ?
			//: LABEL ANDROID IOS
			qsTr("Back to setup") :
			//: LABEL ANDROID IOS
			qsTr("Back to start page")
			smartEidUsed: root.smartEidUsed

			navigationAction: NavigationAction {
				action: root.startedByOnboarding ? NavigationAction.Action.Back : NavigationAction.Action.Close

				onClicked: {
					if (root.startedByOnboarding)
						root.requestBack();
					AuthModel.continueWorkflow();
				}
			}

			onLeaveView: AuthModel.continueWorkflow()
		}
	}
	Component {
		id: whiteListSurveyView

		WhiteListSurveyView {
			smartEidUsed: root.smartEidUsed

			onDone: pUserAccepted => {
				SurveyModel.setDeviceSurveyPending(pUserAccepted);
				AuthModel.continueWorkflow();
			}
		}
	}
	Component {
		id: authWorkflow

		GeneralWorkflow {
			autoInsertCard: root.autoInsertCard
			hideSwitch: root.hideTechnologySwitch
			initialPlugin: root.initialPlugin
			smartEidUsed: root.smartEidUsed
			workflowModel: AuthModel
			workflowTitle: root.title
		}
	}
	Component {
		id: transportPinReminder

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
	MultiInfoData {
		id: infoData

		contentType: fromPasswordType(NumberModel.passwordType, NumberModel.isCanAllowedMode)
	}
	Component {
		id: multiInfoView

		MultiInfoView {
			infoContent: infoData
			smartEidUsed: root.smartEidUsed

			onAbortCurrentWorkflow: AuthModel.cancelWorkflow()
			onClose: pop()
		}
	}
	Component {
		id: enterPinView

		EnterPasswordView {
			//: LABEL ANDROID IOS A11y button to confirm the PIN and start the provider authentication
			accessibleContinueText: passwordType === NumberModel.PasswordType.PIN || passwordType === NumberModel.PasswordType.SMART_PIN || (passwordType === NumberModel.PasswordType.CAN && NumberModel.isCanAllowedMode) ? qsTr("Authenticate with provider") : ""
			moreInformationText: infoData.linkText
			smartEidUsed: root.smartEidUsed
			title: root.title

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Cancel

				onClicked: {
					root.replace(progressView);
					AuthModel.cancelWorkflow();
				}
			}

			onPasswordEntered: {
				replace(progressView);
				AuthModel.continueWorkflow();
			}
			onRequestPasswordInfo: push(multiInfoView)
		}
	}
	Component {
		id: regularAbortedAuthView

		AbortedProgressView {
			networkInterfaceActive: connectivityManager.networkInterfaceActive
			smartEidUsed: root.smartEidUsed
			title: root.title

			onCancel: AuthModel.cancelWorkflow()
		}
	}
	Component {
		id: confirmAbortedAuthView

		AuthCanceledView {
			smartEidUsed: root.smartEidUsed
			startedByOnboarding: root.startedByOnboarding
			title: root.title

			navigationAction: NavigationAction {
				action: !root.isSelfAuth ? NavigationAction.Cancel : root.startedByOnboarding ? NavigationAction.Back : NavigationAction.Close
				enabled: root.isSelfAuth

				onClicked: AuthModel.continueWorkflow()
			}

			onLeaveView: AuthModel.continueWorkflow()
		}
	}
	Component {
		id: checkConnectivityView

		CheckConnectivityView {
			title: root.title

			onCancel: AuthModel.cancelWorkflow()
		}
	}
	Component {
		id: cardPositionView

		CardPositionView {
			title: root.title

			onCancelClicked: AuthModel.cancelWorkflow()
			onContinueClicked: {
				pop();
				AuthModel.continueWorkflow();
			}
		}
	}
	Component {
		id: authResult

		ResultErrorView {
			id: authResultView

			animation: AuthModel.statusCodeAnimation
			//: LABEL ANDROID IOS
			buttonText: root.startedByOnboarding ? qsTr("Back to setup") : AuthModel.resultViewButtonText
			errorCode: AuthModel.statusCodeDisplayString
			errorDescription: AuthModel.errorText
			//: LABEL ANDROID IOS
			header: qsTr("Authentication failed")
			hintButtonText: AuthModel.statusHintActionText
			hintText: AuthModel.statusHintText
			hintTitle: AuthModel.statusHintTitle
			//: LABEL ANDROID IOS
			mailButtonText: AuthModel.errorIsMasked ? qsTr("Send log") : ""
			smartEidUsed: root.smartEidUsed
			subheader: AuthModel.errorHeader
			text: AuthModel.resultString
			title: root.title

			navigationAction: NavigationAction {
				action: root.startedByOnboarding ? NavigationAction.Action.Back : NavigationAction.Action.Close

				onClicked: authResultView.continueClicked()
			}

			onContinueClicked: AuthModel.continueWorkflow()
			onHintClicked: {
				AuthModel.continueWorkflow();
				AuthModel.invokeStatusHintAction();
			}
			onMailClicked: LogModel.mailLog("support@ausweisapp.de", AuthModel.getEmailHeader(), AuthModel.getEmailBody())
		}
	}
	Component {
		id: redirectToProvider

		RedirectView {
			title: root.title

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Cancel
				enabled: false
			}

			onContinueClicked: {
				pop();
				AuthModel.continueWorkflow();
			}
		}
	}
	Component {
		id: inputErrorView

		InputErrorView {
			smartEidUsed: root.smartEidUsed
			title: root.title

			navigationAction: NavigationAction {
				action: AuthModel.isBasicReader ? NavigationAction.Action.Cancel : NavigationAction.Action.None

				onClicked: {
					root.replace(progressView);
					AuthModel.cancelWorkflow();
				}
			}

			onContinueClicked: {
				root.requestInput();
			}
			onPasswordInfoRequested: push(multiInfoView)
		}
	}
	Component {
		id: inputSuccessView

		InputSuccessView {
			smartEidUsed: root.smartEidUsed
			title: root.title

			navigationAction: NavigationAction {
				action: AuthModel.isBasicReader ? NavigationAction.Action.Cancel : NavigationAction.Action.None

				onClicked: {
					root.replace(progressView);
					AuthModel.cancelWorkflow();
				}
			}

			onContinueClicked: {
				switch (passwordType) {
				case NumberModel.PasswordType.CAN:
				case NumberModel.PasswordType.PUK:
					root.requestInput();
					break;
				}
			}
		}
	}
}
