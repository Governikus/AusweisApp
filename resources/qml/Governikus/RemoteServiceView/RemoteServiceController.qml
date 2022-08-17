/*
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15

import Governikus.View 1.0
import Governikus.Type.RemoteServiceModel 1.0
import Governikus.EnterPasswordView 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.ReaderPlugIn 1.0
import Governikus.Type.NumberModel 1.0

Controller {
	id: controller

	Connections {
		target: RemoteServiceModel
		function onFireCurrentStateChanged() {
			switch (RemoteServiceModel.currentState) {
				case "Initial":
					break
				case "StateStartIfdService":
					setLockedAndHidden()
					RemoteServiceModel.continueWorkflow()
					break
				case "StateEnterPacePasswordIfd":
					requestInput()
					break
				case "StateEnterNewPacePinIfd":
					requestInput()
					break
				case "FinalState":
					RemoteServiceModel.continueWorkflow()
					setLockedAndHidden(false)
					break
				default:
					RemoteServiceModel.continueWorkflow()
			}
		}
	}

	function requestInput(pState) {
		if (RemoteServiceModel.isBasicReader && RemoteServiceModel.pinPadModeOn()) {
			push(enterPinView)
		} else {
			RemoteServiceModel.continueWorkflow()
		}
	}

	Component {
		id: enterPinView

		EnterPasswordView {
			id: passwordView

			enableTransportPinLink: RemoteServiceModel.enableTransportPinLink

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Cancel
				onClicked: {
					pop()
					RemoteServiceModel.cancelPasswordRequest()
				}
			}

			onPasswordEntered: {
				pop()
				RemoteServiceModel.continueWorkflow()
			}

			onChangePinLength: {
				RemoteServiceModel.changePinLength()
				state = RemoteServiceModel.getPasswordType()
			}

			Connections {
				target: RemoteServiceModel
				function onFireConnectedChanged() { if (!RemoteServiceModel.connectedToPairedDevice && passwordView.StackView.visible) pop() }
			}
		}
	}
}
