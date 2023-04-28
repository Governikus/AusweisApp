/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.EnterPasswordView 1.0
import Governikus.PasswordInfoView 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.PasswordType 1.0
import Governikus.Type.ReaderPlugIn 1.0
import Governikus.Type.RemoteServiceModel 1.0

Controller {
	id: controller
	function requestInput(pState) {
		if (RemoteServiceModel.isBasicReader && RemoteServiceModel.pinPadModeOn()) {
			push(enterPinView);
		} else {
			RemoteServiceModel.continueWorkflow();
		}
	}

	Connections {
		function onFireCurrentStateChanged() {
			switch (RemoteServiceModel.currentState) {
			case "Initial":
				break;
			case "StateStartIfdService":
				setLockedAndHidden();
				RemoteServiceModel.continueWorkflow();
				break;
			case "StateEnterPacePasswordIfd":
				requestInput();
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

		target: RemoteServiceModel
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
