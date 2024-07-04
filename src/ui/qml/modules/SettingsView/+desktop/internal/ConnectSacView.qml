/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.EnterPasswordView
import Governikus.PasswordInfoView
import Governikus.ProgressView
import Governikus.RemoteServiceView
import Governikus.Style
import Governikus.TitleBar
import Governikus.Type
import Governikus.View

SectionPage {
	id: root

	enum SubView {
		None,
		EnterPassword,
		WaitForPairing,
		PairingFailed,
		PairingSuccess
	}

	property alias rootEnabled: mainTitleBarAction.rootEnabled

	signal pairingFailed
	signal pairingSuccessful

	titleBarAction: TitleBarAction {
		id: mainTitleBarAction

		rootEnabled: false
		//: LABEL DESKTOP
		text: qsTr("Pairing")

		customSubAction: NavigationAction {
			type: d.view === ConnectSacView.SubView.PairingSuccess || d.view === ConnectSacView.SubView.PairingFailed ? NavigationAction.Action.Back : NavigationAction.Action.Cancel
			visible: true

			onClicked: d.view === ConnectSacView.SubView.PairingSuccess ? root.pairingSuccessful() : root.pairingFailed()
		}
	}

	onVisibleChanged: {
		if (visible) {
			d.view = ConnectSacView.SubView.EnterPassword;
		} else {
			d.view = ConnectSacView.SubView.None;
		}
		updateTitleBarActions();
	}

	QtObject {
		id: d

		property int view
	}
	EnterPasswordView {
		passwordType: NumberModel.PasswordType.REMOTE_PIN
		visible: d.view === ConnectSacView.SubView.EnterPassword

		onPasswordEntered: d.view = ConnectSacView.SubView.WaitForPairing
	}
	ProgressView {

		//: LABEL DESKTOP
		text: qsTr("Pairing the device ...")
		visible: d.view === ConnectSacView.SubView.WaitForPairing

		Connections {
			function onFirePairingFailed(pDeviceName, pErrorMessage) {
				pairingFailedView.deviceName = pDeviceName;
				pairingFailedView.errorMessage = pErrorMessage;
				d.view = ConnectSacView.SubView.PairingFailed;
			}
			function onFirePairingSuccess(pDeviceName) {
				pairingSuccessView.deviceName = pDeviceName;
				d.view = ConnectSacView.SubView.PairingSuccess;
			}

			enabled: visible
			target: RemoteServiceModel
		}
	}
	PairingFailedView {
		id: pairingFailedView

		deviceName: ""
		errorMessage: ""
		visible: d.view === ConnectSacView.SubView.PairingFailed

		onNextView: root.pairingFailed()
	}
	PairingSuccessView {
		id: pairingSuccessView

		deviceName: ""
		visible: d.view === ConnectSacView.SubView.PairingSuccess

		onNextView: root.pairingSuccessful()
	}
}
