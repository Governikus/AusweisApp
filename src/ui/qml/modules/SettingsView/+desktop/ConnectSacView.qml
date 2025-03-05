/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.EnterPasswordView
import Governikus.ProgressView
import Governikus.RemoteServiceView
import Governikus.TitleBar
import Governikus.Type

EnterPasswordView {
	id: root

	property bool showSuccessView: true

	signal pairingFailed
	signal pairingSuccessful

	passwordType: NumberModel.PasswordType.REMOTE_PIN

	//: LABEL DESKTOP
	title: qsTr("Pairing")

	titleBarSettings: TitleBarSettings {
		navigationAction: NavigationAction.Cancel

		onNavigationActionClicked: root.pairingFailed()
	}

	onPasswordEntered: push(progressView)

	Component {
		id: progressView

		ProgressView {
			//: LABEL DESKTOP
			headline: qsTr("Pairing the device ...")
			progress: root.progress
			title: root.title

			Connections {
				function onFirePairingFailed(pDeviceName, pErrorMessage) {
					root.replace(pairingFailedView, {
						deviceName: pDeviceName,
						errorMessage: pErrorMessage
					});
				}
				function onFirePairingSuccess(pDeviceName) {
					if (root.showSuccessView) {
						root.replace(pairingSuccessView, {
							deviceName: pDeviceName
						});
					} else {
						root.pop();
						root.pairingSuccessful();
					}
				}

				target: RemoteServiceModel
			}
		}
	}
	Component {
		id: pairingFailedView

		PairingFailedView {
			progress: root.progress
			title: root.title

			onLeaveView: {
				root.pop();
				root.pairingFailed();
			}
		}
	}
	Component {
		id: pairingSuccessView

		PairingSuccessView {
			progress: root.progress
			title: root.title

			onLeaveView: {
				root.pop();
				root.pairingSuccessful();
			}
		}
	}
}
