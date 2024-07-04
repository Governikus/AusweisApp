/**
 * Copyright (c) 2023-2024 Governikus GmbH & Co. KG, Germany
 */

import Governikus.Style
import Governikus.ResultView

ResultView {
	id: root

	required property string deviceName
	required property string errorMessage

	icon: "qrc:///images/workflow_error_sak_connection_%1.svg".arg(Style.currentTheme.name)

	//: ERROR ALL_PLATFORMS An error occurred while pairing the device.
	text: qsTr("Pairing to \"%1\" failed:").arg(deviceName) + "<br/>%1".arg(errorMessage)
}
