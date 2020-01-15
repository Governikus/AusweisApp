/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQml.Models 2.3

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.UiModule 1.0
import Governikus.Type.RemoteServiceModel 1.0
import Governikus.EnterPasswordView 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.ProgressView 1.0
import Governikus.ResultView 1.0
import Governikus.UpdateView 1.0


SectionPage {
	id: sectionPage

	enum SubView {
		None,
		EnterPassword,
		PairingInfo,
		WaitForPairing,
		PairingFailed,
		AppUpdateView
	}

	Accessible.name: qsTr("Settings view") + SettingsModel.translationTrigger
	Accessible.description: qsTr("This is the settings panel of the AusweisApp2.") + SettingsModel.translationTrigger
	Keys.onEscapePressed: {
		if (d.view === SettingsView.SubView.None) {
			event.accepted = false
			return
		}

		d.view = SettingsView.SubView.None
	}

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP_QML
		text: qsTr("Settings") + SettingsModel.translationTrigger
		helpTopic: Utils.helpTopicOf(tabbedPane.currentContentItem, "settings")

		onClicked: {
			d.view = SettingsView.SubView.None
		}
	}

	Connections {
		target: SettingsModel
		onFireAppUpdateDataChanged: d.view = SettingsView.SubView.AppUpdateView
	}

	QtObject {
		id: d

		property int view: SettingsView.SubView.None
		property int precedingView
	}

	TabbedPane {
		id: tabbedPane

		visible: d.view === SettingsView.SubView.None
		anchors.fill: parent
		anchors.margins: Constants.pane_spacing

		sectionsModel: {
			var model = [
				//: LABEL DESKTOP_QML
				qsTr("General") + SettingsModel.translationTrigger,
				//: LABEL DESKTOP_QML
				qsTr("Smartphone as card reader") + SettingsModel.translationTrigger,
				//: LABEL DESKTOP_QML
				qsTr("USB card reader") + SettingsModel.translationTrigger,
				//: LABEL DESKTOP_QML
				qsTr("Security and privacy") + SettingsModel.translationTrigger
			]

			if (plugin.debugBuild) {
				model.push(
					//: LABEL DESKTOP_QML
					qsTr("Debug options") + SettingsModel.translationTrigger
				)
			}

			if (SettingsModel.developerOptions) {
				model.push(
					//: LABEL DESKTOP_QML
					qsTr("Developer options") + SettingsModel.translationTrigger
				)
			}

			return model
		}

		sectionDelegate: TabbedPaneDelegateOneLineText {
			sectionName: model ? model.modelData : ""
		}

		contentObjectModel: ObjectModel {
			Component { GeneralSettings {} }
			Component {
				RemoteReaderView {
					width: parent.width
					height: Math.max(implicitHeight, tabbedPane.availableHeight)
					onPairDevice: {
						if (RemoteServiceModel.rememberServer(pDeviceId)) {
							d.view = TabbedReaderView.SubView.EnterPassword
							appWindow.menuBar.updateActions()
						}
					}
					onUnpairDevice: RemoteServiceModel.forgetDevice(pDeviceId)
					onMoreInformation: {
						d.precedingView = d.view
						d.view = TabbedReaderView.SubView.PairingInfo
						appWindow.menuBar.updateActions()
					}
				}
			}
			Component {
				CardReaderView {
					width: parent.width
					height: Math.max(implicitHeight, tabbedPane.availableHeight)
				}
			}
			Component { SecurityAndPrivacySettings {} }
		}

		Component {
			id: debugSettings
			DebugSettings {}
		}

		Component {
			id: developerSettings
			DeveloperSettings {}
		}

		Component.onCompleted: {
			if (plugin.debugBuild) {
				contentObjectModel.append(debugSettings)
			}
			if (SettingsModel.developerOptions) {
				contentObjectModel.append(developerSettings)
			}
		}
	}

	UpdateView {
		visible: d.view === SettingsView.SubView.AppUpdateView
		anchors.fill: parent

		onLeaveView: d.view = SettingsView.SubView.None
	}

	EnterPasswordView {
		id: enterPassword

		visible: d.view === SettingsView.SubView.EnterPassword

		statusIcon: "qrc:///images/phone_to_pc.svg"
		passwordType: NumberModel.PASSWORD_REMOTE_PIN

		onPasswordEntered: d.view = SettingsView.SubView.WaitForPairing

		onRequestPasswordInfo: {
			d.precedingView = d.view
			d.view = SettingsView.SubView.PairingInfo
			appWindow.menuBar.updateActions()
		}
	}

	PasswordInfoView {
		id: passwordInfoView

		visible: d.view === SettingsView.SubView.PairingInfo

		passwordType: NumberModel.PASSWORD_REMOTE_PIN

		onClose: {
			d.view = d.precedingView
			appWindow.menuBar.updateActions()
		}
	}

	ProgressView {
		visible: d.view === SettingsView.SubView.WaitForPairing

		//: LABEL DESKTOP_QML
		text: qsTr("Pairing the device ...") + SettingsModel.translationTrigger

		Connections {
			enabled: visible
			target: RemoteServiceModel

			onFirePairingFailed: {
				pairingFailedView.deviceName = pDeviceName
				pairingFailedView.errorMessage = pErrorMessage
				d.view = SettingsView.SubView.PairingFailed
			}

			onFirePairingSuccess: {
				ApplicationModel.showFeedback(qsTr("The device \"%1\" has been paired.").arg(pDeviceName))
				d.view = SettingsView.SubView.None
			}
		}
	}

	ResultView {
		id: pairingFailedView

		property string deviceName
		property string errorMessage

		visible: d.view === SettingsView.SubView.PairingFailed

		//: ERROR DESKTOP_QML An error occurred while pairing the device.
		text: qsTr("Pairing to \"%1\" failed:").arg(deviceName) + "<br/>\"%2\"".arg(errorMessage) + SettingsModel.translationTrigger
		resultType: ResultView.Type.IsError
		onNextView: d.view = SettingsView.SubView.None
	}
}
