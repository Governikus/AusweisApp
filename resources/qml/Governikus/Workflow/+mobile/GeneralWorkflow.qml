/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.TechnologyInfo 1.0
import Governikus.TitleBar 1.0
import Governikus.Workflow 1.0
import Governikus.ResultView 1.0
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

	Item {
		anchors {
			bottom: technologySwitch.visible ? technologySwitch.top : parent.bottom
			left: parent.left
			right: parent.right
			top: parent.top
		}
		NfcWorkflow {
			anchors.fill: parent
			visible: workflowModel.readerPlugInType === ReaderPlugIn.NFC

			onStartScanIfNecessary: workflowModel.startScanExplicitly()
		}
		SmartWorkflow {
			anchors.fill: parent
			visible: workflowModel.readerPlugInType === ReaderPlugIn.SMART
			workflowModel: baseItem.workflowModel
		}
		RemoteWorkflow {
			anchors.fill: parent
			visible: workflowModel.readerPlugInType === ReaderPlugIn.REMOTE_IFD || workflowModel.readerPlugInType === ReaderPlugIn.PCSC

			onDeviceUnpaired: function (pDeviceName) {
				push(deviceUnpairedView, {
						"deviceName": pDeviceName
					});
			}

			Component {
				id: deviceUnpairedView
				ResultView {
					property string deviceName

					resultType: ResultView.Type.IsError
					//: INFO ANDROID IOS The paired smartphone was removed since it did not respond to connection attempts. It needs to be paired again before using it.
					text: qsTr("The device \"%1\" was unpaired because it did not react to connection attempts. Pair the device again to use it as a card reader.").arg(deviceName)
					title: workflowTitle

					onCancelClicked: pop()
					onContinueClicked: pop()
				}
			}
		}
		SimulatorWorkflow {
			anchors.fill: parent
			visible: workflowModel.readerPlugInType === ReaderPlugIn.SIMULATOR
			workflowModel: baseItem.workflowModel
		}
	}
	TechnologySwitch {
		id: technologySwitch
		selectedTechnology: workflowModel.readerPlugInType
		supportedTechnologies: workflowModel.supportedPlugInTypes
		visible: workflowModel.supportedPlugInTypes.length > 1

		onRequestPluginType: pReaderPlugInType => workflowModel.readerPlugInType = pReaderPlugInType

		anchors {
			bottom: parent.bottom
			left: parent.left
			right: parent.right
		}
	}
}
