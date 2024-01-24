/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Workflow
import Governikus.ResultView
import Governikus.Style
import Governikus.Type.ReaderPlugIn

Item {
	id: baseItem

	property alias autoInsertCard: smartWorkflow.autoInsertCard
	readonly property bool contentIsScrolled: nfcWorkflow.visible && !nfcWorkflow.atYBeginning || smartWorkflow.visible && !smartWorkflow.atYBeginning || remoteWorkflow.visible && !remoteWorkflow.atYBeginning || simulatorWorkflow.visible && !simulatorWorkflow.atYBeginning
	required property var workflowModel

	NfcWorkflow {
		id: nfcWorkflow

		anchors.fill: parent
		visible: baseItem.workflowModel.readerPlugInType === ReaderPlugIn.NFC

		onStartScanIfNecessary: baseItem.workflowModel.startScanExplicitly()
	}
	SmartWorkflow {
		id: smartWorkflow

		anchors.fill: parent
		visible: baseItem.workflowModel.readerPlugInType === ReaderPlugIn.SMART
		workflowModel: baseItem.workflowModel
	}
	RemoteWorkflow {
		id: remoteWorkflow

		anchors.fill: parent
		visible: baseItem.workflowModel.readerPlugInType === ReaderPlugIn.REMOTE_IFD || baseItem.workflowModel.readerPlugInType === ReaderPlugIn.PCSC

		onDeviceUnpaired: function (pDeviceName) {
			push(deviceUnpairedView, {
					"deviceName": pDeviceName
				});
		}

		Component {
			id: deviceUnpairedView

			ResultView {
				property string deviceName

				icon: "qrc:///images/workflow_error_no_sak_%1.svg".arg(Style.currentTheme.name)
				//: INFO ANDROID IOS The paired smartphone was removed since it did not respond to connection attempts. It needs to be paired again before using it.
				text: qsTr("The device \"%1\" was unpaired because it did not react to connection attempts. Pair the device again to use it as a card reader.").arg(deviceName)
				title: workflowTitle

				onCancelClicked: pop()
				onContinueClicked: pop()
			}
		}
	}
	SimulatorWorkflow {
		id: simulatorWorkflow

		anchors.fill: parent
		visible: baseItem.workflowModel.readerPlugInType === ReaderPlugIn.SIMULATOR
		workflowModel: baseItem.workflowModel
	}
}
