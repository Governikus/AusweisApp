/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.EnterPasswordView
import Governikus.PasswordInfoView
import Governikus.ProgressView
import Governikus.ResultView
import Governikus.Style
import Governikus.TitleBar
import Governikus.Type.ApplicationModel
import Governikus.Type.PasswordType
import Governikus.Type.RemoteServiceModel
import Governikus.View

SectionPage {
	id: root

	enum SubView {
		None,
		EnterPassword,
		WaitForPairing,
		PairingFailed
	}

	property alias rootEnabled: mainTitleBarAction.rootEnabled

	signal closeView

	titleBarAction: TitleBarAction {
		id: mainTitleBarAction

		rootEnabled: false
		//: LABEL DESKTOP
		text: qsTr("Pairing")

		customSubAction: NavigationAction {
			visible: true

			onClicked: root.closeView()
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
		passwordType: PasswordType.REMOTE_PIN
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
			function onFirePairingSuccess() {
				root.closeView();
			}

			enabled: visible
			target: RemoteServiceModel
		}
	}
	ResultView {
		id: pairingFailedView

		property string deviceName
		property string errorMessage

		icon: "qrc:///images/desktop/workflow_error_sak_connection_%1.svg".arg(Style.currentTheme.name)

		//: ERROR DESKTOP An error occurred while pairing the device.
		text: qsTr("Pairing to \"%1\" failed:").arg(deviceName) + "<br/>\"%2\"".arg(errorMessage)
		visible: d.view === ConnectSacView.SubView.PairingFailed

		onNextView: root.closeView()
	}
}
