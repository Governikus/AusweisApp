/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.TechnologyInfo 1.0
import Governikus.Style 1.0

Item {
	id: baseItem

	property var workflowModel

	Item {
		id: progressIndicator
		height: Style.dimens.workflow_progress_indicator_size

		anchors {
			left: parent.left
			right: parent.right
			top: parent.top
		}
		TintableIcon {
			id: icon
			anchors.centerIn: parent
			desaturate: true
			source: "qrc:///images/mobile/phone_simulator.svg"
			sourceSize.height: Style.dimens.header_icon_size
			tintEnabled: false
		}
	}
	TechnologyInfo {
		id: technologyInfo
		//: LABEL ANDROID IOS
		enableButtonText: qsTr("Continue")

		//: LABEL ANDROID IOS
		titleText: qsTr("Simulator")

		onEnableClicked: workflowModel.insertSimulator()

		anchors {
			bottom: parent.bottom
			left: parent.left
			leftMargin: Constants.component_spacing
			right: parent.right
			rightMargin: Constants.component_spacing
			top: progressIndicator.bottom
		}
	}
}
