/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.AuthView 1.0
import Governikus.EnterPasswordView 1.0
import Governikus.PasswordInfoView 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Workflow 1.0
import Governikus.Type.ChatModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.PasswordType 1.0
import Governikus.Type.ReaderPlugIn 1.0
import Governikus.Type.RemoteServiceModel 1.0
import Governikus.Type.SettingsModel 1.0

Controller {
	id: controller

	readonly property bool enterPasswordShown: stackView.currentItem instanceof EnterPasswordView
	readonly property bool workflowShown: stackView.currentItem instanceof GeneralWorkflow

	function requestCard() {
		if (RemoteServiceModel.hasCard && workflowShown) {
			pop();
		} else if (!RemoteServiceModel.hasCard && !workflowShown) {
			push(generalWorkflow);
		}
	}
	function requestInput(pState) {
		if (RemoteServiceModel.isBasicReader && RemoteServiceModel.pinPadModeOn()) {
			push(enterPinView);
		} else {
			RemoteServiceModel.continueWorkflow();
		}
	}

	Connections {
		function onFireConnectedChanged() {
			if (RemoteServiceModel.connectedToPairedDevice && !workflowShown) {
				RemoteServiceModel.setInitialPluginType();
				requestCard();
			} else if (!RemoteServiceModel.connectedToPairedDevice && workflowShown) {
				pop();
			}
		}
		function onFireCurrentStateChanged() {
			switch (RemoteServiceModel.currentState) {
			case "Initial":
				break;
			case "StateStartIfdService":
				setLockedAndHidden();
				RemoteServiceModel.continueWorkflow();
				break;
			case "StateEnterPacePasswordIfd":
				if (SettingsModel.showAccessRights) {
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
		function onFireHasCardChanged() {
			if (!RemoteServiceModel.connectedToPairedDevice) {
				return;
			}
			if (enterPasswordShown) {
				return;
			}
			requestCard();
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
			titleBarColor: RemoteServiceModel.readerPlugInType === ReaderPlugIn.SMART ? Style.color.accent_smart : Style.color.accent
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
			}
			onPasswordEntered: {
				pop();
				RemoteServiceModel.startScanExplicitly();
				RemoteServiceModel.continueWorkflow();
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
