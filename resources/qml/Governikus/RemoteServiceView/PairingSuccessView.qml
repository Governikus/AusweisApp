/**
 * Copyright (c) 2023-2024 Governikus GmbH & Co. KG, Germany
 */

import Governikus.ResultView

ResultView {
	id: root

	required property string deviceName

	icon: "qrc:///images/phone_to_pc.svg"

	//: INFO ALL_PLATFORMS The pairing with the SaC server was successful.
	text: qsTr("Pairing with %1 successful, it will be listed under \"Paired devices\".").arg(deviceName) + "<br/>" + qsTr("The device may now be used as a card reader.")
}
