/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQml.Models 2.15
import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.UiModule 1.0
import Governikus.Type.RemoteServiceModel 1.0
import Governikus.UpdateView 1.0

SectionPage {
	id: sectionPage
	enum SubView {
		None,
		ConnectSacView,
		AppUpdateView
	}

	titleBarAction: TitleBarAction {
		id: titleBarAction
		helpTopic: Utils.helpTopicOf(tabbedPane.currentContentItem, "settings")

		//: LABEL DESKTOP
		text: qsTr("Settings")

		customSubAction: CancelAction {
			visible: d.view === SettingsView.SubView.EnterPassword || d.view === SettingsView.SubView.WaitForPairing

			onClicked: d.view = SettingsView.SubView.None
		}

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
		anchors.margins: Constants.pane_spacing
		sectionsModel: {
			var model = [
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
					height: Math.max(implicitHeight, tabbedPane.availableHeight)
					width: parent.width

					onMoreInformation: {
						d.precedingView = d.view;
						d.view = TabbedReaderView.SubView.ConnectSacView;
						connectSacView.showPairingInformation();
						updateTitleBarActions();
					}
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
					height: Math.max(implicitHeight, tabbedPane.availableHeight)
					width: parent.width
				}
			}
			Component {
				SecurityAndPrivacySettings {
				}
			}
		}
		sectionDelegate: TabbedPaneDelegateText {
			sectionName: model ? model.modelData : ""
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

		onCloseView: d.view = SettingsView.SubView.None
	}
}
