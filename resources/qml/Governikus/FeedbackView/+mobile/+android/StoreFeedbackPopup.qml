/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0

ConfirmationPopup {
	//: INFO ANDROID Header of the app rating popup.
	title: qsTr("Are you satisfied with AusweisApp2?")
	//: INFO ANDROID Content of the app rating popup.
	text: qsTr("We would be very grateful if you could leave a rating on the Google Play Store!")
	//: LABEL ANDROID
	cancelButtonText: qsTr("Do not ask again")
	//: LABEL ANDROID
	okButtonText: qsTr("Rate app")

	onConfirmed: Qt.openUrlExternally(ApplicationModel.storeUrl)
}
