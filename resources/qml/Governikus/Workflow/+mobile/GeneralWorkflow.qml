/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Global 1.0
import Governikus.TechnologyInfo 1.0
import Governikus.TitleBar 1.0
import Governikus.Workflow 1.0
import Governikus.View 1.0
import Governikus.Type.ReaderPlugIn 1.0

SectionPage {
	id: baseItem

	property var controller
	property var workflowModel
	property string workflowTitle

	navigationAction: NavigationAction {
		state: "cancel"
		onClicked: workflowModel.cancelWorkflow()
	}
	title: workflowTitle

	NfcWorkflow {
		visible: workflowModel.readerPlugInType == ReaderPlugIn.NFC
		anchors {
			left: parent.left
			right: parent.right
			top: parent.top
			bottom: technologySwitch.top
		}
		onStartScanIfNecessary: workflowModel.startScanIfNecessary()
	}

	RemoteWorkflow {
		visible: workflowModel.readerPlugInType == ReaderPlugIn.REMOTE || workflowModel.readerPlugInType == ReaderPlugIn.PCSC
		anchors {
			left: parent.left
			right: parent.right
			top: parent.top
			bottom: technologySwitch.top
		}
	}

	TechnologySwitch {
		id: technologySwitch

		anchors {
			left: parent.left
			right: parent.right
			bottom: parent.bottom
		}

		selectedTechnology: workflowModel.readerPlugInType
		onRequestPluginType: workflowModel.readerPlugInType = pReaderPlugInType;
	}
}
