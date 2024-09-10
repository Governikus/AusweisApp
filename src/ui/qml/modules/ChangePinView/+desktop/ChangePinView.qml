/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.EnterPasswordView
import Governikus.Global
import Governikus.TitleBar
import Governikus.PasswordInfoView
import Governikus.ProgressView
import Governikus.ResultView
import Governikus.SettingsView
import Governikus.Style
import Governikus.View
import Governikus.Workflow
import Governikus.Type

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
		InitialInputError,
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
		rootEnabled: d.activeView === ChangePinView.SubViews.Start || d.activeView === ChangePinView.SubViews.NoPassword
		showSettings: changePinController.workflowState === ChangePinController.WorkflowStates.Reader
		//: LABEL DESKTOP
		text: qsTr("Change PIN")

		customSubAction: NavigationAction {
			enabled: d.cancelAllowed

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

	Keys.onEscapePressed: event => {
		if (d.cancelAllowed) {
			ChangePinModel.cancelWorkflow();
		} else {
			event.accepted = false;
		}
	}

	QtObject {
		id: d

		readonly property int activeView: inputError.visible ? ChangePinView.SubViews.InputError : pinUnlocked.visible ? ChangePinView.SubViews.PinUnlocked : initialInputError.visible ? ChangePinView.SubViews.InitialInputError : view
		readonly property bool cancelAllowed: view !== ChangePinView.SubViews.NoPassword && view !== ChangePinView.SubViews.Start && view !== ChangePinView.SubViews.Result && (ChangePinModel.isBasicReader || generalWorkflow.waitingFor !== Workflow.WaitingFor.Password)
		property int enteredPasswordType: NumberModel.PasswordType.PIN
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
			id: mainView

			anchors.fill: parent

			onChangePin: ChangePinModel.startWorkflow(false)
			onChangePinInfoRequested: baseItem.showPasswordInfo()
			onChangeTransportPin: ChangePinModel.startWorkflow(true)
			onNoPinAvailable: d.view = ChangePinView.SubViews.NoPassword
		}
	}
	PasswordInfoView {
		id: noPasswordView

		visible: d.activeView === ChangePinView.SubViews.NoPassword

		infoContent: PasswordInfoData {
			contentType: PasswordInfoData.Type.NO_PIN
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

		icon: "qrc:///images/workflow_error_no_sak_%1.svg".arg(Style.currentTheme.name)
		//: INFO DESKTOP The paired devices was removed since it did not respond to connection attempts. It needs to be paired again if it should be used as card reader.
		text: qsTr("The device \"%1\" was unpaired because it did not react to connection attempts. Pair the device again to use it as a card reader.").arg(deviceName)
		visible: d.activeView === ChangePinView.SubViews.WorkflowError

		onNextView: d.view = ChangePinView.SubViews.Workflow
	}
	EnterPasswordView {
		id: enterPasswordView

		moreInformationText: infoData.linkText
		passwordType: d.passwordType
		visible: d.activeView === ChangePinView.SubViews.Password

		onPasswordEntered: pPasswordType => {
			d.enteredPasswordType = pPasswordType;
			switch (pPasswordType) {
			case NumberModel.PasswordType.NEW_PIN:
			case NumberModel.PasswordType.NEW_SMART_PIN:
				break;
			case NumberModel.PasswordType.NEW_PIN_CONFIRMATION:
			case NumberModel.PasswordType.NEW_SMART_PIN_CONFIRMATION:
				if (NumberModel.commitNewPin()) {
					d.view = ChangePinView.SubViews.ProgressNewPin;
					ChangePinModel.continueWorkflow();
				}
				break;
			default:
				ChangePinModel.continueWorkflow();
				d.view = ChangePinView.SubViews.Progress;
			}
		}
		onRequestPasswordInfo: baseItem.showPasswordInfo()
	}
	PasswordInfoData {
		id: infoData

		contentType: {
			if (NumberModel.inputError === "" || !ChangePinModel.requestTransportPin) {
				return fromPasswordType(d.passwordType);
			}
			switch (ChangePinModel.lastReturnCode) {
			case CardReturnCode.INVALID_CAN:
			case CardReturnCode.INVALID_PUK:
				return fromPasswordType(d.passwordType);
			default:
				return PasswordInfoData.Type.TRANSPORT_PIN_NOT_WORKING;
			}
		}
	}
	PasswordInfoView {
		id: passwordInfoView

		infoContent: changePinController.workflowState === ChangePinController.WorkflowStates.Initial ? mainView.pinInfo : infoData
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
	InputErrorView {
		id: inputError

		property bool errorConfirmed: false

		inputError: NumberModel.inputError
		isTransportPin: ChangePinModel.requestTransportPin
		passwordType: NumberModel.passwordType
		returnCode: ChangePinModel.lastReturnCode
		visible: !errorConfirmed && NumberModel.inputError !== "" && d.view !== ChangePinView.SubViews.Result && d.view !== ChangePinView.SubViews.PasswordInfo

		onContinueClicked: {
			errorConfirmed = true;
			if (ChangePinModel.isBasicReader) {
				d.view = ChangePinView.SubViews.Password;
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
		visible: NumberModel.initialInputError !== "" && d.view !== ChangePinView.SubViews.Result && d.view !== ChangePinView.SubViews.PasswordInfo

		onContinueClicked: {
			NumberModel.setInitialInputErrorShown();
			if (ChangePinModel.isBasicReader) {
				d.view = ChangePinView.SubViews.Password;
			}
		}
		onPasswordInfoRequested: showPasswordInfo()
	}
	InputSuccessView {
		id: pinUnlocked

		property bool confirmed: true

		isTransportPin: ChangePinModel.requestTransportPin
		passwordType: NumberModel.PasswordType.PUK
		visible: !confirmed && (d.view === ChangePinView.SubViews.Password || generalWorkflow.waitingFor === Workflow.WaitingFor.Password)

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

			target: ChangePinModel
		}
	}
	ResultView {
		id: cardPositionView

		icon: "qrc:///images/workflow_error_nfc_%1.svg".arg(Style.currentTheme.name)
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

		//: LABEL DESKTOP
		buttonText: qsTr("Back to start page")
		hintButtonText: ChangePinModel.statusHintActionText
		hintText: ChangePinModel.statusHintText
		hintTitle: ChangePinModel.statusHintTitle
		icon: ChangePinModel.error ? ChangePinModel.statusCodeImage.arg(Style.currentTheme.name) : "qrc:///images/workflow_success_changepin_%1.svg".arg(Style.currentTheme.name)
		mailButtonVisible: ChangePinModel.errorIsMasked
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
