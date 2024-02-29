/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.TechnologyInfo
import Governikus.Style

GFlickableColumnLayout {
	id: baseItem

	property var workflowModel

	clip: true
	maximumContentWidth: Style.dimens.max_text_width
	spacing: 0
	topMargin: 0

	Item {
		id: progressIndicator

		Layout.alignment: Qt.AlignCenter
		implicitHeight: Style.dimens.workflow_progress_indicator_size
		implicitWidth: icon.implicitWidth

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

		Layout.alignment: Qt.AlignHCenter
		//: LABEL ANDROID IOS
		enableButtonText: qsTr("Continue")

		//: LABEL ANDROID IOS
		titleText: qsTr("Simulator")

		onEnableClicked: workflowModel.insertSimulator()
	}
}
