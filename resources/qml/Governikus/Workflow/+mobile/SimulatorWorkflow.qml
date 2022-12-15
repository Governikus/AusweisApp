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
		height: parent.height / 2

		anchors {
			left: parent.left
			right: parent.right
			top: parent.top
		}
		TintableIcon {
			id: icon
			anchors.centerIn: parent
			desaturate: true
			height: Math.ceil(parent.height * 0.25) * 2
			source: "qrc:///images/mobile/phone_simulator.svg"
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
