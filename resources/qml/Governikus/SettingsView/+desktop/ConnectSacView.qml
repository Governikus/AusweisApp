/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15

import Governikus.EnterPasswordView 1.0
import Governikus.ProgressView 1.0
import Governikus.ResultView 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.PasswordType 1.0
import Governikus.Type.RemoteServiceModel 1.0
import Governikus.View 1.0

SectionPage {
	id: root

	property alias rootEnabled: mainTitleBarAction.rootEnabled

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
		updateTitleBarActions()
	}

	titleBarAction: TitleBarAction {
		id: mainTitleBarAction

		visible: d.view !== ConnectSacView.SubView.PairingInfo || !d.externalMoreInformation
		//: LABEL DESKTOP
		text: qsTr("Pairing")
		helpTopic: "settingsRemoteReader"
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
		passwordType: PasswordType.REMOTE_PIN

		onPasswordEntered: d.view = ConnectSacView.SubView.WaitForPairing

		onRequestPasswordInfo: {
			d.view = ConnectSacView.SubView.PairingInfo
			updateTitleBarActions()
		}
	}

	PasswordInfoView {
		visible: d.view === ConnectSacView.SubView.PairingInfo

		passwordType: PasswordType.REMOTE_PIN
		rootEnabled: mainTitleBarAction.rootEnabled

		onClose: {
			if (d.externalMoreInformation) {
				root.closeView()
				d.externalMoreInformation = false
			} else {
				d.view = ConnectSacView.SubView.EnterPassword
			}
			updateTitleBarActions()
		}
	}

	ProgressView {
		visible: d.view === ConnectSacView.SubView.WaitForPairing

		//: LABEL DESKTOP
		text: qsTr("Pairing the device ...")

		Connections {
			enabled: visible
			target: RemoteServiceModel

			function onFirePairingFailed(pDeviceName, pErrorMessage) {
				pairingFailedView.deviceName = pDeviceName
				pairingFailedView.errorMessage = pErrorMessage
				d.view = ConnectSacView.SubView.PairingFailed
			}

			function onFirePairingSuccess (pDeviceName) {
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

		//: ERROR DESKTOP An error occurred while pairing the device.
		text: qsTr("Pairing to \"%1\" failed:").arg(deviceName) + "<br/>\"%2\"".arg(errorMessage)
		resultType: ResultView.Type.IsError
		onNextView: root.closeView()
	}
}
