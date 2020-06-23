/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.EnterPasswordView 1.0
import Governikus.ProgressView 1.0
import Governikus.ResultView 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.NumberModel  1.0
import Governikus.Type.RemoteServiceModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.View 1.0

SectionPage {
	id: root

	signal closeView()

	function showPairingInformation() {
		d.view = ConnectSacView.SubView.PairingInfo
		d.externalMoreInformation = true
	}

	enum SubView {
		None,
		EnterPassword,
		PairingInfo,
		WaitForPairing,
		PairingFailed
	}

	QtObject {
		id: d

		property int view
		property bool externalMoreInformation: false
	}

	onVisibleChanged: {
		if (visible) {
			d.view = ConnectSacView.SubView.EnterPassword
		} else {
			d.view = ConnectSacView.SubView.None
			d.externalMoreInformation = false
		}
		appWindow.menuBar.updateActions()
	}

	titleBarAction: TitleBarAction {
		visible: d.view !== ConnectSacView.SubView.PairingInfo || !d.externalMoreInformation
		//: LABEL DESKTOP_QML
		text: qsTr("Pairing") + SettingsModel.translationTrigger
		helpTopic: "readerDeviceTab"
		rootEnabled: false
		customSubAction: CancelAction {
			visible: true
			onClicked: root.closeView()
		}

		onClicked: {
			if (d.view === ConnectSacView.SubView.PairingInfo && !d.externalMoreInformation) {
				d.view = ConnectSacView.SubView.EnterPassword
			}
		}
	}

	EnterPasswordView {
		visible: d.view === ConnectSacView.SubView.EnterPassword

		statusIcon: "qrc:///images/phone_to_pc.svg"
		passwordType: NumberModel.PASSWORD_REMOTE_PIN

		onPasswordEntered: d.view = ConnectSacView.SubView.WaitForPairing

		onRequestPasswordInfo: {
			d.view = ConnectSacView.SubView.PairingInfo
			appWindow.menuBar.updateActions()
		}
	}

	PasswordInfoView {
		visible: d.view === ConnectSacView.SubView.PairingInfo

		passwordType: NumberModel.PASSWORD_REMOTE_PIN
		titleBarAction.rootEnabled: ApplicationModel.currentWorkflow === "" && d.externalMoreInformation

		onClose: {
			if (d.externalMoreInformation) {
				root.closeView()
				d.externalMoreInformation = false
			} else {
				d.view = ConnectSacView.SubView.EnterPassword
			}
			appWindow.menuBar.updateActions()
		}
	}

	ProgressView {
		visible: d.view === ConnectSacView.SubView.WaitForPairing

		//: LABEL DESKTOP_QML
		text: qsTr("Pairing the device ...") + SettingsModel.translationTrigger

		Connections {
			enabled: visible
			target: RemoteServiceModel

			onFirePairingFailed: {
				pairingFailedView.deviceName = pDeviceName
				pairingFailedView.errorMessage = pErrorMessage
				d.view = ConnectSacView.SubView.PairingFailed
			}

			onFirePairingSuccess: {
				ApplicationModel.showFeedback(qsTr("The device \"%1\" has been paired.").arg(pDeviceName))
				root.closeView()
			}
		}
	}

	ResultView {
		id: pairingFailedView

		property string deviceName
		property string errorMessage

		visible: d.view === ConnectSacView.SubView.PairingFailed

		//: ERROR DESKTOP_QML An error occurred while pairing the device.
		text: qsTr("Pairing to \"%1\" failed:").arg(deviceName) + "<br/>\"%2\"".arg(errorMessage) + SettingsModel.translationTrigger
		resultType: ResultView.Type.IsError
		onNextView: root.closeView()
	}
}