/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.Workflow 1.0
import Governikus.View 1.0
import Governikus.Type.ReaderPlugIn 1.0

SectionPage
{
	QtObject {
		id: d
		readonly property int readerPlugInType: workflowModel.readerPlugInType
	}

	id: baseItem

	property var controller
	property var workflowModel
	property string workflowTitle

	property int waitingFor: 0

	navigationAction: NavigationAction {
		enabled: baseItem.waitingFor !== Workflow.WaitingFor.Password
		state: enabled ? "cancel" : "hidden"
		onClicked: workflowModel.cancelWorkflow()
	}
	title: workflowTitle

	NfcWorkflow
	{
		anchors.fill: parent
		visible: d.readerPlugInType === ReaderPlugIn.NFC
		onRequestPluginType: workflowModel.readerPlugInType = pReaderPlugInType;
	}

	RemoteWorkflow
	{
		anchors.fill: parent
		visible: d.readerPlugInType === ReaderPlugIn.REMOTE || d.readerPlugInType === ReaderPlugIn.PCSC
		onRequestPluginType: workflowModel.readerPlugInType = pReaderPlugInType;
	}

	BluetoothWorkflow
	{
		anchors.fill: parent
		waitingFor: baseItem.waitingFor
		visible: d.readerPlugInType === ReaderPlugIn.BLUETOOTH
		onLocationPermissionInfoConfirmedChanged: controller.locationPermissionConfirmed = locationPermissionInfoConfirmed
		onContinueWorkflow: workflowModel.continueWorkflow()
		onRequestPluginType: workflowModel.readerPlugInType = pReaderPlugInType;
	}
}
