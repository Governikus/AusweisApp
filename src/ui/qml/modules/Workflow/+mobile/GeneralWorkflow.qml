/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

import Governikus.Animations
import Governikus.TitleBar
import Governikus.Workflow
import Governikus.RemoteServiceView
import Governikus.ResultView
import Governikus.View
import Governikus.Type

SectionPage {
	id: root

	property alias autoInsertCard: technologyInfo.autoInsertCard
	property alias cardInitiallyAppeared: technologyInfo.cardInitiallyAppeared
	property bool hideSwitch: false
	property var initialPlugin: null
	readonly property bool isLandscape: ApplicationWindow.window && ApplicationWindow.menuBar ? ApplicationWindow.window.height - ApplicationWindow.menuBar.height < ApplicationWindow.window.width : false
	required property var workflowModel
	property string workflowTitle

	contentIsScrolled: technologyInfo.contentIsScrolled
	title: workflowTitle

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Cancel

		onClicked: root.workflowModel.cancelWorkflow()
	}

	Component.onCompleted: {
		if (initialPlugin != null) {
			root.workflowModel.readerPluginType = initialPlugin;
		}
	}

	Connections {
		function onReaderPluginTypeChanged() {
			root.updateFocus();
		}

		target: root.workflowModel
	}
	WorkflowInfoList {
		id: technologyInfo

		anchors.fill: parent
		workflowModel: root.workflowModel

		onRemoteDeviceUnpaired: pDeviceName => {
			root.push(deviceUnpairedView, {
				deviceName: pDeviceName
			});
		}
		onShowNfcInformation: root.push(nfcConnectionInfoView)
		onShowRemoteServiceSettings: pEnableScan => {
			root.push(remoteServiceSettings, {
				enableScan: pEnableScan
			});
		}

		Component {
			id: deviceUnpairedView

			ResultView {
				property string deviceName

				animationSymbol: Symbol.Type.ERROR
				animationType: AnimationLoader.SAC_RESULT
				//: INFO ANDROID IOS The paired smartphone was removed since it did not respond to connection attempts. It needs to be paired again before using it.
				text: qsTr("The device \"%1\" was unpaired because it did not react to connection attempts. Pair the device again to use it as a card reader.").arg(deviceName)
				title: root.workflowTitle

				onCancelClicked: root.pop()
				onContinueClicked: root.pop()
			}
		}
		Component {
			id: remoteServiceSettings

			RemoteServiceSettings {
				property bool enableScan: false
				property bool stopScan: true

				allowUsage: true

				Component.onCompleted: if (enableScan)
					RemoteServiceModel.startDetection()
				Component.onDestruction: if (enableScan)
					RemoteServiceModel.stopDetection(stopScan)
				onPairedDeviceFound: {
					stopScan = false;
					root.pop(root);
					SettingsModel.preferredTechnology = ReaderManagerPluginType.REMOTE_IFD;
					root.workflowModel.readerPluginType = ReaderManagerPluginType.REMOTE_IFD;
				}
			}
		}
		Component {
			id: nfcConnectionInfoView

			NfcConnectionInfoView {
				onShowRemoteServiceSettings: root.push(remoteServiceSettings, {
					enableScan: true
				})
			}
		}
	}
}
