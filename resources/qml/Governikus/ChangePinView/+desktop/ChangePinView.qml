/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
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
import Governikus.Type.ChangePinModel
import Governikus.Type.NumberModel
import Governikus.Type.PasswordType
import Governikus.Type.UiModule

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

	Keys.onEscapePressed: if (d.cancelAllowed)
		ChangePinModel.cancelWorkflow()

	QtObject {
		id: d

		readonly property int activeView: inputError.visible ? ChangePinView.SubViews.InputError : pinUnlocked.visible ? ChangePinView.SubViews.PinUnlocked : view
		readonly property bool cancelAllowed: view !== ChangePinView.SubViews.NoPassword && view !== ChangePinView.SubViews.Start && (ChangePinModel.isBasicReader || generalWorkflow.waitingFor !== Workflow.WaitingFor.Password)
		property int enteredPasswordType: PasswordType.PIN
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

			onChangePin: ChangePinModel.startWorkflow(false)
			onChangeTransportPin: ChangePinModel.startWorkflow(true)
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

		icon: "qrc:///images/workflow_error_no_sak_%1.svg".arg(Style.currentTheme.name)
		//: INFO DESKTOP The paired devices was removed since it did not respond to connection attempts. It needs to be paired again if it should be used as card reader.
		text: qsTr("The device \"%1\" was unpaired because it did not react to connection attempts. Pair the device again to use it as a card reader.").arg(deviceName)
		visible: d.activeView === ChangePinView.SubViews.WorkflowError

		onNextView: d.view = ChangePinView.SubViews.Workflow
	}
	EnterPasswordView {
		id: enterPasswordView

		moreInformationText: infoData.linkText
		visible: d.activeView === ChangePinView.SubViews.Password

		onPasswordEntered: pPasswordType => {
			d.enteredPasswordType = pPasswordType;
			switch (pPasswordType) {
			case PasswordType.NEW_PIN:
			case PasswordType.NEW_SMART_PIN:
				break;
			case PasswordType.NEW_PIN_CONFIRMATION:
			case PasswordType.NEW_SMART_PIN_CONFIRMATION:
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
		//: INFO DESKTOP
		readonly property string transportPinHint: qsTr("Please note that you may use the five-digit Transport PIN only once to change to a six-digit ID card PIN. If you already set a six-digit ID card PIN, the five-digit Transport PIN is no longer valid.")

		icon: switch (d.enteredPasswordType) {
		case PasswordType.TRANSPORT_PIN:
			return "qrc:///images/workflow_error_wrong_transportpin_%1.svg".arg(Style.currentTheme.name);
		case PasswordType.SMART_PIN:
		case PasswordType.PIN:
			return "qrc:///images/workflow_error_wrong_pin_%1.svg".arg(Style.currentTheme.name);
		case PasswordType.CAN:
			return "qrc:///images/workflow_error_wrong_can_%1.svg".arg(Style.currentTheme.name);
		case PasswordType.PUK:
			return "qrc:///images/workflow_error_wrong_puk_%1.svg".arg(Style.currentTheme.name);
		default:
			return "qrc:///images/status_error_%1.svg".arg(Style.currentTheme.name);
		}
		text: NumberModel.inputError + (NumberModel.inputError !== "" && ChangePinModel.requestTransportPin ? "<br><br>%1".arg(transportPinHint) : "")
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

		animatedIcon: "qrc:///images/puk_%1.webp".arg(Style.currentTheme.name)
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

		hintButtonText: ChangePinModel.statusHintActionText
		hintText: ChangePinModel.statusHintText
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
