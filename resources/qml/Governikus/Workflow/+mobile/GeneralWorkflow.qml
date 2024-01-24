/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
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
import Governikus.Type.ReaderPlugIn

SectionPage {
	id: baseItem

	property alias autoInsertCard: technologyInfo.autoInsertCard
	property bool hideSwitch: false
	property var initialPlugIn: null
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
		if (initialPlugIn != null) {
			technologySwitch.requestPluginType(initialPlugIn);
		}
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
			selectedTechnology: workflowModel.readerPlugInType
			supportedTechnologies: workflowModel.supportedPlugInTypes
			visible: !hideSwitch && workflowModel.supportedPlugInTypes.length > 1

			onRequestPluginType: pReaderPlugInType => workflowModel.readerPlugInType = pReaderPlugInType
		}
	}
}
