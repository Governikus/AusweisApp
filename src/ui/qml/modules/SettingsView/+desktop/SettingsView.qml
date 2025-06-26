/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQml.Models
import Governikus.Global
import Governikus.MultiInfoView
import Governikus.TitleBar
import Governikus.Type
import Governikus.UpdateView
import Governikus.View

SectionPage {
	id: root

	//: LABEL DESKTOP
	title: qsTr("Settings")

	titleBarSettings: TitleBarSettings {
		navigationAction: NavigationAction.Back

		onNavigationActionClicked: root.pop()
	}

	Keys.onPressed: event => {
		tabbedPane.handleKeyPress(event.key);
	}

	Connections {
		function onFireAppUpdateDataChanged(pAfterManualRequest) {
			if (pAfterManualRequest) {
				root.push(updateView);
			}
		}

		target: SettingsModel
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
			if (UiPluginModel.debugBuild) {
				//: LABEL DESKTOP
				model.push(qsTr("Debug options"));
			}
			if (SettingsModel.developerOptions) {
				//: LABEL DESKTOP
				model.push(qsTr("Developer options"));
			}
			return model;
		}

		contentObjectModel: ObjectModel {
			Component {
				GeneralSettings {
				}
			}
			Component {
				RemoteReaderView {
					onPairDevice: pDeviceId => {
						if (RemoteServiceModel.rememberServer(pDeviceId)) {
							root.push(connectSacView);
						}
					}
					onShowNoSacFoundInfo: root.push(noSacFoundInfo)

					Component {
						id: noSacFoundInfo

						MultiInfoView {
							progress: root.progress

							infoContent: MultiInfoData {
								contentType: MultiInfoData.NO_SAC_FOUND
							}
							titleBarSettings: TitleBarSettings {
								navigationAction: NavigationAction.Back

								onNavigationActionClicked: root.pop()
							}
						}
					}
					Component {
						id: connectSacView

						ConnectSacView {
							onPairingFailed: root.pop()
							onPairingSuccessful: root.pop()
						}
					}
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
			if (UiPluginModel.debugBuild) {
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
	Component {
		id: updateView

		UpdateView {
			onLeaveView: root.pop()
		}
	}
}
