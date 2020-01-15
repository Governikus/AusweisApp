/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.Workflow 1.0
import Governikus.View 1.0
import Governikus.Type.ReaderPlugIn 1.0

SectionPage {
	id: baseItem

	property var controller
	property var workflowModel
	property string workflowTitle
	property int waitingFor: 0

	navigationAction: NavigationAction {
		enabled: baseItem.waitingFor !== Workflow.WaitingFor.Password
		state: enabled ? "cancel" : "hidden"
		onClicked: if (enabled) workflowModel.cancelWorkflow()
	}
	title: workflowTitle

	NfcWorkflow {
		visible: workflowModel.readerPlugInType == ReaderPlugIn.NFC
		anchors.fill: parent

		onRequestPluginType: workflowModel.readerPlugInType = pReaderPlugInType;
	}

	RemoteWorkflow {
		visible: workflowModel.readerPlugInType == ReaderPlugIn.REMOTE || workflowModel.readerPlugInType == ReaderPlugIn.PCSC
		anchors.fill: parent

		onRequestPluginType: workflowModel.readerPlugInType = pReaderPlugInType;
	}

	BluetoothWorkflow {
		visible: workflowModel.readerPlugInType == ReaderPlugIn.BLUETOOTH
		anchors.fill: parent

		waitingFor: baseItem.waitingFor
		onLocationPermissionInfoConfirmedChanged: controller.locationPermissionConfirmed = locationPermissionInfoConfirmed
		onContinueWorkflow: workflowModel.continueWorkflow()
		onRequestPluginType: workflowModel.readerPlugInType = pReaderPlugInType;
	}
}
