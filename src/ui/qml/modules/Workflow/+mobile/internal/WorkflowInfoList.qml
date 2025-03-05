/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Workflow
import Governikus.Type

Item {
	id: root

	property alias autoInsertCard: smartWorkflow.autoInsertCard
	property alias cardInitiallyAppeared: nfcWorkflow.cardInitiallyAppeared
	readonly property bool contentIsScrolled: nfcWorkflow.visible && !nfcWorkflow.atYBeginning || smartWorkflow.visible && !smartWorkflow.atYBeginning || remoteWorkflow.visible && !remoteWorkflow.atYBeginning || simulatorWorkflow.visible && !simulatorWorkflow.atYBeginning
	required property var workflowModel

	signal remoteDeviceUnpaired(var pDeviceName)
	signal showNfcInformation
	signal showRemoteServiceSettings(bool pEnableScan)

	NfcWorkflow {
		id: nfcWorkflow

		anchors.fill: parent
		visible: root.workflowModel.readerPluginType === ReaderManagerPluginType.NFC

		onShowNfcInformation: root.showNfcInformation()
		onShowRemoteServiceSettings: root.showRemoteServiceSettings(true)
		onStartScanIfNecessary: root.workflowModel.startScanExplicitly()
	}
	SmartWorkflow {
		id: smartWorkflow

		anchors.fill: parent
		visible: root.workflowModel.readerPluginType === ReaderManagerPluginType.SMART
		workflowModel: root.workflowModel
	}
	RemoteWorkflow {
		id: remoteWorkflow

		anchors.fill: parent
		visible: root.workflowModel.readerPluginType === ReaderManagerPluginType.REMOTE_IFD || root.workflowModel.readerPluginType === ReaderManagerPluginType.PCSC

		onDeviceUnpaired: pDeviceName => root.remoteDeviceUnpaired(pDeviceName)
		onShowRemoteServiceSettings: root.showRemoteServiceSettings(false)
	}
	SimulatorWorkflow {
		id: simulatorWorkflow

		anchors.fill: parent
		visible: root.workflowModel.readerPluginType === ReaderManagerPluginType.SIMULATOR
		workflowModel: root.workflowModel
	}
}
