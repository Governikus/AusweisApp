/*
 * \copyright Copyright (c) 2015-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.EnterPasswordView 1.0
import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.ProgressView 1.0
import Governikus.ResultView 1.0
import Governikus.SettingsView 1.0
import Governikus.View 1.0
import Governikus.Workflow 1.0
import Governikus.Type.ChangePinModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.UiModule 1.0


SectionPage {
	id: baseItem

	enum SubViews {
		Start,
		Workflow,
		Password,
		NoPassword,
		PasswordInfo,
		Progress,
		ProgressNewPin,
		CardPosition,
		InputError,
		Data,
		PinUnlocked,
		Result,
		ReturnToMain,
		ReaderSettings
	}

	// Expose private properties, needed for unit tests
	property alias _d : d

	function showWithPrecedingView(pNextView) {
		readerView.precedingView = d.activeView
		d.view = pNextView
		ApplicationWindow.menuBar.updateActions()
	}

	function showPasswordInfo() {
		showWithPrecedingView(ChangePinView.SubViews.PasswordInfo)
	}

	function showSettings() {
		showWithPrecedingView(ChangePinView.SubViews.ReaderSettings)
	}

	Keys.onEscapePressed: if (d.cancelAllowed) ChangePinModel.cancelWorkflow()

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP_QML
		text: qsTr("Change PIN")
		rootEnabled: d.activeView === ChangePinView.SubViews.Start
		helpTopic: "pinManagement"
		showSettings: (changePinController.workflowState === ChangePinController.WorkflowStates.Reader ||
						changePinController.workflowState === ChangePinController.WorkflowStates.Card)
						&& d.activeView !== ChangePinView.SubViews.Progress
						&& d.activeView !== ChangePinView.SubViews.ProgressNewPin
						&& d.activeView !== ChangePinView.SubViews.NoPassword

		onClicked: {
			if (d.activeView === ChangePinView.SubViews.PasswordInfo ||
				d.activeView === ChangePinView.SubViews.NoPassword ||
				d.activeView === ChangePinView.SubViews.ReaderSettings) {
					d.view = readerView.precedingView
					ApplicationWindow.menuBar.updateActions()
			}
		}

		customSubAction: CancelAction {
			visible: d.cancelAllowed

			onClicked: {
				if (pinResult.visible) {
					ChangePinModel.continueWorkflow()
					baseItem.nextView(UiModule.DEFAULT)
				} else {
					ChangePinModel.cancelWorkflow()
				}
			}
		}

		customSettingsHandler: baseItem.showSettings
	}

	QtObject {
		id: d

		property var view: ChangePinView.SubViews.Start
		readonly property int activeView: inputError.visible ? ChangePinView.SubViews.InputError : pinUnlocked.visible ? ChangePinView.SubViews.PinUnlocked : view
		readonly property bool cancelAllowed: view !== ChangePinView.SubViews.NoPassword && view !== ChangePinView.SubViews.Start && (ChangePinModel.isBasicReader || generalWorkflow.waitingFor != Workflow.WaitingFor.Password)
	}

	TabbedReaderView {
		id: readerView

		property int precedingView: ChangePinView.SubViews.Start

		visible: d.activeView === ChangePinView.SubViews.ReaderSettings
		onCloseView: {
			d.view = precedingView
			ApplicationWindow.menuBar.updateActions()
		}
	}

	ChangePinController {
		id: changePinController

		onNextView: {
			if (pName === ChangePinView.SubViews.ReturnToMain) {
				baseItem.nextView(UiModule.DEFAULT)
				return;
			}

			d.view = pName
		}
	}

	DecisionView {
		visible: d.activeView === ChangePinView.SubViews.Start

		style: DecisionView.ButtonStyle.AllButtons
		mainIconSource: "qrc:/images/material_lock.svg"
		//: LABEL DESKTOP_QML Title of PIN change start page. User is asked which type of PIN they have.
		questionText: qsTr("What kind of PIN do you have?")
		questionSubText: "%1<br><br><a href=\"#\">%2</a>"
			//: LABEL DESKTOP_QML Description of PIN change start page. User has a choice of which PIN to set.
			.arg(qsTr("Here you have the possibility to set or change the PIN of your identity card."))
			//: LABEL DESKTOP_QML More information link. Leads to information about pin letter.
			.arg(qsTr("More information"))
		agreeButton.iconText: "?"
		//: LABEL DESKTOP_QML
		agreeText: qsTr("No PIN")
		neutralButton.iconText: "5"
		//: LABEL DESKTOP_QML
		neutralText: qsTr("Five-digit Transport PIN")
		disagreeButton.iconText: "6"
		//: LABEL DESKTOP_QML
		disagreeText: qsTr("Six-digit PIN")

		onSubTextLinkActivated: baseItem.showPasswordInfo()

		onNeutral: ChangePinModel.startWorkflow(true)
		onDisagree: ChangePinModel.startWorkflow(false)
		onAgree: d.view = ChangePinView.SubViews.NoPassword
	}

	ResultView {
		id: noPasswordView

		visible: d.activeView === ChangePinView.SubViews.NoPassword

		resultType: ResultView.Type.IsInfo
		buttonType: NavigationButton.Type.Check
		//: LABEL DESKTOP_QML
		text: qsTr("You cannot find your PIN letter? You have set a PIN when picking up the ID card or later that you cannot recall now?\n\nIf this is the case please turn to the competent authority and set a new PIN there.")

		onNextView: baseItem.nextView(UiModule.DEFAULT)
	}

	GeneralWorkflow {
		id: generalWorkflow

		visible: d.activeView === ChangePinView.SubViews.Workflow

		isPinChange: true
		waitingFor: switch (changePinController.workflowState) {
						case ChangePinController.WorkflowStates.Reader:
							return Workflow.WaitingFor.Reader
						case ChangePinController.WorkflowStates.Card:
							return Workflow.WaitingFor.Card
						case ChangePinController.WorkflowStates.Password:
							return Workflow.WaitingFor.Password
						default:
							return Workflow.WaitingFor.None
		}
		onSettingsRequested: baseItem.showSettings()
	}

	EnterPasswordView {
		id: enterPasswordView

		visible: d.activeView === ChangePinView.SubViews.Password

		onPasswordEntered: {
			d.view = pWasNewPin ? ChangePinView.SubViews.ProgressNewPin : ChangePinView.SubViews.Progress
			ChangePinModel.continueWorkflow()
		}

		onRequestPasswordInfo: baseItem.showPasswordInfo()
	}

	PasswordInfoView {
		id: passwordInfoView

		visible: d.activeView === ChangePinView.SubViews.PasswordInfo

		onClose: {
			d.view = readerView.precedingView
			ApplicationWindow.menuBar.updateActions()
		}
	}

	ProgressView {
		id: pinProgressView

		visible: d.activeView === ChangePinView.SubViews.Progress || d.activeView === ChangePinView.SubViews.ProgressNewPin

		text: d.activeView === ChangePinView.SubViews.ProgressNewPin
			//: LABEL DESKTOP_QML Processing screen label while the card communication is running after the new PIN has been entered during PIN change process.
			? qsTr("Setting new PIN")
			//: LABEL DESKTOP_QML Processing screen label while the card communication is running after the old PIN has been entered during PIN change process.
			: qsTr("Change PIN")
		//: INFO DESKTOP_QML Processing screen text while the card communication is running after the PIN has been entered during PIN change process.
		subText: qsTr("Please do not move the ID card.")
	}

	ResultView {
		id: inputError

		property bool errorConfirmed: false

		visible: !errorConfirmed && NumberModel.hasPasswordError && d.view != ChangePinView.SubViews.Result

		resultType: ResultView.Type.IsError
		text: NumberModel.inputError
		onNextView: errorConfirmed = true

		Connections {
			target: NumberModel
			onFireInputErrorChanged: inputError.errorConfirmed = false
		}
	}

	ResultView {
		id: pinUnlocked

		property bool confirmed: true

		visible: !confirmed && (d.view === ChangePinView.SubViews.Password || generalWorkflow.waitingFor === Workflow.WaitingFor.Password)

		resultType: ResultView.Type.IsSuccess
		//: INFO DESKTOP_QML The ID card has just been unblocked and the user can now continue with their PIN change.
		text: qsTr("Your PIN is unblocked. You now have three more attempts to change your PIN.")
		onNextView: confirmed = true

		Connections {
			target: ChangePinModel
			onFireOnPinUnlocked: pinUnlocked.confirmed = false
		}
	}

	ResultView {
		id: cardPositionView

		visible: d.activeView === ChangePinView.SubViews.CardPosition

		resultType: ResultView.Type.IsInfo
		//: INFO DESKTOP_QML The NFC signal is weak or unstable, the user is asked to change the card's position to (hopefully) reduce the distance to the NFC chip.
		text: qsTr("Weak NFC signal. Please\n- change the card position\n- remove the mobile phone case (if present)\n- connect the smartphone with a charging cable")
		onNextView: ChangePinModel.continueWorkflow()
	}

	ResultView {
		id: pinResult

		visible: d.activeView === ChangePinView.SubViews.Result

		resultType: ChangePinModel.error ? ResultView.Type.IsError : ResultView.Type.IsSuccess
		text: ChangePinModel.resultString
		onNextView: {
			ChangePinModel.continueWorkflow()
			baseItem.nextView(pName)
		}
		supportButtonsVisible: ChangePinModel.error && !ChangePinModel.isCancellationByUser()
		onEmailButtonPressed: ChangePinModel.sendResultMail()
	}
}
