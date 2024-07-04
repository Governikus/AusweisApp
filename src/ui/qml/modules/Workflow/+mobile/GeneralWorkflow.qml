/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.TechnologyInfo
import Governikus.TitleBar
import Governikus.Workflow
import Governikus.ResultView
import Governikus.Style
import Governikus.View
import Governikus.Type

SectionPage {
	id: baseItem

	property alias autoInsertCard: technologyInfo.autoInsertCard
	property bool hideSwitch: false
	property var initialPlugin: null
	readonly property bool isLandscape: ApplicationWindow.window && ApplicationWindow.menuBar ? ApplicationWindow.window.height - ApplicationWindow.menuBar.height < ApplicationWindow.window.width : false
	property alias workflowModel: technologyInfo.workflowModel
	property string workflowTitle

	contentIsScrolled: technologyInfo.contentIsScrolled || isLandscape && !technologySwitch.atYBeginning
	title: workflowTitle

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Cancel

		onClicked: workflowModel.cancelWorkflow()
	}

	Component.onCompleted: {
		if (initialPlugin != null) {
			technologySwitch.requestPluginType(initialPlugin);
		}
	}

	Connections {
		function onReaderPluginTypeChanged() {
			baseItem.updateFocus();
		}

		target: workflowModel
	}
	GridLayout {
		anchors.fill: parent
		columns: 3
		flow: isLandscape ? Flow.LeftToRight : Flow.TopToBottom
		rows: 3

		WorkflowInfoList {
			id: technologyInfo

			Layout.fillHeight: true
			Layout.fillWidth: true

			onRemoteDeviceUnpaired: pDeviceName => {
				baseItem.push(deviceUnpairedView, {
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
					title: baseItem.workflowTitle

					onCancelClicked: baseItem.pop()
					onContinueClicked: baseItem.pop()
				}
			}
		}
		GSeparator {
			id: separator

			readonly property real shorteningFactor: 0.75

			Layout.alignment: Qt.AlignCenter
			Layout.preferredHeight: isLandscape ? parent.height * shorteningFactor : Style.dimens.separator_size
			Layout.preferredWidth: isLandscape ? Style.dimens.separator_size : parent.width * shorteningFactor
			visible: technologySwitch.visible
		}
		TechnologySwitch {
			id: technologySwitch

			Layout.fillHeight: isLandscape
			Layout.fillWidth: !isLandscape
			flowVertically: isLandscape
			selectedTechnology: workflowModel.readerPluginType
			supportedTechnologies: workflowModel.supportedPluginTypes
			visible: !hideSwitch && workflowModel.supportedPluginTypes.length > 1

			onRequestPluginType: pReaderPluginType => workflowModel.readerPluginType = pReaderPluginType
		}
	}
}
