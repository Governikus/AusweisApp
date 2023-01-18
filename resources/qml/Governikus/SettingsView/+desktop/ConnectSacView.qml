/*
 * \copyright Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
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
	enum SubView {
		None,
		EnterPassword,
		PairingInfo,
		WaitForPairing,
		PairingFailed
	}

	property alias rootEnabled: mainTitleBarAction.rootEnabled

	signal closeView

	function showPairingInformation() {
		d.view = ConnectSacView.SubView.PairingInfo;
		d.externalMoreInformation = true;
	}

	titleBarAction: TitleBarAction {
		id: mainTitleBarAction
		helpTopic: "settingsRemoteReader"
		rootEnabled: false
		//: LABEL DESKTOP
		text: qsTr("Pairing")
		visible: d.view !== ConnectSacView.SubView.PairingInfo || !d.externalMoreInformation

		customSubAction: CancelAction {
			visible: true

			onClicked: root.closeView()
		}

		onClicked: {
			if (d.view === ConnectSacView.SubView.PairingInfo && !d.externalMoreInformation) {
				d.view = ConnectSacView.SubView.EnterPassword;
			}
		}
	}

	onVisibleChanged: {
		if (visible) {
			d.view = ConnectSacView.SubView.EnterPassword;
		} else {
			d.view = ConnectSacView.SubView.None;
			d.externalMoreInformation = false;
		}
		updateTitleBarActions();
	}

	QtObject {
		id: d

		property bool externalMoreInformation: false
		property int view
	}
	EnterPasswordView {
		passwordType: PasswordType.REMOTE_PIN
		statusIcon: "qrc:///images/phone_to_pc.svg"
		visible: d.view === ConnectSacView.SubView.EnterPassword

		onPasswordEntered: d.view = ConnectSacView.SubView.WaitForPairing
		onRequestPasswordInfo: {
			d.view = ConnectSacView.SubView.PairingInfo;
			updateTitleBarActions();
		}
	}
	PasswordInfoView {
		passwordType: PasswordType.REMOTE_PIN
		rootEnabled: mainTitleBarAction.rootEnabled
		visible: d.view === ConnectSacView.SubView.PairingInfo

		onClose: {
			if (d.externalMoreInformation) {
				root.closeView();
				d.externalMoreInformation = false;
			} else {
				d.view = ConnectSacView.SubView.EnterPassword;
			}
			updateTitleBarActions();
		}
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
				ApplicationModel.showFeedback(qsTr("The device \"%1\" has been paired.").arg(pDeviceName));
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

		resultType: ResultView.Type.IsError

		//: ERROR DESKTOP An error occurred while pairing the device.
		text: qsTr("Pairing to \"%1\" failed:").arg(deviceName) + "<br/>\"%2\"".arg(errorMessage)
		visible: d.view === ConnectSacView.SubView.PairingFailed

		onNextView: root.closeView()
	}
}
