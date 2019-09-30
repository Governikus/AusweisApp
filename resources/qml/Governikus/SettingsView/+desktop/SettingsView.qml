/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
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
import Governikus.Type.NumberModel  1.0


SectionPage {
	id: sectionPage

	enum SubView {
		None,
		EnterPassword,
		PairingInfo
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

	isAbstract: d.view !== SettingsView.SubView.None

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP_QML
		text: qsTr("Settings") + SettingsModel.translationTrigger

		onClicked: {
			d.view = SettingsView.SubView.None
		}
	}

	QtObject {
		id: d

		property int view: SettingsView.SubView.None
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

			if (plugin.developerBuild) {
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
				}
			}
			Component {
				CardReaderView {
					width: parent.width
					height: Math.max(implicitHeight, tabbedPane.availableHeight)
				}
			}
			Component { SecurityAndPrivacySettings {} }
			Component { DeveloperSettings {} }
		}
	}

	EnterPasswordView {
		id: enterPassword

		visible: d.view === SettingsView.SubView.EnterPassword

		statusIcon: "qrc:///images/phone_to_pc.svg"
		passwordType: NumberModel.PASSWORD_REMOTE_PIN

		onPasswordEntered: d.view = SettingsView.SubView.None

		onRequestPasswordInfo: {
			d.view = SettingsView.SubView.PairingInfo
			appWindow.menuBar.updateActions()
		}
	}

	PasswordInfoView {
		id: passwordInfoView

		visible: d.view === SettingsView.SubView.PairingInfo

		passwordType: NumberModel.PASSWORD_REMOTE_PIN

		onClose: {
			d.view = SettingsView.SubView.EnterPassword
			appWindow.menuBar.updateActions()
		}
	}
}
