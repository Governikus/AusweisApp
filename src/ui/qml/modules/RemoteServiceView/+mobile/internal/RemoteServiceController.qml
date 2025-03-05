/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.AuthView
import Governikus.EnterPasswordView
import Governikus.MultiInfoView
import Governikus.TitleBar
import Governikus.View
import Governikus.Workflow
import Governikus.Type

Controller {
	id: root

	readonly property bool editRightsShown: root.stackView.currentItem instanceof EditRights
	readonly property bool enterPasswordShown: root.stackView.currentItem instanceof EnterPasswordView
	readonly property bool isSmartWorkflow: RemoteServiceModel.readerPluginType === ReaderManagerPluginType.SMART
	readonly property bool workflowShown: root.stackView.currentItem instanceof GeneralWorkflow

	function processStateChange(pState) {
		switch (pState) {
		case "StateStartIfdService":
			root.setLockedAndHidden();
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
		if (RemoteServiceModel.hasCard && root.workflowShown) {
			root.pop();
		} else if (!RemoteServiceModel.hasCard && !workflowShown) {
			root.push(generalWorkflow);
		}
	}
	function requestInput(pState) {
		if (RemoteServiceModel.isBasicReader && RemoteServiceModel.pinPadModeOn()) {
			root.push(enterPinView, {
				passwordType: NumberModel.passwordType
			});
		} else {
			RemoteServiceModel.continueWorkflow();
		}
	}

	Connections {
		function onFireConnectedChanged() {
			if (RemoteServiceModel.connectedToPairedDevice && !root.workflowShown) {
				root.requestCard();
				RemoteServiceModel.setInitialPluginType();
			} else if (!RemoteServiceModel.connectedToPairedDevice) {
				root.popAll();
			}
		}
		function onFireHasCardChanged() {
			if (!RemoteServiceModel.connectedToPairedDevice) {
				return;
			}
			if (root.enterPasswordShown || root.editRightsShown) {
				return;
			}
			root.requestCard();
		}
		function onFireStateEntered(pState) {
			root.processStateChange(pState);
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
					root.pop();
					RemoteServiceModel.cancelPasswordRequest();
				}
			}

			onRightsAccepted: {
				ChatModel.transferAccessRights();
				root.pop();
				root.requestInput();
			}
		}
	}
	Component {
		id: generalWorkflow

		GeneralWorkflow {
			cardInitiallyAppeared: RemoteServiceModel.cardInitiallyAppeared
			smartEidUsed: RemoteServiceModel.readerPluginType === ReaderManagerPluginType.SMART
			workflowModel: RemoteServiceModel
			//: LABEL ANDROID IOS
			workflowTitle: qsTr("Card reader")

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Cancel

				onClicked: {
					RemoteServiceModel.setRunning(false);
				}
			}
		}
	}
	MultiInfoData {
		id: infoData

		contentType: fromPasswordType(NumberModel.passwordType)
	}
	Component {
		id: multiInfoView

		MultiInfoView {
			infoContent: infoData

			onAbortCurrentWorkflow: {
				root.popAll();
				RemoteServiceModel.cancelPasswordRequest();
			}
			onClose: root.pop()
		}
	}
	Component {
		id: enterPinView

		EnterPasswordView {
			id: passwordView

			enableTransportPinLink: RemoteServiceModel.enableTransportPinLink
			moreInformationText: infoData.linkText
			smartEidUsed: root.isSmartWorkflow
			//: LABEL ANDROID IOS
			title: qsTr("Card reader")

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Cancel

				onClicked: {
					root.pop();
					RemoteServiceModel.cancelPasswordRequest();
				}
			}

			onChangePinLength: {
				RemoteServiceModel.changePinLength();
				passwordView.passwordType = NumberModel.passwordType;
			}
			onPasswordEntered: pPasswordType => {
				switch (pPasswordType) {
				case NumberModel.PasswordType.NEW_PIN:
				case NumberModel.PasswordType.NEW_SMART_PIN:
					root.processStateChange(RemoteServiceModel.currentState);
					break;
				case NumberModel.PasswordType.NEW_PIN_CONFIRMATION:
				case NumberModel.PasswordType.NEW_SMART_PIN_CONFIRMATION:
					if (NumberModel.commitNewPin()) {
						root.popAll();
						RemoteServiceModel.startScanExplicitly();
						RemoteServiceModel.continueWorkflow();
					} else {
						popAll();
						RemoteServiceModel.passwordsDiffer();
					}
					break;
				default:
					pop();
					RemoteServiceModel.startScanExplicitly();
					RemoteServiceModel.continueWorkflow();
				}
			}
			onRequestPasswordInfo: root.push(multiInfoView)
		}
	}
}
