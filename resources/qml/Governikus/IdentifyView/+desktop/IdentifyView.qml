/*
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
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
import Governikus.Type.CardReturnCode 1.0


SectionPage
{
	id: identifyView

	enum SubViews {
		Undefined,
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
		ReaderSettings
	}

	activeFocusOnTab: true
	Keys.onEscapePressed: if (d.cancelAllowed) AuthModel.cancelWorkflow()

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP_QML
		text: qsTr("Identify") + SettingsModel.translationTrigger
		rootEnabled: false
		helpTopic: "ausweisenPage"
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
				d.view = readerView.preceedingView
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

		customSettingsHandler: function(){
			readerView.preceedingView = d.activeView
			d.view = IdentifyView.SubViews.ReaderSettings
			ApplicationWindow.menuBar.updateActions()
		}
	}

	QtObject {
		id: d

		property int view: IdentifyView.SubViews.Undefined
		readonly property int activeView: inputError.visible ? IdentifyView.SubViews.InputError : view
		readonly property bool cancelAllowed: AuthModel.isBasicReader || generalWorkflow.waitingFor != Workflow.WaitingFor.Password
	}

	TabbedReaderView {
		id: readerView

		property int preceedingView: IdentifyView.SubViews.Undefined

		visible: d.activeView === IdentifyView.SubViews.ReaderSettings
		onCloseView: {
			d.view = preceedingView
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

	ProgressView {
		id: checkConnectivityView

		visible: d.activeView === IdentifyView.SubViews.Connectivity

		//: INFO DESKTOP_QML Header of the message that no network connection is present during the authentication procedure.
		text: qsTr("No network connectivity") + SettingsModel.translationTrigger
		//: INFO DESKTOP_QML Content of the message that no network connection is present during the authentication procedure.
		subText: qsTr("Please enable the network interface or cancel the workflow.") + SettingsModel.translationTrigger
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
			helpTopic: "pinTab"
			customSubAction: CancelAction {
				onClicked:  {
					d.view = IdentifyView.SubViews.Password
					ApplicationWindow.menuBar.updateActions()
				}
			}
		}
		resultType: ResultView.Type.IsInfo
		//: INFO DESKTOP_QML The user clicked that the current PIN has 5 digits (transport PIN) which needs to be changed to a 6-digit PIN. The current process will be aborted and needs to be restarted *manually* by the user.
		text: qsTr("You have to change your transport PIN into a personal PIN to use the online ID function. You are currently leaving the started process and are forwarded to the PIN management. Please restart the desired process after the PIN has been changed.") + SettingsModel.translationTrigger
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
		text: qsTr("Weak NFC signal.\nPlease reposition your card.") + SettingsModel.translationTrigger
		onNextView: AuthModel.continueWorkflow()
	}

	ProgressView {
		id: identifyProgressView

		readonly property bool inProgress: identifyController.workflowState !== IdentifyController.WorkflowStates.Initial

		visible: d.activeView === IdentifyView.SubViews.Progress

		//: INFO DESKTOP_QML Header of the progress information during the authentication process.
		text: (inProgress ? qsTr("Authentication in progress") : qsTr("Acquiring provider certificate")) + SettingsModel.translationTrigger
		subText: {
			SettingsModel.translationTrigger;
			if (!visible) {
				return ""
			}
			if (AuthModel.isBasicReader) {
				//: INFO DESKTOP_QML Second line text if a basic card reader is used and data is exchanged with the card/server in the background. Is not actually visible since the basic reader password handling is done by EnterPasswordView.
				return qsTr("Please wait a moment...")
			}
			if (!!NumberModel.inputError) {
				return NumberModel.inputError
			}
			if (NumberModel.pinDeactivated) {
				//: INFO DESKTOP_QML The online authentication feature of the id card is deactivated and needs to be activated by the local authorities.
				return qsTr("The online identification function of your ID card is deactivated. Please contact the authority responsible for issuing your identification document to activate the online identification function.")
			}
			//: INFO DESKTOP_QML Generic progress status message during authentication.
			return qsTr("Please wait a moment...")
		}
		subTextColor: !AuthModel.isBasicReader && (NumberModel.inputError || NumberModel.pinDeactivated) ? Style.color.warning_text : Style.color.secondary_text
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
		emailButtonVisible: AuthModel.resultString
		onEmailButtonPressed: AuthModel.sendResultMail()
	}
}
