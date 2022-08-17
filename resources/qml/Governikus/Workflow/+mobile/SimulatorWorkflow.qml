/*
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Global 1.0
import Governikus.TechnologyInfo 1.0


Item {
	id: baseItem

	property var workflowModel

	Item {
		id: progressIndicator

		anchors {
			left: parent.left
			top: parent.top
			right: parent.right
		}
		height: parent.height / 2

		TintableIcon {
			id: icon

			anchors.centerIn: parent
			height: Math.ceil(parent.height * 0.25) * 2

			tintEnabled: false
			source: "qrc:///images/mobile/phone_simulator.svg"
			desaturate: true
		}
	}

	TechnologyInfo {
		id: technologyInfo

		anchors {
			left: parent.left
			right: parent.right
			top: progressIndicator.bottom
			bottom: parent.bottom
			leftMargin: Constants.component_spacing
			rightMargin: Constants.component_spacing
		}

		//: LABEL ANDROID IOS
		titleText: qsTr("Simulator")
		//: LABEL ANDROID IOS
		enableButtonText: qsTr("Continue")

		onEnableClicked: workflowModel.insertSimulator()
	}
}
