/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Workflow
import Governikus.Type

Item {
	id: baseItem

	property alias autoInsertCard: smartWorkflow.autoInsertCard
	readonly property bool contentIsScrolled: nfcWorkflow.visible && !nfcWorkflow.atYBeginning || smartWorkflow.visible && !smartWorkflow.atYBeginning || remoteWorkflow.visible && !remoteWorkflow.atYBeginning || simulatorWorkflow.visible && !simulatorWorkflow.atYBeginning
	required property var workflowModel

	signal remoteDeviceUnpaired(var pDeviceName)

	NfcWorkflow {
		id: nfcWorkflow

		anchors.fill: parent
		visible: baseItem.workflowModel.readerPluginType === ReaderManagerPluginType.NFC

		onStartScanIfNecessary: baseItem.workflowModel.startScanExplicitly()
	}
	SmartWorkflow {
		id: smartWorkflow

		anchors.fill: parent
		visible: baseItem.workflowModel.readerPluginType === ReaderManagerPluginType.SMART
		workflowModel: baseItem.workflowModel
	}
	RemoteWorkflow {
		id: remoteWorkflow

		anchors.fill: parent
		visible: baseItem.workflowModel.readerPluginType === ReaderManagerPluginType.REMOTE_IFD || baseItem.workflowModel.readerPluginType === ReaderManagerPluginType.PCSC

		onDeviceUnpaired: pDeviceName => baseItem.remoteDeviceUnpaired(pDeviceName)
	}
	SimulatorWorkflow {
		id: simulatorWorkflow

		anchors.fill: parent
		visible: baseItem.workflowModel.readerPluginType === ReaderManagerPluginType.SIMULATOR
		workflowModel: baseItem.workflowModel
	}
}
