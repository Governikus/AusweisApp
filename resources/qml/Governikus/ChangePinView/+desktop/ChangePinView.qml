/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.EnterPasswordView 1.0
import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.PasswordInfoView 1.0
import Governikus.ProgressView 1.0
import Governikus.ResultView 1.0
import Governikus.SettingsView 1.0
import Governikus.View 1.0
import Governikus.Workflow 1.0
import Governikus.Type.ChangePinModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.PasswordType 1.0
import Governikus.Type.UiModule 1.0

SectionPage {
	id: baseItem
	enum SubViews {
		Start,
		Workflow,
		WorkflowError,
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
		readonly property int passwordType: NumberModel.passwordType
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
	SectionPage {
		visible: d.activeView === ChangePinView.SubViews.Start

		ChangePinViewContent {
			anchors.fill: parent
			moreInformationText: infoData.linkText

			onMoreInformationRequested: baseItem.showPasswordInfo()
			onNoPinAvailable: d.view = ChangePinView.SubViews.NoPassword
		}
	}
	PasswordInfoView {
		id: noPasswordView
		visible: d.activeView === ChangePinView.SubViews.NoPassword

		infoContent: PasswordInfoData {
			contentType: PasswordInfoContent.Type.NO_PIN
		}

		onClose: d.view = ChangePinView.SubViews.Start
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

		onDeviceUnpaired: function (pDeviceName) {
			deviceUnpairedView.deviceName = pDeviceName;
			showWithPrecedingView(ChangePinView.SubViews.WorkflowError);
		}
		onSettingsRequested: baseItem.showSettings()
	}
	ResultView {
		id: deviceUnpairedView

		property string deviceName

		resultType: ResultView.Type.IsError
		//: INFO DESKTOP The paired devices was removed since it did not respond to connection attempts. It needs to be paired again if it should be used as card reader.
		text: qsTr("The device \"%1\" was unpaired because it did not react to connection attempts. Pair the device again to use it as a card reader.").arg(deviceName)
		visible: d.activeView === ChangePinView.SubViews.WorkflowError

		onNextView: d.view = ChangePinView.SubViews.Workflow
	}
	EnterPasswordView {
		id: enterPasswordView
		moreInformationText: infoData.linkText
		visible: d.activeView === ChangePinView.SubViews.Password

		onPasswordEntered: pWasNewPin => {
			d.view = pWasNewPin ? ChangePinView.SubViews.ProgressNewPin : ChangePinView.SubViews.Progress;
			ChangePinModel.continueWorkflow();
		}
		onRequestPasswordInfo: baseItem.showPasswordInfo()
	}
	PasswordInfoData {
		id: infoData
		contentType: changePinController.workflowState === ChangePinController.WorkflowStates.Initial ? PasswordInfoContent.Type.CHANGE_PIN : fromPasswordType(d.passwordType)
	}
	PasswordInfoView {
		id: passwordInfoView
		infoContent: infoData
		visible: d.activeView === ChangePinView.SubViews.PasswordInfo

		onAbortCurrentWorkflow: ChangePinModel.cancelWorkflow()
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
		mailButtonVisible: ChangePinModel.errorIsMasked
		resultType: ChangePinModel.error ? ResultView.Type.IsError : ResultView.Type.IsSuccess
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
