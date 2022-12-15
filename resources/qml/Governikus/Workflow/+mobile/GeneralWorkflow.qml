/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.TechnologyInfo 1.0
import Governikus.TitleBar 1.0
import Governikus.Workflow 1.0
import Governikus.View 1.0
import Governikus.Type.ReaderPlugIn 1.0

SectionPage {
	id: baseItem

	property var workflowModel
	property string workflowTitle

	title: workflowTitle

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Cancel

		onClicked: workflowModel.cancelWorkflow()
	}

	NfcWorkflow {
		visible: workflowModel.readerPlugInType === ReaderPlugIn.NFC

		onStartScanIfNecessary: workflowModel.startScanIfNecessary()

		anchors {
			bottom: technologySwitch.top
			left: parent.left
			right: parent.right
			top: parent.top
		}
	}
	SmartWorkflow {
		visible: workflowModel.readerPlugInType === ReaderPlugIn.SMART
		workflowModel: baseItem.workflowModel

		anchors {
			bottom: technologySwitch.top
			left: parent.left
			right: parent.right
			top: parent.top
		}
	}
	RemoteWorkflow {
		visible: workflowModel.readerPlugInType === ReaderPlugIn.REMOTE_IFD || workflowModel.readerPlugInType === ReaderPlugIn.PCSC

		anchors {
			bottom: technologySwitch.top
			left: parent.left
			right: parent.right
			top: parent.top
		}
	}
	SimulatorWorkflow {
		visible: workflowModel.readerPlugInType === ReaderPlugIn.SIMULATOR
		workflowModel: baseItem.workflowModel

		anchors {
			bottom: technologySwitch.top
			left: parent.left
			right: parent.right
			top: parent.top
		}
	}
	TechnologySwitch {
		id: technologySwitch
		selectedTechnology: workflowModel.readerPlugInType
		supportedTechnologies: workflowModel.supportedPlugInTypes

		onRequestPluginType: pReaderPlugInType => workflowModel.readerPlugInType = pReaderPlugInType

		anchors {
			bottom: parent.bottom
			left: parent.left
			right: parent.right
		}
	}
}
