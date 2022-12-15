/*
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.ResultView 1.0
import Governikus.Type.ConnectivityManager 1.0

ResultView {
	id: root
	//: LABEL ANDROID IOS
	buttonText: qsTr("Retry")
	resultType: ResultView.Type.IsInfo
	//: INFO ANDROID IOS The NFC signal is weak, by repositioning the card the signal might improve.
	text: qsTr("Weak NFC signal. Please\n- change the card position\n- remove the mobile phone case (if present)\n- connect the smartphone with a charging cable")
}
