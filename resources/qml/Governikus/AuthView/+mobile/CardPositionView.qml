/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.ResultView
import Governikus.Style

ResultView {
	id: root

	//: LABEL ANDROID IOS
	buttonText: qsTr("Retry")
	icon: "qrc:///images/workflow_error_nfc_%1.svg".arg(Style.currentTheme.name)
	//: INFO ANDROID IOS The NFC signal is weak, by repositioning the card the signal might improve.
	text: qsTr("Weak NFC signal. Please\n- change the card position\n- remove the mobile phone case (if present)\n- connect the smartphone with a charging cable")
}
