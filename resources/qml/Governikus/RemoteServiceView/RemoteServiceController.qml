/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.AuthView
import Governikus.EnterPasswordView
import Governikus.PasswordInfoView
import Governikus.Style
import Governikus.TitleBar
import Governikus.View
import Governikus.Workflow
import Governikus.Type.ChatModel
import Governikus.Type.NumberModel
import Governikus.Type.PasswordType
import Governikus.Type.ReaderPlugIn
import Governikus.Type.RemoteServiceModel
import Governikus.Type.SettingsModel

Controller {
	id: controller

	readonly property bool editRightsShown: stackView.currentItem instanceof EditRights
	readonly property bool enterPasswordShown: stackView.currentItem instanceof EnterPasswordView
	readonly property bool isSmartWorkflow: RemoteServiceModel.readerPlugInType === ReaderPlugIn.SMART
	readonly property bool workflowShown: stackView.currentItem instanceof GeneralWorkflow

	function processStateChange(pState) {
		switch (pState) {
		case "StateStartIfdService":
			setLockedAndHidden();
			RemoteServiceModel.continueWorkflow();
			break;
		case "StateEnterPacePasswordIfd":
			if (SettingsModel.showAccessRights && RemoteServiceModel.isPinAuthentication()) {
				push(editRights);
			} else {
				requestInput();
			}
			break;
		case "StateEstablishPaceChannelIfd":
		case "StateChangePinIfd":
			requestCard();
			RemoteServiceModel.continueWorkflow();
			break;
		case "StateEnterNewPacePinIfd":
			requestInput();
			break;
		case "FinalState":
			RemoteServiceModel.continueWorkflow();
			setLockedAndHidden(false);
			break;
		default:
			RemoteServiceModel.continueWorkflow();
		}
	}
	function requestCard() {
		if (RemoteServiceModel.hasCard && workflowShown) {
			pop();
		} else if (!RemoteServiceModel.hasCard && !workflowShown) {
			push(generalWorkflow);
		}
	}
	function requestInput(pState) {
		if (RemoteServiceModel.isBasicReader && RemoteServiceModel.pinPadModeOn()) {
			push(enterPinView, {
					"passwordType": NumberModel.passwordType,
					"inputError": NumberModel.inputError
				});
		} else {
			RemoteServiceModel.continueWorkflow();
		}
	}

	Connections {
		function onFireConnectedChanged() {
			if (RemoteServiceModel.connectedToPairedDevice && !workflowShown) {
				requestCard();
				RemoteServiceModel.setInitialPluginType();
			} else if (!RemoteServiceModel.connectedToPairedDevice && (workflowShown || enterPasswordShown || editRightsShown)) {
				pop();
			}
		}
		function onFireHasCardChanged() {
			if (!RemoteServiceModel.connectedToPairedDevice) {
				return;
			}
			if (enterPasswordShown || editRightsShown) {
				return;
			}
			requestCard();
		}
		function onFireStateEntered(pState) {
			processStateChange(pState);
		}

		target: RemoteServiceModel
	}
	Component {
		id: editRights

		EditRights {
			//: LABEL ANDROID IOS
			actionText: qsTr("You are about to identify yourself towards the following provider using the device \"%1\":").arg(RemoteServiceModel.connectedClientName)
			//: LABEL ANDROID IOS
			title: qsTr("Card reader")
			workflowModel: RemoteServiceModel

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Cancel

				onClicked: {
					pop();
					RemoteServiceModel.cancelPasswordRequest();
				}
			}

			onRightsAccepted: {
				ChatModel.transferAccessRights();
				pop();
				requestInput();
			}
		}
	}
	Component {
		id: generalWorkflow

		GeneralWorkflow {
			smartEidUsed: RemoteServiceModel.readerPlugInType === ReaderPlugIn.SMART
			workflowModel: RemoteServiceModel
			//: LABEL ANDROID IOS
			workflowTitle: qsTr("Remote service")

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Cancel

				onClicked: {
					RemoteServiceModel.setRunning(false);
				}
			}
		}
	}
	PasswordInfoData {
		id: infoData

		contentType: fromPasswordType(NumberModel.passwordType)
	}
	Component {
		id: passwordInfoView

		PasswordInfoView {
			infoContent: infoData

			onAbortCurrentWorkflow: {
				popAll();
				RemoteServiceModel.cancelPasswordRequest();
			}
			onClose: pop()
		}
	}
	Component {
		id: enterPinView

		EnterPasswordView {
			id: passwordView

			enableTransportPinLink: RemoteServiceModel.enableTransportPinLink
			moreInformationText: infoData.linkText
			smartEidUsed: isSmartWorkflow
			//: LABEL ANDROID IOS
			title: qsTr("Card reader")

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Cancel

				onClicked: {
					pop();
					RemoteServiceModel.cancelPasswordRequest();
				}
			}

			onChangePinLength: {
				RemoteServiceModel.changePinLength();
				passwordView.passwordType = NumberModel.passwordType;
			}
			onPasswordEntered: pPasswordType => {
				switch (pPasswordType) {
				case PasswordType.NEW_PIN:
				case PasswordType.NEW_SMART_PIN:
					controller.processStateChange(RemoteServiceModel.currentState);
					break;
				case PasswordType.NEW_PIN_CONFIRMATION:
				case PasswordType.NEW_SMART_PIN_CONFIRMATION:
					if (NumberModel.commitNewPin()) {
						popAll();
						RemoteServiceModel.startScanExplicitly();
						RemoteServiceModel.continueWorkflow();
					} else {
						controller.processStateChange(RemoteServiceModel.currentState);
					}
					break;
				default:
					pop();
					RemoteServiceModel.startScanExplicitly();
					RemoteServiceModel.continueWorkflow();
				}
			}
			onRequestPasswordInfo: push(passwordInfoView)

			Connections {
				function onFireConnectedChanged() {
					if (!RemoteServiceModel.connectedToPairedDevice && passwordView.StackView.visible)
						pop();
				}

				target: RemoteServiceModel
			}
		}
	}
}
