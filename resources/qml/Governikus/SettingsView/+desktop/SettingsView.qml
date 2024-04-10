/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQml.Models
import Governikus.Global
import Governikus.View
import Governikus.TitleBar
import Governikus.Type.ApplicationModel
import Governikus.Type.SettingsModel
import Governikus.Type.UiModule
import Governikus.Type.RemoteServiceModel
import Governikus.UpdateView

SectionPage {
	id: sectionPage

	enum SubView {
		None,
		ConnectSacView,
		AppUpdateView
	}

	titleBarAction: TitleBarAction {
		id: titleBarAction

		//: LABEL DESKTOP
		text: qsTr("Settings")

		onClicked: {
			d.view = SettingsView.SubView.None;
		}
	}

	Keys.onEscapePressed: event => {
		if (d.view === SettingsView.SubView.None) {
			event.accepted = false;
			return;
		}
		d.view = SettingsView.SubView.None;
	}

	Connections {
		function onFireAppUpdateDataChanged() {
			d.view = SettingsView.SubView.AppUpdateView;
		}

		target: SettingsModel
	}
	QtObject {
		id: d

		property int precedingView
		property int view: SettingsView.SubView.None
	}
	TabbedPane {
		id: tabbedPane

		anchors.fill: parent
		sectionsModel: {
			let model = [
				//: LABEL DESKTOP
				qsTr("General"),
				//: LABEL DESKTOP
				qsTr("Smartphone as card reader"),
				//: LABEL DESKTOP
				qsTr("USB card reader"),
				//: LABEL DESKTOP
				qsTr("Security and privacy")];
			if (plugin.debugBuild) {
				model.push(
					//: LABEL DESKTOP
					qsTr("Debug options"));
			}
			if (SettingsModel.developerOptions) {
				model.push(
					//: LABEL DESKTOP
					qsTr("Developer options"));
			}
			return model;
		}
		visible: d.view === SettingsView.SubView.None

		contentObjectModel: ObjectModel {
			Component {
				GeneralSettings {
				}
			}
			Component {
				RemoteReaderView {
					onPairDevice: pDeviceId => {
						if (RemoteServiceModel.rememberServer(pDeviceId)) {
							d.view = SettingsView.SubView.ConnectSacView;
							updateTitleBarActions();
						}
					}
					onUnpairDevice: pDeviceId => RemoteServiceModel.forgetDevice(pDeviceId)
				}
			}
			Component {
				CardReaderView {
				}
			}
			Component {
				SecurityAndPrivacySettings {
				}
			}
		}

		Component.onCompleted: {
			if (plugin.debugBuild) {
				contentObjectModel.append(debugSettings);
			}
			if (SettingsModel.developerOptions) {
				contentObjectModel.append(developerSettings);
			}
		}

		Component {
			id: debugSettings

			DebugSettings {
			}
		}
		Component {
			id: developerSettings

			DeveloperSettings {
			}
		}
	}
	UpdateView {
		anchors.fill: parent
		visible: d.view === SettingsView.SubView.AppUpdateView

		onLeaveView: d.view = SettingsView.SubView.None
	}
	ConnectSacView {
		id: connectSacView

		rootEnabled: titleBarAction.rootEnabled
		visible: d.view === SettingsView.SubView.ConnectSacView

		onPairingFailed: d.view = SettingsView.SubView.None
		onPairingSuccessful: d.view = SettingsView.SubView.None
	}
}
