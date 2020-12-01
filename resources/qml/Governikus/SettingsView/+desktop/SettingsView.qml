/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQml.Models 2.12

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

	Accessible.name: qsTr("Settings view")
	Accessible.description: qsTr("This is the settings panel of the AusweisApp2.")
	Keys.onEscapePressed: {
		if (d.view === SettingsView.SubView.None) {
			event.accepted = false
			return
		}

		d.view = SettingsView.SubView.None
	}

	titleBarAction: TitleBarAction {
		id: titleBarAction

		//: LABEL DESKTOP_QML
		text: qsTr("Settings")
		helpTopic: Utils.helpTopicOf(tabbedPane.currentContentItem, "settings")

		onClicked: {
			d.view = SettingsView.SubView.None
		}

		customSubAction: CancelAction {
			visible: d.view === SettingsView.SubView.EnterPassword || d.view === SettingsView.SubView.WaitForPairing

			onClicked: d.view = SettingsView.SubView.None
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
				qsTr("General"),
				//: LABEL DESKTOP_QML
				qsTr("Smartphone as card reader"),
				//: LABEL DESKTOP_QML
				qsTr("USB card reader"),
				//: LABEL DESKTOP_QML
				qsTr("Security and privacy")
			]

			if (plugin.debugBuild) {
				model.push(
					//: LABEL DESKTOP_QML
					qsTr("Debug options")
				)
			}

			if (SettingsModel.developerOptions) {
				model.push(
					//: LABEL DESKTOP_QML
					qsTr("Developer options")
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
							d.view = SettingsView.SubView.ConnectSacView
							appWindow.menuBar.updateActions()
						}
					}
					onUnpairDevice: RemoteServiceModel.forgetDevice(pDeviceId)
					onMoreInformation: {
						d.precedingView = d.view
						d.view = TabbedReaderView.SubView.ConnectSacView
						connectSacView.showPairingInformation()
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

	ConnectSacView {
		id: connectSacView

		visible: d.view === SettingsView.SubView.ConnectSacView

		rootEnabled: titleBarAction.rootEnabled

		onCloseView: d.view = SettingsView.SubView.None
	}
}
