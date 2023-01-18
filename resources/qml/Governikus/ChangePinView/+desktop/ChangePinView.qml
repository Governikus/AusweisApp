/*
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.EnterPasswordView 1.0
import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.ProgressView 1.0
import Governikus.ResultView 1.0
import Governikus.SettingsView 1.0
import Governikus.View 1.0
import Governikus.Workflow 1.0
import Governikus.Type.PinResetInformationModel 1.0
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
	property alias _d: d

	function showPasswordInfo() {
		showWithPrecedingView(ChangePinView.SubViews.PasswordInfo);
	}
	function showSettings() {
		showWithPrecedingView(ChangePinView.SubViews.ReaderSettings);
	}
	function showWithPrecedingView(pNextView) {
		d.precedingView = d.activeView;
		d.view = pNextView;
		updateTitleBarActions();
	}

	titleBarAction: TitleBarAction {
		customSettingsHandler: baseItem.showSettings
		helpTopic: "pinManagement"
		rootEnabled: d.activeView === ChangePinView.SubViews.Start || d.activeView === ChangePinView.SubViews.NoPassword
		showSettings: changePinController.workflowState === ChangePinController.WorkflowStates.Reader
		//: LABEL DESKTOP
		text: qsTr("Change PIN")

		customSubAction: CancelAction {
			visible: d.cancelAllowed

			onClicked: {
				if (pinResult.visible) {
					ChangePinModel.continueWorkflow();
					baseItem.nextView(UiModule.DEFAULT);
				} else {
					ChangePinModel.cancelWorkflow();
				}
			}
		}

		onClicked: {
			if (d.activeView === ChangePinView.SubViews.PasswordInfo || d.activeView === ChangePinView.SubViews.NoPassword || d.activeView === ChangePinView.SubViews.ReaderSettings) {
				d.view = d.precedingView;
				updateTitleBarActions();
			}
		}
	}

	Keys.onEscapePressed: if (d.cancelAllowed)
		ChangePinModel.cancelWorkflow()

	QtObject {
		id: d

		readonly property int activeView: inputError.visible ? ChangePinView.SubViews.InputError : pinUnlocked.visible ? ChangePinView.SubViews.PinUnlocked : view
		readonly property bool cancelAllowed: view !== ChangePinView.SubViews.NoPassword && view !== ChangePinView.SubViews.Start && (ChangePinModel.isBasicReader || generalWorkflow.waitingFor !== Workflow.WaitingFor.Password)
		property int precedingView: ChangePinView.SubViews.Start
		property var view: ChangePinView.SubViews.Start
	}
	TabbedReaderView {
		visible: d.activeView === ChangePinView.SubViews.ReaderSettings

		onCloseView: {
			d.view = d.precedingView;
			updateTitleBarActions();
		}
	}
	ChangePinController {
		id: changePinController
		onNextView: pName => {
			if (pName === ChangePinView.SubViews.ReturnToMain) {
				baseItem.nextView(UiModule.DEFAULT);
				return;
			}
			d.view = pName;
		}
	}
	DecisionView {
		agreeButton.iconText: "?"
		//: LABEL DESKTOP
		agreeText: qsTr("PIN unknown")
		disagreeButton.iconText: "6"
		//: LABEL DESKTOP
		disagreeText: qsTr("Six-digit PIN")
		mainIconSource: "qrc:/images/material_lock.svg"
		moreInformationVisible: true
		neutralButton.iconText: "5"
		//: LABEL DESKTOP
		neutralText: qsTr("Five-digit Transport PIN")
		//: LABEL DESKTOP Description of PIN change start page. User has a choice of which PIN to set.
		questionSubText: qsTr("Please make a choice to set or change your PIN.")
		//: LABEL DESKTOP Title of PIN change start page. User is asked which type of PIN they have.
		questionText: qsTr("What kind of PIN do you have?")
		style: DecisionView.ButtonStyle.AllButtons
		visible: d.activeView === ChangePinView.SubViews.Start

		onAgree: d.view = ChangePinView.SubViews.NoPassword
		onDisagree: ChangePinModel.startWorkflow(false)
		onMoreInformationClicked: baseItem.showPasswordInfo()
		onNeutral: ChangePinModel.startWorkflow(true)
	}
	ResultView {
		id: noPasswordView
		buttonType: NavigationButton.Type.Check
		hintButtonText: PinResetInformationModel.pinResetActionText
		hintText: PinResetInformationModel.pinUnknownHint
		resultType: ResultView.Type.IsInfo
		text: PinResetInformationModel.pinUnknownText
		visible: d.activeView === ChangePinView.SubViews.NoPassword

		onHintClicked: Qt.openUrlExternally(PinResetInformationModel.pinResetUrl)
		onNextView: baseItem.nextView(UiModule.DEFAULT)
	}
	GeneralWorkflow {
		id: generalWorkflow
		isPinChange: true
		visible: d.activeView === ChangePinView.SubViews.Workflow
		waitingFor: switch (changePinController.workflowState) {
		case ChangePinController.WorkflowStates.Reader:
			return Workflow.WaitingFor.Reader;
		case ChangePinController.WorkflowStates.Password:
			return Workflow.WaitingFor.Password;
		default:
			return Workflow.WaitingFor.None;
		}

		onRequestPasswordInfo: baseItem.showPasswordInfo()
		onSettingsRequested: baseItem.showSettings()
	}
	EnterPasswordView {
		id: enterPasswordView
		visible: d.activeView === ChangePinView.SubViews.Password

		onPasswordEntered: pWasNewPin => {
			d.view = pWasNewPin ? ChangePinView.SubViews.ProgressNewPin : ChangePinView.SubViews.Progress;
			ChangePinModel.continueWorkflow();
		}
		onRequestPasswordInfo: baseItem.showPasswordInfo()
	}
	PasswordInfoView {
		id: passwordInfoView
		changePinInfo: changePinController.workflowState === ChangePinController.WorkflowStates.Initial
		visible: d.activeView === ChangePinView.SubViews.PasswordInfo

		onClose: {
			d.view = d.precedingView;
			updateTitleBarActions();
		}
	}
	ProgressView {
		id: pinProgressView
		//: INFO DESKTOP Processing screen text while the card communication is running after the PIN has been entered during PIN change process.
		subText: qsTr("Please do not move the ID card.")
		text: d.activeView === ChangePinView.SubViews.ProgressNewPin ?
		//: LABEL DESKTOP Processing screen label while the card communication is running after the new PIN has been entered during PIN change process.
		qsTr("Setting new PIN") :
		//: LABEL DESKTOP Processing screen label while the card communication is running after the old PIN has been entered during PIN change process.
		qsTr("Change PIN")
		visible: d.activeView === ChangePinView.SubViews.Progress || d.activeView === ChangePinView.SubViews.ProgressNewPin
	}
	ResultView {
		id: inputError

		property bool errorConfirmed: false

		resultType: ResultView.Type.IsError
		text: NumberModel.inputError
		visible: !errorConfirmed && NumberModel.hasPasswordError && d.view !== ChangePinView.SubViews.Result

		onNextView: errorConfirmed = true

		Connections {
			function onFireInputErrorChanged() {
				inputError.errorConfirmed = false;
			}

			target: NumberModel
		}
	}
	ResultView {
		id: pinUnlocked

		property bool confirmed: true

		resultType: ResultView.Type.IsSuccess
		//: INFO DESKTOP The ID card has just been unblocked and the user can now continue with their ID card PIN change.
		text: qsTr("Your ID card PIN is unblocked. You now have three more attempts to change your PIN.")
		visible: !confirmed && (d.view === ChangePinView.SubViews.Password || generalWorkflow.waitingFor === Workflow.WaitingFor.Password)

		onNextView: confirmed = true

		Connections {
			function onFireOnPinUnlocked() {
				pinUnlocked.confirmed = false;
			}

			target: ChangePinModel
		}
	}
	ResultView {
		id: cardPositionView
		resultType: ResultView.Type.IsInfo
		text: ChangePinModel.isRemoteReader ?
		//: INFO DESKTOP The NFC signal is weak or unstable, the user is asked to change the card's position to (hopefully) reduce the distance to the NFC chip.
		qsTr("Weak NFC signal. Please\n- change the card position\n- remove the mobile phone case (if present)\n- connect the smartphone with a charging cable") :
		//: INFO DESKTOP The NFC signal is weak or unstable, while using a stationary card reader.
		qsTr("Weak NFC signal. Please\n- make sure the card is positioned correctly on the reader\n- do note move the card while it is being accessed")
		visible: d.activeView === ChangePinView.SubViews.CardPosition

		onNextView: ChangePinModel.continueWorkflow()
	}
	ResultView {
		id: pinResult
		hintButtonText: ChangePinModel.statusHintActionText
		hintText: ChangePinModel.statusHintText
		resultType: ChangePinModel.error ? ResultView.Type.IsError : ResultView.Type.IsSuccess
		supportButtonsVisible: ChangePinModel.errorIsMasked
		text: ChangePinModel.resultString
		visible: d.activeView === ChangePinView.SubViews.Result

		onEmailButtonPressed: ChangePinModel.sendResultMail()
		onHintClicked: ChangePinModel.invokeStatusHintAction()
		onNextView: pName => {
			ChangePinModel.continueWorkflow();
			baseItem.nextView(pName);
		}
	}
}
